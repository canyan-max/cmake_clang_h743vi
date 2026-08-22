/**
 ******************************************************************************
 *@file               :   mcu_spi.c
 *@brief              :   STM32 implementation of the portable SPI API.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */

#include <stddef.h>
#include <stdint.h>
#include "plat_spi.h"
#include "board_config.h"

#define PLAT_SPI_HAL_MAX_TRANSFER_SIZE  (0xFFFEU)

typedef struct PLAT_SPI_RESOURCE_T
{
    SPI_HandleTypeDef          *p_handle;
    const uint8_t              *p_next_data;
    uint32_t                    remaining_size;
    plat_spi_tx_complete_cb_t   callback;
    void                       *p_callback_context;
    volatile uint8_t            busy;
    volatile uint8_t            aborting;
} plat_spi_resource_t;

typedef struct PLAT_SPI_BLOCKING_WAIT_T
{
    volatile platform_err_t status;
    volatile uint8_t        completed;
} plat_spi_blocking_wait_t;

static plat_spi_resource_t spi_resource_table[PLAT_SPI_ID_NUM] = {
    [PLAT_SPI_ID_0] = {
        .p_handle = &BOARD_SPI0_HANDLE,
    },
};

static plat_spi_resource_t *plat_spi_get_resource(plat_spi_id_t id)
{
    if(id >= PLAT_SPI_ID_NUM)
    {
        return NULL;
    }
    return &spi_resource_table[id];
}

static plat_spi_id_t plat_spi_find_id(const SPI_HandleTypeDef *p_handle)
{
    for(uint32_t i = 0U; i < (uint32_t)PLAT_SPI_ID_NUM; i++)
    {
        if(spi_resource_table[i].p_handle == p_handle)
        {
            return (plat_spi_id_t)i;
        }
    }
    return PLAT_SPI_ID_NUM;
}

static uint8_t plat_spi_timeout_is_valid(uint32_t timeout_ms)
{
    return (uint8_t)((0U != timeout_ms) && (UINT32_MAX != timeout_ms));
}

static platform_err_t plat_spi_convert_hal_status(HAL_StatusTypeDef status)
{
    switch(status)
    {
        case HAL_OK:
            return PLATFORM_ERR_OK;
        case HAL_BUSY:
            return PLATFORM_ERR_BUSY;
        case HAL_TIMEOUT:
            return PLATFORM_ERR_TIMEOUT;
        case HAL_ERROR:
        default:
            return PLATFORM_ERR_HW;
    }
}

static uint32_t plat_spi_get_remaining_timeout(uint32_t start_ms,
                                               uint32_t timeout_ms)
{
    uint32_t elapsed_ms = HAL_GetTick() - start_ms;
    return (elapsed_ms < timeout_ms) ? (timeout_ms - elapsed_ms) : 0U;
}

static void plat_spi_reset_async_state(plat_spi_resource_t *p_resource)
{
    p_resource->p_next_data        = NULL;
    p_resource->remaining_size     = 0U;
    p_resource->callback           = NULL;
    p_resource->p_callback_context = NULL;
    p_resource->aborting           = 0U;
    p_resource->busy               = 0U;
}

static void plat_spi_finish_async_from_isr(plat_spi_id_t  id,
                                           platform_err_t status)
{
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (0U == p_resource->busy))
    {
        return;
    }

    plat_spi_tx_complete_cb_t callback = p_resource->callback;
    void *p_callback_context = p_resource->p_callback_context;
    plat_spi_reset_async_state(p_resource);

    if(NULL != callback)
    {
        callback(id, status, p_callback_context);
    }
}

static platform_err_t plat_spi_start_next_dma_chunk(plat_spi_resource_t *p_resource)
{
    if((NULL == p_resource) || (0U == p_resource->busy) ||
       (NULL == p_resource->p_next_data) ||
       (0U == p_resource->remaining_size))
    {
        return PLATFORM_ERR_PARAM;
    }

    const uint8_t *p_chunk = p_resource->p_next_data;
    uint32_t chunk_size =
        (p_resource->remaining_size > PLAT_SPI_HAL_MAX_TRANSFER_SIZE)
            ? PLAT_SPI_HAL_MAX_TRANSFER_SIZE
            : p_resource->remaining_size;

    SCB_CleanDCache_by_Addr((uint32_t *)(uintptr_t)p_chunk,
                            (int32_t)chunk_size);

    p_resource->p_next_data += chunk_size;
    p_resource->remaining_size -= chunk_size;
    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(
        p_resource->p_handle, (uint8_t *)(uintptr_t)p_chunk,
        (uint16_t)chunk_size);
    if(HAL_OK != status)
    {
        p_resource->p_next_data = p_chunk;
        p_resource->remaining_size += chunk_size;
    }
    return plat_spi_convert_hal_status(status);
}

static void plat_spi_blocking_complete_from_isr(plat_spi_id_t  id,
                                                platform_err_t status,
                                                void          *p_context)
{
    ((void)id);
    plat_spi_blocking_wait_t *p_wait = (plat_spi_blocking_wait_t *)p_context;
    if(NULL != p_wait)
    {
        p_wait->status    = status;
        p_wait->completed = 1U;
    }
}

platform_err_t plat_spi_write(plat_spi_id_t  id,
                              const uint8_t *p_data,
                              uint32_t       size,
                              uint32_t       timeout_ms)
{
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (NULL == p_data) || (0U == size) ||
       (0U == plat_spi_timeout_is_valid(timeout_ms)))
    {
        return PLATFORM_ERR_PARAM;
    }
    if(0U != p_resource->busy)
    {
        return PLATFORM_ERR_BUSY;
    }

    uint32_t start_ms = HAL_GetTick();
    while(size > 0U)
    {
        uint32_t remaining_ms = plat_spi_get_remaining_timeout(start_ms,
                                                               timeout_ms);
        if(0U == remaining_ms)
        {
            return PLATFORM_ERR_TIMEOUT;
        }

        uint16_t chunk_size = (uint16_t)((size > PLAT_SPI_HAL_MAX_TRANSFER_SIZE)
                                             ? PLAT_SPI_HAL_MAX_TRANSFER_SIZE
                                             : size);
        HAL_StatusTypeDef status = HAL_SPI_Transmit(p_resource->p_handle,
                                                    (uint8_t *)(uintptr_t)
                                                        p_data,
                                                    chunk_size, remaining_ms);
        if(HAL_OK != status)
        {
            return plat_spi_convert_hal_status(status);
        }

        p_data += chunk_size;
        size -= chunk_size;
    }
    return PLATFORM_ERR_OK;
}

platform_err_t
plat_spi_write_dma_async(plat_spi_id_t              id,
                         const uint8_t             *p_data,
                         uint32_t                   size,
                         plat_spi_tx_complete_cb_t callback,
                         void                      *p_context)
{
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (NULL == p_data) || (0U == size))
    {
        return PLATFORM_ERR_PARAM;
    }

    uint32_t interrupt_state = __get_PRIMASK();
    __disable_irq();
    if((0U != p_resource->busy) ||
       (HAL_SPI_STATE_READY != HAL_SPI_GetState(p_resource->p_handle)))
    {
        __set_PRIMASK(interrupt_state);
        return PLATFORM_ERR_BUSY;
    }

    p_resource->p_next_data        = p_data;
    p_resource->remaining_size     = size;
    p_resource->callback           = callback;
    p_resource->p_callback_context = p_context;
    p_resource->aborting           = 0U;
    p_resource->busy               = 1U;
    __set_PRIMASK(interrupt_state);

    platform_err_t status = plat_spi_start_next_dma_chunk(p_resource);
    if(PLATFORM_ERR_OK != status)
    {
        plat_spi_reset_async_state(p_resource);
    }
    return status;
}

platform_err_t plat_spi_abort_dma(plat_spi_id_t id)
{
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if(NULL == p_resource)
    {
        return PLATFORM_ERR_PARAM;
    }

    uint32_t interrupt_state = __get_PRIMASK();
    __disable_irq();
    if(0U == p_resource->busy)
    {
        __set_PRIMASK(interrupt_state);
        return PLATFORM_ERR_OK;
    }
    p_resource->aborting = 1U;
    __set_PRIMASK(interrupt_state);

    HAL_StatusTypeDef hal_status = HAL_SPI_Abort(p_resource->p_handle);
    plat_spi_reset_async_state(p_resource);
    return plat_spi_convert_hal_status(hal_status);
}

platform_err_t plat_spi_is_busy(plat_spi_id_t id, uint8_t *p_busy)
{
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (NULL == p_busy))
    {
        return PLATFORM_ERR_PARAM;
    }

    *p_busy = p_resource->busy;
    return PLATFORM_ERR_OK;
}

platform_err_t plat_spi_write_dma_blocking(plat_spi_id_t  id,
                                           const uint8_t *p_data,
                                           uint32_t       size,
                                           uint32_t       timeout_ms)
{
    if((NULL == plat_spi_get_resource(id)) || (NULL == p_data) || (0U == size) ||
       (0U == plat_spi_timeout_is_valid(timeout_ms)))
    {
        return PLATFORM_ERR_PARAM;
    }

    plat_spi_blocking_wait_t wait = {
        .status    = PLATFORM_ERR_HW,
        .completed = 0U,
    };
    uint32_t start_ms = HAL_GetTick();
    platform_err_t status = plat_spi_write_dma_async(
        id, p_data, size, plat_spi_blocking_complete_from_isr, &wait);
    if(PLATFORM_ERR_OK != status)
    {
        return status;
    }

    while(0U == wait.completed)
    {
        if(0U == plat_spi_get_remaining_timeout(start_ms, timeout_ms))
        {
            platform_err_t abort_status = plat_spi_abort_dma(id);
            if(0U != wait.completed)
            {
                return wait.status;
            }
            return (PLATFORM_ERR_OK == abort_status) ? PLATFORM_ERR_TIMEOUT
                                                     : abort_status;
        }
    }
    return wait.status;
}

/**
 * @brief Continue or complete a Platform-owned SPI DMA transfer from ISR.
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *p_hspi)
{
    plat_spi_id_t id = plat_spi_find_id(p_hspi);
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (0U == p_resource->busy) ||
       (0U != p_resource->aborting))
    {
        return;
    }

    if(0U != p_resource->remaining_size)
    {
        platform_err_t status = plat_spi_start_next_dma_chunk(p_resource);
        if(PLATFORM_ERR_OK != status)
        {
            plat_spi_finish_async_from_isr(id, status);
        }
        return;
    }

    plat_spi_finish_async_from_isr(id, PLATFORM_ERR_OK);
}

/**
 * @brief Complete a Platform-owned SPI DMA transfer with an error from ISR.
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *p_hspi)
{
    plat_spi_id_t id = plat_spi_find_id(p_hspi);
    plat_spi_resource_t *p_resource = plat_spi_get_resource(id);
    if((NULL == p_resource) || (0U == p_resource->busy) ||
       (0U != p_resource->aborting))
    {
        return;
    }

    plat_spi_finish_async_from_isr(id, PLATFORM_ERR_HW);
}

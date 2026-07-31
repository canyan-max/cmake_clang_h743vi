/**
 ******************************************************************************
 *@file               :   bsp_at24.h
 *@brief              :   AT24 EEPROM driver 鈥?protocol functions + I2C transport.
 *                        Transport is the ONLY replaceable part (I2C read/write).
 *                        Protocol functions are direct calls, not function pointers.
 *@version            :   V2.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef BSP_DRV_AT24_H
#define BSP_DRV_AT24_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

/* ---- defines ------------------------------------------------------------- */
#define AT24_MEMADD_SIZE_8BIT   (1U)
#define AT24_MEMADD_SIZE_16BIT  (2U)

/* ---- forward decalarations ----------------------------------------------- */
typedef struct AT24_DEV_T      at24_dev_t;

/* ---- state enum ---------------------------------------------------------- */
typedef enum
{
    AT24_OK        = 0x00U,
    AT24_ERROR     = 0x01U,
    AT24_BUSY      = 0x02U,
    AT24_TIMEOUT   = 0x03U,
    AT24_PARAM_ERR = 0x04U,
} at24_state_t;

/* ---- transport (the only replaceable part 鈥?I2C bus) --------------------- */
typedef struct IIC_OPS_T
{
    void *p_iic_handle;
    at24_state_t (*pf_iic_init)(void *p_handle);
    at24_state_t (*pf_iic_deinit)(void *p_handle);
    at24_state_t (*pf_iic_dev_isready)(void *p_handle, uint8_t dev_adr,
                                        uint32_t trials, uint32_t timeout);
    at24_state_t (*pf_mem_read)(void *p_handle, uint16_t dev_adr,
                                 uint16_t mem_adr, uint16_t mem_adr_size,
                                 uint8_t *p_data, uint16_t size, uint32_t timeout);
    at24_state_t (*pf_mem_write)(void *p_handle, uint16_t dev_adr,
                                  uint16_t mem_adr, uint16_t mem_adr_size,
                                  uint8_t *p_data, uint16_t size, uint32_t timeout);
} iic_ops_t;

/* ---- device instance ----------------------------------------------------- */
struct AT24_DEV_T
{
    const iic_ops_t *iic_ops;
    uint32_t          max_byte_addr;
    uint32_t          page_size;
    uint16_t          adr_size;
    uint8_t           dev_adr;
    uint8_t           is_inited;
};

/* ---- protocol functions (direct calls, no ops indirection) --------------- */
at24_state_t at24_write_page(at24_dev_t *p_dev, uint16_t mem_adr,
                              uint8_t *p_data, uint16_t size, uint32_t timeout);
at24_state_t at24_read_bytes(at24_dev_t *p_dev, uint16_t mem_adr,
                              uint8_t *p_data, uint16_t size, uint32_t timeout);
at24_state_t at24_write_byte(at24_dev_t *p_dev, uint16_t mem_adr,
                              uint8_t data, uint32_t timeout);

/* ---- board-level init (wires Impl transport + I2C init) ------------------ */
at24_state_t bsp_at24_init(at24_dev_t *p_dev, uint32_t max_byte_addr,
                             uint32_t page_size, uint16_t adr_size, uint8_t dev_adr);

#ifdef __cplusplus
}
#endif

#endif /* BSP_DRV_AT24_H */

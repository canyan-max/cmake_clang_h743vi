/**
 ******************************************************************************
 *@file               :   ov2640_driver.h
 *@brief              :   Portable OV2640 image sensor driver contract.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef OV2640_DRIVER_H
#define OV2640_DRIVER_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* define -------------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/
typedef enum OV2640_STATUS_T
{
    OV2640_STATUS_OK = 0U,
    OV2640_STATUS_PARAM,
    OV2640_STATUS_NOT_INITIALIZED,
    OV2640_STATUS_ID_MISMATCH,
    OV2640_STATUS_BUSY,
    OV2640_STATUS_TIMEOUT,
    OV2640_STATUS_TRANSPORT,
} ov2640_status_t;

typedef enum OV2640_PROFILE_T
{
    OV2640_PROFILE_RGB565_SVGA = 0U,
    OV2640_PROFILE_NUM,
} ov2640_profile_t;

typedef struct OV2640_TRANSPORT_T
{
    ov2640_status_t (*pf_write_reg)(uint8_t  address_7b,
                                    uint8_t  reg,
                                    uint8_t  value,
                                    uint32_t timeout_ms);
    ov2640_status_t (*pf_read_reg)(uint8_t  address_7b,
                                   uint8_t  reg,
                                   uint8_t *p_value,
                                   uint32_t timeout_ms);
    void (*pf_delay_ms)(uint32_t delay_ms);
} ov2640_transport_t;

typedef struct OV2640_DRIVER_T
{
    const ov2640_transport_t *p_transport;
    uint8_t                   address_7b;
    uint8_t                   initialized;
} ov2640_driver_t;

/* variables ----------------------------------------------------------------*/

/* function  ----------------------------------------------------------------*/
/**
 * @brief Initialize one OV2640 instance and verify its product ID.
 * @param p_driver Instance storage owned by the caller.
 * @param p_transport Immutable host register transport implementation.
 * @param address_7b Unshifted 7-bit SCCB address.
 * @param timeout_ms Finite timeout for each register transaction.
 * @retval OV2640_STATUS_OK on success; otherwise an OV2640 status.
 */
ov2640_status_t ov2640_init(ov2640_driver_t          *p_driver,
                            const ov2640_transport_t *p_transport,
                            uint8_t                    address_7b,
                            uint32_t                   timeout_ms);

/**
 * @brief Apply one supported sensor profile and configure DSP output size.
 * @param p_driver Initialized OV2640 instance.
 * @param profile Supported sensor and output-format profile.
 * @param output_width DSP output width in pixels; must be a multiple of four.
 * @param output_height DSP output height in lines; must be a multiple of four.
 * @param timeout_ms Finite timeout for each register transaction.
 * @retval OV2640_STATUS_OK on success; otherwise an OV2640 status.
 */
ov2640_status_t ov2640_configure(ov2640_driver_t *p_driver,
                                 ov2640_profile_t profile,
                                 uint16_t          output_width,
                                 uint16_t          output_height,
                                 uint32_t          timeout_ms);

/**
 * @brief Read the OV2640 product identifier from an initialized instance.
 * @param p_driver Initialized OV2640 instance.
 * @param p_product_id Destination for the 16-bit product identifier.
 * @param timeout_ms Finite timeout for each register transaction.
 * @retval OV2640_STATUS_OK on success; otherwise an OV2640 status.
 */
ov2640_status_t ov2640_read_product_id(ov2640_driver_t *p_driver,
                                       uint16_t         *p_product_id,
                                       uint32_t          timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* OV2640_DRIVER_H */

/* end of file --------------------------------------------------------------*/


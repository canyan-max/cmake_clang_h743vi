/**
 ******************************************************************************
 *@file               :   eeprom_intf.h
 *@brief              :   EEPROM BSP interface contract consumed by device layer.
 *                        Swap bsp_at24_intf.c to replace EEPROM chip.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!  2026
 ******************************************************************************
 */
#ifndef EEPROM_INTF_H
#define EEPROM_INTF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "plat_error.h"

/* ---- capacity contract (device uses for bounds check) -------------------- */
#define BSP_EEPROM_MAX_BYTE_ADDR  (256U)

/* ---- ops struct ---------------------------------------------------------- */
typedef struct BSP_EEPROM_OPS_T
{
    platform_err_t (*pf_init)(void);
    platform_err_t (*pf_read)(uint16_t mem_adr, uint8_t *p_data,
                              uint16_t size, uint32_t timeout);
    platform_err_t (*pf_write)(uint16_t mem_adr, uint8_t *p_data,
                               uint16_t size, uint32_t timeout);
} bsp_eeprom_ops_t;

extern const bsp_eeprom_ops_t g_eeprom_bsp_ops;

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_INTF_H */

/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   cmds.c
 * 
 *@pardependencies    :
 *                        cmds.c
 *                        shell.h   
 *                        bsp_drv_at24.h
 * 
 *@author             :   null
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@version            :   V1.0 
 *
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                                /* stdint lib header file */
#include "shell.h"                                  /* shell lib header file */
#include "storage_handle.h"                      /* storage_lib header file. */
#include "main.h"
#include "log.h"
#include "core_dwt.h"                                    /* dwt header file. */
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/
extern storage_handle_t g_storage_handle;
/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/

int write_i2c1(uint8_t adr , uint8_t w_code)
{
    uint32_t time = get_dwt_us();
    uint8_t  ret = storage_handle_write(&g_storage_handle, \
                          adr, &w_code, 1, \
                          2);
    time = get_dwt_us() - time;
    logInfo("write_i2c1 time %d us" , time);
    return ret;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 write_i2c1, \
                 write_i2c1, \
                 write_i2c1);



int read_i2c1(uint8_t adr)
{
    uint8_t r_code = 0;
    uint32_t time = get_dwt_us();
    logInfo("read_i2c1 time1 %d us" , time);
    storage_handle_read(&g_storage_handle, \
                          adr, &r_code, 1, \
                          2);
    time = get_dwt_us() - time;
    logInfo("read_i2c1 time2 %d us" , time);
    return r_code;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 read_i2c1, \
                 read_i2c1, \
                 read_i2c1);

int sys_restart(void)
{
    NVIC_SystemReset();
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 sys_restart, \
                 sys_restart, \
                 sys_restart);





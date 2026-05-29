/**
 ******************************************************************************
 *@Copyright          :   (C), Inc.(Gmbh) or its affiliates.AllRights Reserved.
 *
 *@file               :   cmds.c
 * 
 *@pardependencies    :
 *                        cmds.c
 *                        xxx.h   
 *                        xxxx.h
 * 
 *@author             :   null
 * 
 *@brief              :   Provide the HAL APIs of description.
 * 
 *@Processingflow     :
 * 
 *@calldirectly       :
 * 
 *@version            :   V1.0 
 *@note               :   1 tab == 4 spaces!
 *                        global variable format 
 *                        uint8_t  g_xxxx_xxx = 0U;
 *                        uint8_t *p_xxxx_xxx = 0U;
 *                        
 *                        enumeration format 
 *                        typedef enum XXX_XXXX_T
 *                        {
 *                          XXXXX_XXXX              = 0x00U,
 *                        }xxxx_xxxx_t;
 *
 *                        struct format 
 *                        typedef struct XXXXXX_T
 *                        {
 *                          uint8_t xxxx_xxxx;
 *                          uint8_t (*pf_iic_init)(void*);
 *                        }xxxxx_t;
 *
 *                        macro definition format
 *                        #define XXXX_XXXX
 *                        #define XXXX_XXXX         (0U)
 ******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stddef.h>                                /* stdint lib header file */
#include "shell.h"                                  /* shell lib header file */
#include "bsp_handle_led.h"               /* bsp_handle_led lib header file. */
#include "bsp_driver_at24.h"               /* bsp_driver_at24 lib header file. */
// #include "i2c.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
extern at24_driver_t g_at24c02_drv; 
int write_i2c1(uint8_t w_code)
{

  uint8_t i = w_code;
  at24_state_t ret = AT24_OK;
  ret = \
  g_at24c02_drv.pf_write_byte(&g_at24c02_drv, 0x00, i, 5);
  return ret;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 write_i2c1, \
                 write_i2c1, \
                 write_i2c1);



int read_i2c1(void)
{
  uint8_t i = 0x00;
  at24_state_t ret = AT24_OK;
  ret = g_at24c02_drv.pf_read_bytes(&g_at24c02_drv, \
                            0x00, \
                            &i, \
                            1, \
                            5);
  if(AT24_OK != ret)
  {
      return ret;
  }
  return i;
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





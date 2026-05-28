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
#include "i2c.h"
/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
int write_i2c1(void)
{

  uint8_t i = 0x55;
  uint32_t ret = HAL_I2C_Mem_Write(&hi2c1, \
                             0xA0, \
                             0x00,\
                             I2C_MEMADD_SIZE_8BIT,\
                             &i, 1,\
                             1000);
  return ret;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 write_i2c1, \
                 write_i2c1, \
                 write_i2c1);



int read_i2c1(void)
{
  uint8_t i = 0xff; 
  if(HAL_I2C_Mem_Read(&hi2c1, \
                             0xA1, \
                             0x00,\
                             I2C_MEMADD_SIZE_8BIT,\
                             &i, 1,\
                             1000) != HAL_OK)
  {
    return -1;
  }
  return i;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 read_i2c1, \
                 read_i2c1, \
                 read_i2c1);





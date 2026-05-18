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
#include "cmds.h"                                    /* cmds lib header file */
#include "main.h"                                    /* main lib header file */
#include "shell.h"                                  /* shell lib header file */
#include "shell_cfg.h"
#include "shell_port.h"

/* define   -----------------------------------------------------------------*/

/* typedef ------------------------------------------------------------------*/

/* variables ----------------------------------------------------------------*/

/* Private  functions  ------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
int led_tog(void)
{
    HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
    return 1;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
                 tog_led2, \
                 led_tog, \
                 led_tog);

// int set_point(uint16_t  x , uint16_t y)
// {
//     stxxx_dev_status_t ret = STXXX_DEV_OK;
//     stxxx_pos_t pos;
//     pos.x = x;
//     pos.y = y;
//     uint16_t color = color24bit_to_rgb565(0xFF0000U);
//     ret = stxxx_draw_point(&pos,color);
//     return ret;
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  point, \
//                  set_point, \
//                  set_point);


// int show_pitcture(uint16_t  x , uint16_t y,uint16_t  w , uint16_t h )
// {
//     stxxx_dev_status_t ret = STXXX_DEV_OK;
//     stxxx_pos_t pos;
//     pos.x = x;
//     pos.y = y;
//     pos.h = h;
//     pos.w = w;
//     ret = stxxx_images(&pos,(uint16_t*)gImage_123);
//     return ret;
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  pct, \
//                  show_pitcture, \
//                  show_pitcture);

// int clear_all(void)
// {
//     stxxx_dev_status_t ret = STXXX_DEV_OK;
//     ret = stxxx_clear_all();
//     return ret;
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  clr, \
//                  clear_all, \
//                  clear_all);

// int rst(void)
// {
//     NVIC_SystemReset();
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  rst, \
//                  rst, \
//                  restart the board);


// int on_off_led(void)
// {
//     HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
//     return 1;
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  tog2, \
//                  on_off_led, \
//                  ledcontrol);


// char soft_version[] = "v100";
// SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), \
//                  version, \
//                  soft_version, \
//                  string);

// typedef struct {
//     int id;
//     char name[20];
//     int value;
// } MyStruct;

// MyStruct myStruct = {1, "test", 1};

// int cmd_showStruct(int argc, char *argv[]) 
// {
//     (void)argc;
//     (void)argv;
//     Shell *shell = shellGetCurrent();
//     shellPrint(shell, "id = %d\r\n", myStruct.id);
//     shellPrint(shell, "name = %s\r\n", myStruct.name);
//     shellPrint(shell, "value = %d\r\n", myStruct.value);
//     return 0;
// }

// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), \
//                  showStruct, \
//                  cmd_showStruct, \
//                  display myStruct);

/* end of file --------------------------------------------------------------*/




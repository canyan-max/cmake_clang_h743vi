#!/usr/bin/env python3
"""Generate a portable MCU layered source skeleton."""
import argparse
from pathlib import Path

HEADER = """/**\n ******************************************************************************\n *@file               :   {name}\n *@brief              :   {brief}\n *@version            :   V1.0\n *@note               :   1 tab == 4 spaces!\n ******************************************************************************\n */\n"""

def write(root, rel, text):
    path = root / rel
    if path.exists():
        raise FileExistsError(f"Refusing to overwrite: {path}")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8", newline="\n")

def h(name, guard, brief, body):
    return HEADER.format(name=name, brief=brief) + f"\n/* Define to prevent recursive inclusion ------------------------------------*/\n#ifndef {guard}\n#define {guard}\n\n#ifdef __cplusplus\nextern \"C\"\n{{\n#endif\n{body}\n#ifdef __cplusplus\n}}\n#endif\n\n#endif /* {guard} */\n\n/* end of file --------------------------------------------------------------*/\n"

def c(name, brief, includes, body):
    return HEADER.format(name=name, brief=brief) + f"\n/* Includes -----------------------------------------------------------------*/\n{includes}\n\n{body}\n/* end of file --------------------------------------------------------------*/\n"

def common_files(root):
    write(root, "00_Board/Inc/board_resources.h", h("board_resources.h", "BOARD_RESOURCES_H", "Provide vendor-independent board resources.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n\n/* typedef ------------------------------------------------------------------*/\ntypedef uint8_t board_gpio_resource_id_t;\n\n/* define -------------------------------------------------------------------*/\n#define BOARD_GPIO_STATUS_LED      ((board_gpio_resource_id_t)0U)\n#define BOARD_GPIO_RESOURCE_COUNT  ((board_gpio_resource_id_t)1U)\n#define BOARD_STATUS_LED_ON_LEVEL  (1U)\n"))
    write(root, "00_Board/Inc/board_target_binding.h", h("board_target_binding.h", "BOARD_TARGET_BINDING_H", "Bind board resources to the selected MCU SDK.", "\n/* Includes -----------------------------------------------------------------*/\n/* Include target-generated peripheral headers here. */\n\n/* define -------------------------------------------------------------------*/\n/* Define Board resource to vendor pin/handle mappings here. */\n"))
    write(root, "04_Platform/01_common/Inc/plat_error.h", h("plat_error.h", "PLAT_ERROR_H", "Provide common Platform error definitions.", "\n/* typedef ------------------------------------------------------------------*/\ntypedef enum PLATFORM_ERR_T\n{\n    PLATFORM_ERR_OK = 0,\n    PLATFORM_ERR_PARAM,\n    PLATFORM_ERR_BUSY,\n    PLATFORM_ERR_HW,\n    PLATFORM_ERR_TIMEOUT,\n} platform_err_t;\n"))
    write(root, "04_Platform/03_mcu_interface/Inc/plat_resource.h", h("plat_resource.h", "PLAT_RESOURCE_H", "Provide common storage for opaque Platform resource IDs.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n\n/* typedef ------------------------------------------------------------------*/\ntypedef uint8_t plat_resource_id_t;\n"))
    write(root, "04_Platform/03_mcu_interface/Inc/plat_gpio.h", h("plat_gpio.h", "PLAT_GPIO_H", "Provide the portable GPIO contract.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"plat_error.h\"\n#include \"plat_resource.h\"\n\n/* typedef ------------------------------------------------------------------*/\ntypedef plat_resource_id_t plat_gpio_id_t;\n\n/* function  ----------------------------------------------------------------*/\nplatform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level);\n"))
    write(root, "05_Impl/01_mcu/Src/mcu_gpio.c", c("mcu_gpio.c", "Provide the selected MCU GPIO implementation placeholder.", '#include "plat_gpio.h"\n#include "board_resources.h"\n#include "board_target_binding.h"', "/* Exported functions -------------------------------------------------------*/\n\nplatform_err_t plat_gpio_write(plat_gpio_id_t id, uint8_t level)\n{\n    if((uint32_t)id >= (uint32_t)BOARD_GPIO_RESOURCE_COUNT)\n    {\n        return PLATFORM_ERR_PARAM;\n    }\n\n    (void)level;\n    /* Map the validated Board ID through board_target_binding.h here. */\n    return PLATFORM_ERR_HW;\n}\n\n"))

def app_files(root, layered):
    include = '#include "app_main.h"' + ('\n#include "service_app.h"' if layered else '')
    call = 'service_app_process();' if layered else '/* Add Service orchestration when required. */'
    write(root, "01_App/Inc/app_main.h", h("app_main.h", "APP_MAIN_H", "Provide application entry APIs.", "\n/* function  ----------------------------------------------------------------*/\nvoid app_main_init(void);\nvoid app_main_process(void);\n"))
    write(root, "01_App/Src/app_main.c", c("app_main.c", "Provide application entry processing.", include, f"/* Exported functions -------------------------------------------------------*/\n\nvoid app_main_init(void)\n{{\n}}\n\nvoid app_main_process(void)\n{{\n    {call}\n}}\n\n"))

def layered_files(root):
    write(root, "02_Service/Inc/service_app.h", h("service_app.h", "SERVICE_APP_H", "Provide application service APIs.", "\n/* function  ----------------------------------------------------------------*/\nvoid service_app_process(void);\n"))
    write(root, "02_Service/Src/service_app.c", c("service_app.c", "Provide application service processing.", '#include "service_app.h"\n#include "bsp_status_led.h"', "/* Exported functions -------------------------------------------------------*/\n\nvoid service_app_process(void)\n{\n    (void)bsp_status_led_set(1U);\n}\n\n"))

def bsp_files(root):
    write(root, "04_Platform/02_bsp/Inc/bsp_status_led.h", h("bsp_status_led.h", "BSP_STATUS_LED_H", "Provide the board status LED capability.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"plat_error.h\"\n\n/* function  ----------------------------------------------------------------*/\nplatform_err_t bsp_status_led_set(uint8_t enabled);\n"))
    write(root, "04_Platform/02_bsp/Src/bsp_status_led.c", c("bsp_status_led.c", "Provide the board status LED capability.", '#include "bsp_status_led.h"\n#include "board_resources.h"\n#include "plat_gpio.h"', "/* Exported functions -------------------------------------------------------*/\n\nplatform_err_t bsp_status_led_set(uint8_t enabled)\n{\n    uint8_t level = (0U != enabled) ? BOARD_STATUS_LED_ON_LEVEL\n                                    : (uint8_t)!BOARD_STATUS_LED_ON_LEVEL;\n\n    return plat_gpio_write(BOARD_GPIO_STATUS_LED, level);\n}\n\n"))

def manifest(root, kind, profile):
    groups = "00_Board\n01_App\n04_Platform/01_common\n04_Platform/02_bsp\n04_Platform/03_mcu_interface\n05_Impl/01_mcu"
    if profile == "layered": groups = "00_Board\n01_App\n02_Service\n04_Platform/01_common\n04_Platform/02_bsp\n04_Platform/03_mcu_interface\n05_Impl/01_mcu"
    write(root, "MDK5_GROUPS.md" if kind == "keil" else "ARCHITECTURE.md", f"# {kind.upper()} {profile} MCU skeleton\n\nProject-owned groups/layers:\n\n```text\n{groups}\n```\n\nVendor SDK, startup, linker/scatter, generated initialization, device pack and RTOS remain external.\n")

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--output", required=True)
    p.add_argument("--profile", choices=("minimal", "layered"), default="minimal")
    p.add_argument("--kind", choices=("cmake", "keil"), required=True)
    a = p.parse_args(); root = Path(a.output).resolve()
    if root.exists() and any(root.iterdir()): raise SystemExit(f"Output must be empty: {root}")
    root.mkdir(parents=True, exist_ok=True)
    write(root, "AGENTS.md", """# MCU Layered Project Rules\n\nUse `01_App -> 02_Service -> public bsp_* -> ChipDrivers/plat_* -> 05_Impl/01_mcu -> vendor SDK`. App does not call BSP directly. Service does not include Board, `plat_*`, HAL, or generated peripheral headers.\n\n`00_Board/Inc/board_resources.h` contains vendor-independent resource IDs and fixed hardware properties. A target-specific Board binding contains vendor pins and handles and is private to MCU Implementation. Generic `plat_*` contracts use category-specific opaque IDs over `plat_resource_id_t`; Board owns concrete resource names and counts.\n\nDo not add generic Device, Device Interface, or BSP Binding layers unless a current requirement owns aggregation, complex lifecycle, concurrency arbitration, or genuinely replaceable device semantics. Use standard `<stdint.h>` types directly, finite timeouts, public parameter checks, `platform_err_t`, and bounded resource mapping tables.\n"""); common_files(root); app_files(root, a.profile == "layered"); bsp_files(root)
    if a.profile == "layered": layered_files(root)
    manifest(root, a.kind, a.profile)
    print(f"Generated {a.kind} {a.profile} skeleton: {root}")
if __name__ == "__main__": main()

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
    write(root, "00_Board/Inc/board_config.h", h("board_config.h", "BOARD_CONFIG_H", "Provide board resource configuration.", "\n/* define -------------------------------------------------------------------*/\n/* Define board resources after selecting the target SDK. */\n"))
    write(root, "04_Platform/01_common/Inc/platform_error.h", h("platform_error.h", "PLATFORM_ERROR_H", "Provide common platform error definitions.", "\n/* typedef ------------------------------------------------------------------*/\ntypedef enum PLATFORM_ERR_T\n{\n    PLATFORM_ERR_OK = 0,\n    PLATFORM_ERR_PARAM,\n    PLATFORM_ERR_HW,\n    PLATFORM_ERR_TIMEOUT,\n} platform_err_t;\n"))
    write(root, "04_Platform/03_port_interface/Inc/port_gpio.h", h("port_gpio.h", "PORT_GPIO_H", "Provide portable GPIO port APIs.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"platform_error.h\"\n\n/* typedef ------------------------------------------------------------------*/\ntypedef enum PORT_GPIO_ID_T\n{\n    PORT_GPIO_ID_0 = 0,\n    PORT_GPIO_ID_NUM,\n} port_gpio_id_t;\n\n/* function  ----------------------------------------------------------------*/\nplatform_err_t port_gpio_write(port_gpio_id_t id, uint8_t level);\n"))
    write(root, "05_Impl/target_placeholder/Src/port_gpio.c", c("port_gpio.c", "Provide target GPIO port implementation placeholder.", '#include "port_gpio.h"', "/* Exported functions -------------------------------------------------------*/\n\nplatform_err_t port_gpio_write(port_gpio_id_t id, uint8_t level)\n{\n    if(id >= PORT_GPIO_ID_NUM)\n    {\n        return PLATFORM_ERR_PARAM;\n    }\n    (void)level;\n    /* Map id to the selected vendor SDK GPIO port and pin here. */\n    return PLATFORM_ERR_OK;\n}\n\n"))

def app_files(root, layered):
    include = '#include "app_main.h"' + ('\n#include "service_app.h"' if layered else '\n#include "bsp_gpio.h"')
    call = 'service_app_process();' if layered else 'bsp_gpio_process();'
    write(root, "01_App/Inc/app_main.h", h("app_main.h", "APP_MAIN_H", "Provide application entry APIs.", "\n/* function  ----------------------------------------------------------------*/\nvoid app_main_init(void);\nvoid app_main_process(void);\n"))
    write(root, "01_App/Src/app_main.c", c("app_main.c", "Provide application entry processing.", include, f"/* Exported functions -------------------------------------------------------*/\n\nvoid app_main_init(void)\n{{\n}}\n\nvoid app_main_process(void)\n{{\n    {call}\n}}\n\n"))

def layered_files(root):
    write(root, "02_Service/Inc/service_app.h", h("SERVICE_APP_H", "SERVICE_APP_H", "Provide application service APIs.", "\n/* function  ----------------------------------------------------------------*/\nvoid service_app_process(void);\n"))
    write(root, "02_Service/Src/service_app.c", c("service_app.c", "Provide application service processing.", '#include "service_app.h"\n#include "device_example.h"', "/* Exported functions -------------------------------------------------------*/\n\nvoid service_app_process(void)\n{\n    (void)device_example_write((const uint8_t *)0, 0U);\n}\n\n"))
    write(root, "03_Device/Inc/device_example.h", h("device_example.h", "DEVICE_EXAMPLE_H", "Provide example device APIs.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"platform_error.h\"\n\n/* function  ----------------------------------------------------------------*/\nplatform_err_t device_example_write(const uint8_t *p_data, uint16_t size);\n"))
    write(root, "03_Device/Src/device_example.c", c("device_example.c", "Provide example device APIs.", '#include <stddef.h>\n#include "device_example.h"\n#include "example_intf.h"', "/* Exported functions -------------------------------------------------------*/\n\nplatform_err_t device_example_write(const uint8_t *p_data, uint16_t size)\n{\n    if((NULL == p_data) || (0U == size))\n    {\n        return PLATFORM_ERR_PARAM;\n    }\n    return g_example_ops.pf_write(p_data, size);\n}\n\n"))
    write(root, "03_Device_interface/Inc/example_intf.h", h("example_intf.h", "EXAMPLE_INTF_H", "Provide example replaceable-device contract.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"platform_error.h\"\n\n/* typedef ------------------------------------------------------------------*/\ntypedef struct EXAMPLE_OPS_T\n{\n    platform_err_t (*pf_write)(const uint8_t *p_data, uint16_t size);\n} example_ops_t;\n\n/* variables ----------------------------------------------------------------*/\nextern const example_ops_t g_example_ops;\n"))
    write(root, "04_Platform/02_bsp_binding/Src/example_binding.c", c("example_binding.c", "Bind example chip driver to device contract.", '#include "bsp_example.h"\n#include "example_intf.h"', "/* Private  functions  ------------------------------------------------------*/\n\nstatic platform_err_t example_write(const uint8_t *p_data, uint16_t size)\n{\n    return bsp_example_write(p_data, size);\n}\n\n/* Exported variables -------------------------------------------------------*/\n\nconst example_ops_t g_example_ops = { .pf_write = example_write };\n\n"))

def bsp_files(root):
    write(root, "04_Platform/02_bsp/Inc/bsp_gpio.h", h("bsp_gpio.h", "BSP_GPIO_H", "Provide simple board GPIO APIs.", "\n/* function  ----------------------------------------------------------------*/\nvoid bsp_gpio_process(void);\n"))
    write(root, "04_Platform/02_bsp/Inc/bsp_example.h", h("bsp_example.h", "BSP_EXAMPLE_H", "Provide example chip driver API.", "\n/* Includes -----------------------------------------------------------------*/\n#include <stdint.h>\n#include \"platform_error.h\"\n\n/* function  ----------------------------------------------------------------*/\nplatform_err_t bsp_example_write(const uint8_t *p_data, uint16_t size);\n"))
    write(root, "04_Platform/02_bsp/Src/bsp_gpio.c", c("bsp_gpio.c", "Provide simple board GPIO processing.", '#include "bsp_gpio.h"', "/* Exported functions -------------------------------------------------------*/\n\nvoid bsp_gpio_process(void)\n{\n}\n\n"))
    write(root, "04_Platform/02_bsp/Src/bsp_example.c", c("bsp_example.c", "Provide example chip driver API.", '#include "bsp_example.h"', "/* Exported functions -------------------------------------------------------*/\n\nplatform_err_t bsp_example_write(const uint8_t *p_data, uint16_t size)\n{\n    if((0 == p_data) || (0U == size))\n    {\n        return PLATFORM_ERR_PARAM;\n    }\n    return PLATFORM_ERR_OK;\n}\n\n"))

def manifest(root, kind, profile):
    groups = "Board\nApp\nPlatform Common\nBSP\nPort Interface\nImplementation"
    if profile == "layered": groups = "Board\nApp\nService\nDevice\nDevice Interface\nPlatform Common\nBSP\nBSP Binding\nPort Interface\nImplementation"
    write(root, "MDK5_GROUPS.md" if kind == "keil" else "ARCHITECTURE.md", f"# {kind.upper()} {profile} MCU skeleton\n\nProject-owned groups/layers:\n\n```text\n{groups}\n```\n\nVendor SDK, startup, linker/scatter, generated initialization, device pack and RTOS remain external.\n")

def main():
    p = argparse.ArgumentParser()
    p.add_argument("--output", required=True)
    p.add_argument("--profile", choices=("minimal", "layered"), default="minimal")
    p.add_argument("--kind", choices=("cmake", "keil"), required=True)
    a = p.parse_args(); root = Path(a.output).resolve()
    if root.exists() and any(root.iterdir()): raise SystemExit(f"Output must be empty: {root}")
    root.mkdir(parents=True, exist_ok=True)
    write(root, "AGENTS.md", """# MCU Layered Project Rules\n\n`01_App → 02_Service → 03_Device → 04_Platform → 05_Impl → external SDK`.\n\n`00_Board` contains board-resource mapping only. Keep vendor SDK types and calls in `05_Impl/<target>`. Use `03_Device_interface` plus `02_bsp_binding` only for replaceable, stateful, or protocol-heavy devices. Use fixed-width types, `platform_err_t`, and resource ID enums for fixed board resources.\n"""); common_files(root); app_files(root, a.profile == "layered"); bsp_files(root)
    if a.profile == "layered": layered_files(root)
    manifest(root, a.kind, a.profile)
    print(f"Generated {a.kind} {a.profile} skeleton: {root}")
if __name__ == "__main__": main()

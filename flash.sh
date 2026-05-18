#!/bin/bash
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
HEX_FILE="$PROJECT_DIR/build/Debug/cmake-clang-h743vi.hex"
JLINK_SCRIPT="$PROJECT_DIR/JLinkload/flash.jlink"

if [ ! -f "$HEX_FILE" ]; then
    echo "ERR:NOT FOUND $HEX_FILE "
    exit 1
fi

if command -v JLinkExe &> /dev/null; then
    JLINK_CMD="JLinkExe"
elif command -v JLink.exe &> /dev/null; then
    JLINK_CMD="JLink.exe"
elif command -v jlink.exe &> /dev/null; then
    JLINK_CMD="jlink.exe"
else
    echo "ERR:NOT FOUND JLinkExe or JLink.exe or jlink.exe in PATH"
    exit 1
fi

echo "LOADING $HEX_FILE ..."
cd "$PROJECT_DIR"
$JLINK_CMD -Device STM32H743VI -If SWD -Speed 4000 -CommanderScript "$JLINK_SCRIPT"
echo "load finished $HEX_FILE"
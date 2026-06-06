#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_DIR"

BUILD_DIR="build/Debug"
HEX_FILE="$BUILD_DIR/cmake-clang-h743vi.hex"
JLINK_SCRIPT="$PROJECT_DIR/JLinkload/flash.jlink"

echo "========================================="
echo "Step 1: Clean (optional)"
echo "========================================="
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

echo "========================================="
echo "Step 2: CMake Configuration"
echo "========================================="
# 总是运行 cmake --preset（确保配置最新）
# 若不想每次重新配置，可以保留原来的条件判断，但为了观察信息，这里改为总是执行
cmake --preset Debug

echo "========================================="
echo "Step 3: Build with Ninja"
echo "========================================="
cmake --build "$BUILD_DIR" -j 16
echo "Build completed successfully."

echo "========================================="
echo "Step 4: Flash via JLink"
echo "========================================="
if [ ! -f "$HEX_FILE" ]; then
    echo "ERR: NOT FOUND $HEX_FILE"
    exit 1
fi

# 查找 JLink 命令
if command -v JLinkExe &> /dev/null; then
    JLINK_CMD="JLinkExe"
elif command -v JLink.exe &> /dev/null; then
    JLINK_CMD="JLink.exe"
elif command -v jlink.exe &> /dev/null; then
    JLINK_CMD="jlink.exe"
else
    echo "ERR: JLink tool not found in PATH"
    exit 1
fi

echo "Loading $HEX_FILE ..."
$JLINK_CMD -Device STM32H743VI -If SWD -Speed 4000 -CommanderScript "$JLINK_SCRIPT"
echo "Flash completed."
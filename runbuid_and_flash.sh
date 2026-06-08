#!/bin/bash
set -e

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info()    { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error()   { echo -e "${RED}[ERROR]${NC} $1"; }

# Default configuration
DEFAULT_DEVICE="STM32H743VI"
DEFAULT_SPEED="4000"
DEFAULT_PRESET="Debug"
DEFAULT_HEX_FILE="build/Debug/cmake-clang-h743vi.hex"

# Global variables
CLEAN=false
RECONFIGURE=false
BUILD_ONLY=false
FLASH_ONLY=false
VERIFY=false
RUN_AFTER_FLASH=true
FORCE_ERASE=false
DEVICE="$DEFAULT_DEVICE"
SPEED="$DEFAULT_SPEED"
PRESET="$DEFAULT_PRESET"
HEX_FILE="$DEFAULT_HEX_FILE"
BUILD_DIR="build/$PRESET"
CMAKE_EXTRA_ARGS=()
JLINK_EXTRA_ARGS=()
HEX_FILE_SPECIFIED=false

# Show help
show_help() {
    cat << EOF
Usage: $0 [options]

Options:
  -c, --clean           Clean build directory and rebuild (no flash)
  -r, --reconfigure     Force CMake reconfigure (ignore cache)
  -b, --build-only      Build only, do not flash
  -f, --flash-only      Flash only (requires existing hex file)
  -F, --force-erase     Force erase entire chip before flashing
  -v, --verify          Verify after flash
  --no-reset            Do not reset and run after flash (keep halted)
  -d, --device DEVICE   Target device (default: $DEFAULT_DEVICE)
  -s, --speed SPEED     SWD clock speed in kHz (default: $DEFAULT_SPEED)
  -p, --preset NAME     CMake preset name (default: $DEFAULT_PRESET)
  --hex-file FILE       Hex file path (relative to project root, default: $DEFAULT_HEX_FILE)
  --cmake-args ARGS     Extra arguments to pass to CMake (can be used multiple times)
  --jlink-args ARGS     Extra arguments to pass to JLinkExe (can be used multiple times)
  -h, --help            Show this help

Examples:
  $0                          # Full build and flash
  $0 -c                       # Clean, rebuild, no flash
  $0 -b                       # Build only
  $0 -f                       # Flash only (using existing hex)
  $0 -F                       # Force erase before flashing
  $0 --no-reset               # Flash but keep target halted
EOF
    exit 0
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean) CLEAN=true; shift ;;
        -r|--reconfigure) RECONFIGURE=true; shift ;;
        -b|--build-only) BUILD_ONLY=true; shift ;;
        -f|--flash-only) FLASH_ONLY=true; shift ;;
        -F|--force-erase) FORCE_ERASE=true; shift ;;
        -v|--verify) VERIFY=true; shift ;;
        --no-reset) RUN_AFTER_FLASH=false; shift ;;
        -d|--device) DEVICE="$2"; shift 2 ;;
        -s|--speed) SPEED="$2"; shift 2 ;;
        -p|--preset) 
            PRESET="$2"
            BUILD_DIR="build/$PRESET"
            if ! $HEX_FILE_SPECIFIED; then
                HEX_FILE="build/$PRESET/$(basename "$DEFAULT_HEX_FILE")"
            fi
            shift 2 ;;
        --hex-file) 
            HEX_FILE="$2"
            HEX_FILE_SPECIFIED=true
            shift 2 ;;
        --cmake-args) CMAKE_EXTRA_ARGS+=("$2"); shift 2 ;;
        --jlink-args) JLINK_EXTRA_ARGS+=("$2"); shift 2 ;;
        -h|--help) show_help ;;
        *) log_error "Unknown option: $1"; show_help ;;
    esac
done

# Get project root directory
PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_DIR"

# Ensure HEX_FILE is relative to PROJECT_ROOT (no conversion to absolute)
# File existence check will be done later using relative path

# Conflict checks
if $FLASH_ONLY && $BUILD_ONLY; then
    log_error "--flash-only and --build-only cannot be used together"
    exit 1
fi

if $FLASH_ONLY && $CLEAN; then
    log_error "--flash-only and --clean cannot be used together"
    exit 1
fi

# Clean mode
if $CLEAN; then
    BUILD_ONLY=true
    log_info "Clean mode: removing $BUILD_DIR and rebuilding (no flash)"
    rm -rf "$BUILD_DIR"
fi

# Cross-platform CPU core count
get_cores() {
    if command -v nproc &>/dev/null; then
        nproc
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        sysctl -n hw.ncpu
    else
        echo 4
    fi
}

# CMake configuration
configure_cmake() {
    log_info "CMake configuration (preset: $PRESET)"
    local reconf_flag=""
    if $RECONFIGURE; then
        if cmake --help-option --fresh &>/dev/null; then
            reconf_flag="--fresh"
        else
            log_warning "CMake --fresh not supported, removing cache manually"
            rm -f "$BUILD_DIR/CMakeCache.txt"
            rm -rf "$BUILD_DIR/CMakeFiles"
        fi
    fi

    if [[ ! -f "$BUILD_DIR/build.ninja" ]] || $RECONFIGURE; then
        log_info "Running: cmake --preset $PRESET ${CMAKE_EXTRA_ARGS[*]}"
        cmake --preset "$PRESET" ${reconf_flag} "${CMAKE_EXTRA_ARGS[@]}"
    else
        log_info "CMake cache valid, skipping configuration (use -r to force)"
    fi
}

# Build
build_firmware() {
    local cores=$(get_cores)
    log_info "Starting build (parallel: $cores threads)"
    if ! cmake --build "$BUILD_DIR" -j "$cores"; then
        log_error "Build failed"
        exit 1
    fi
    log_success "Build completed: $HEX_FILE"
}

# Flash (dynamically generated script)
flash_firmware() {
    log_info "Preparing to flash device: $DEVICE"

    if [[ ! -f "$HEX_FILE" ]]; then
        log_error "Hex file not found: $HEX_FILE (current dir: $PWD)"
        exit 1
    fi

    local jlink_cmd=""
    if command -v JLinkExe &>/dev/null; then
        jlink_cmd="JLinkExe"
    elif command -v JLink.exe &>/dev/null; then
        jlink_cmd="JLink.exe"
    elif command -v jlink.exe &>/dev/null; then
        jlink_cmd="jlink.exe"
    else
        log_error "JLink tool not found in PATH"
        exit 1
    fi

    # Generate temporary JLink command script
    local tmp_script=$(mktemp)
    cat > "$tmp_script" << EOF
si SWD
speed $SPEED
device $DEVICE
connect
r
h
EOF

    if $FORCE_ERASE; then
        cat >> "$tmp_script" << EOF
erase
sleep 100
EOF
    fi

    cat >> "$tmp_script" << EOF
loadfile "$HEX_FILE"
EOF

    if $VERIFY; then
        cat >> "$tmp_script" << EOF
verifybin "$HEX_FILE", 0x08000000
EOF
    fi

    if $RUN_AFTER_FLASH; then
        cat >> "$tmp_script" << EOF
r
g
EOF
    else
        cat >> "$tmp_script" << EOF
r
h
echo "Flash done, target halted (--no-reset)"
EOF
    fi

    echo "exit" >> "$tmp_script"

    local jlink_args=(
        "-Device" "$DEVICE"
        "-If" "SWD"
        "-Speed" "$SPEED"
        "-AutoConnect" "1"
        "-ExitOnError" "1"
        "-CommanderScript" "$tmp_script"
    )
    jlink_args+=("${JLINK_EXTRA_ARGS[@]}")

    log_info "Flash command: $jlink_cmd ${jlink_args[*]}"
    if ! "$jlink_cmd" "${jlink_args[@]}"; then
        log_error "Flashing failed"
        rm -f "$tmp_script"
        exit 1
    fi

    rm -f "$tmp_script"
    log_success "Flashing successful"
}

# Main
main() {
    if $FLASH_ONLY; then
        flash_firmware
        exit 0
    fi

    configure_cmake
    build_firmware

    if ! $BUILD_ONLY; then
        flash_firmware
    else
        log_info "Build-only mode, skipping flash"
    fi
}

main
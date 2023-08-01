#!/bin/bash

set -e
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)
cd ${CURRENT_PATH}

usage() {
    echo ""
    echo "Usage: bash <workspace>/build.sh [options]"
    echo "-dos | --download_open_source, { OFF, ON }"
    echo "-bos | --build_open_source, { OFF, ON }"
    echo "-b | --build_config_mode, { Debug, Release }"
    echo "-h | --help"
    echo ""
}

parse_args() {
    for arg in "$@"
    do
        case $arg in
            -h | --help)
            usage
            exit
            ;;
            -b=* | --build_config_mode=*)
            build_config_mode="${arg#*=}"
            ;;
            -dos=* | --download_open_source=*)
            download_open_source="${arg#*=}"
            ;;
            -bos=* | --build_open_source=*)
            build_open_source="${arg#*=}"
            ;;
            *)
            echo "Error: unknown parameter $arg"
            usage
            exit 1
            ;;
        esac
    done
}

build_config_mode=Debug
download_open_source=OFF
build_open_source=OFF
parse_args $@

if [ "${download_open_source}" = "ON" ]; then
    _download_open_source=ON
else
    _download_open_source=OFF
fi

if [ "${build_open_source}" = "ON" ]; then
    _build_open_source=ON
else
    _build_open_source=OFF
fi

if [ "${build_config_mode}" = "Debug" ]; then
    _build_debug_mode=ON
else
    _build_debug_mode=OFF
fi

cmake -S . -B build                               \
  -DDOWNLOAD_OPEN_SOURCE=${_download_open_source} \
  -DREARCHIVE_OPEN_SOURCE=${_build_open_source}   \
  -DBUILD_DEBUG_OPEN_SOURCE=${_build_debug_mode}
cmake --build build --config ${build_config_mode} --parallel
cmake --install build --prefix output --config ${build_config_mode}

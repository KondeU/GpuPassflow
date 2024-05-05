#!/bin/bash

set -ex
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)
cd ${CURRENT_PATH}

usage() {
    echo ""
    echo "Usage: bash <workspace>/build.sh [options]"
    echo "-dos | --download_open_source, { OFF, ON }"
    echo "-bos | --build_open_source, { OFF, ON }"
    echo "-b | --build_config_mode, { Debug, Release }"
    echo "--build_sample, { ON, OFF }"
    echo "--custom_frame_resources_key, { string (FRsKey hash key type name) }"
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
            --build_sample=*)
            build_sample="${arg#*=}"
            ;;
            --custom_frame_resources_key=*)
            custom_frame_resources_key="${arg#*=}"
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
build_sample=ON
custom_frame_resources_key="std::string"
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

if [ "${build_sample}" = "OFF" ]; then
    _build_sample=OFF
else
    _build_sample=ON
fi

if [ "${custom_frame_resources_key}" = "std::string" ]; then
    _custom_frame_resources_key=
else
    _custom_frame_resources_key="-DGP_OPT_FRAME_RESOURCES_KEY_TYPE=${custom_frame_resources_key}"
    _build_sample=OFF
fi

cmake -S . -B build                               \
  -DDOWNLOAD_OPEN_SOURCE=${_download_open_source} \
  -DREARCHIVE_OPEN_SOURCE=${_build_open_source}   \
  -DBUILD_DEBUG_OPEN_SOURCE=${_build_debug_mode}  \
  ${_custom_frame_resources_key}
cmake --build build --config ${build_config_mode} --parallel
cmake --install build --prefix output --config ${build_config_mode}

if [ "${_build_sample}" = "ON" ]; then
    cmake -S ./sample -B build/sample/build ${_custom_frame_resources_key}
    cmake --build build/sample/build --config ${build_config_mode} --parallel
    cmake --install build/sample/build --prefix build/sample/output --config ${build_config_mode}
fi

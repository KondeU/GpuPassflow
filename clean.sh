#!/bin/bash

set -x
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

usage() {
    echo ""
    echo "Usage: bash <workspace>/clean.sh [options]"
    echo "no options, clean all"
    echo "--project, clean build folder, excludes third-party libraries"
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
            --project)
            clean_third_party=OFF
            ;;
            --cmake_cache)
            clean_third_party=OFF
            clean_project=OFF
            ;;
            *)
            echo "Error: unknown parameter $arg"
            usage
            exit 1
            ;;
        esac
    done
}

clean_third_party=ON
clean_project=ON
clean_cmake_cache=ON
parse_args $@

if [ "${clean_cmake_cache}" = "ON" ]; then
    rm -rf ${CURRENT_PATH}/build/CMakeCache.txt
fi

if [ "${clean_project}" = "ON" ]; then
    rm -rf ${CURRENT_PATH}/build
    rm -rf ${CURRENT_PATH}/output
fi

if [ "${clean_third_party}" = "ON" ]; then
    bash ${CURRENT_PATH}/3rd/clean_deps.sh
fi

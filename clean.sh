#!/bin/bash

set -x
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)

rm -rf ${CURRENT_PATH}/build
rm -rf ${CURRENT_PATH}/output

#bash ${CURRENT_PATH}/3rd/clean_deps.sh

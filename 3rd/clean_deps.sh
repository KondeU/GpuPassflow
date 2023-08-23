#!/bin/bash

set -e
CURRENT_PATH=$(cd "$(dirname "$0")"; pwd)
cd ${CURRENT_PATH}

cd deps
for i in `ls`; do if [ "$i" != .keep ]; then rm -rf $i; fi; done;
cd ..

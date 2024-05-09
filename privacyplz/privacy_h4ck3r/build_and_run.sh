#!/usr/bin/env bash

readonly BUILD_DIR="/home/priv/privacy_user_build"
SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

/usr/bin/cmake -B $BUILD_DIR -S $SRC_DIR -D CMAKE_PREFIX_PATH=/opt
/usr/bin/cmake --build $BUILD_DIR

$BUILD_DIR/privacy_user

rm -r $BUILD_DIR
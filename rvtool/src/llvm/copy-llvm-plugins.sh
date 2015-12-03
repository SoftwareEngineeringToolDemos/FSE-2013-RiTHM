#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <LLVM directory>"
    exit 1
fi

LLVM_DIR=`realpath $1`

if [ ! -d "$LLVM_DIR" ]; then
    echo "The directory $LLVM_DIR does not exist. Please download (and build) all dependencies first using build-deps.sh before running this script."
    exit 1
fi

cp -R ./include/llvm/* $LLVM_DIR/include/llvm/
cp -R ./lib/Transforms/* $LLVM_DIR/lib/Transforms/
cp -R ./tools/clang/tools/* $LLVM_DIR/tools/clang/tools/


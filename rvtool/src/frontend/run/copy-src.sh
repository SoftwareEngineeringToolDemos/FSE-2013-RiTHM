#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: $0 <src dir root> <working dir>"
    exit 1
fi

SRC_DIR=`realpath $1`
WORK_DIR=$2/src/

mkdir -p $WORK_DIR
cp -R $SRC_DIR/* $WORK_DIR/

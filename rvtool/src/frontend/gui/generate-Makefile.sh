#!/bin/bash
OUTPUT_DIR=../../../build/frontend/gui
OBJ_DIR=$OUTPUT_DIR/obj
MOC_DIR=$OUTPUT_DIR/moc
UI_H_DIR=$OUTPUT_DIR/ui/h
mkdir -p $OUTPUT_DIR

qmake -o Makefile   "OBJECTS_DIR=$OBJ_DIR"  \
                    "MOC_DIR=$MOC_DIR"      \
                    "CONFIG += release"     \
                    "DESTDIR = $OUTPUT_DIR" \
                    "UI_HEADERS_DIR = $UI_H_DIR" \
                    rvtool.pro 

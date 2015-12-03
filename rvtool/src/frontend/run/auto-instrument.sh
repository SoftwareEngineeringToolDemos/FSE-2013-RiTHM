#!/bin/bash

SHELL_DIR=$PWD
LLVM_DIR=$SHELL_DIR/llvm
LLVM_BIN_DIR=$LLVM_DIR/build/Release+Asserts/bin
LLVM_LIB_DIR=$LLVM_DIR/build/Release+Asserts/lib
LLVM_CC=$LLVM_BIN_DIR/clang
LLVM_OPT=$LLVM_BIN_DIR/opt
LLVM_AUTO_INST=$LLVM_BIN_DIR/auto-instrument


DIR=$1
INST_LINES=$2
MAIN_META=$3
if [ $# > 3 ]; then
	INCLUDE_FILE=$4
	CALLER_FILE=$5
	CALLED_FILE=$6
else
	INCLUDE_FILE="NA"
	CALLER_FILE="NA"
	CALLED_FILE="NA"
fi


# Instrument all source files with the results
SRCFILES=$(ls $DIR/*.c)
cfgfile=$(ls $DIR/*.csf)
for file_name in $SRCFILES
do
	file_base_name=`basename $file_name`
	flag=`awk -v filename=$file_base_name '{if($0~filename"=")print $0;}' $cfgfile`
	if [[ "$flag" != "" ]]; then
		compiler_string=`awk -F= -v filename=$file_base_name '{if($0~filename"=")print $2;}' $cfgfile`
    	$LLVM_AUTO_INST $INST_LINES $MAIN_META $INCLUDE_FILE $CALLER_FILE $CALLED_FILE $file_name -- $compiler_string
    	echo "$LLVM_AUTO_INST $INST_LINES $MAIN_META $INCLUDE_FILE $CALLER_FILE $CALLED_FILE $file_name -- $compiler_string"
	fi
done



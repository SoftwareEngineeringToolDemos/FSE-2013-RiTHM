#!/bin/bash

AMDAPP_LIB_DIR=/opt/AMDAPP/lib/x86
if [  `uname -m` = "x86_64" ];
then
    AMDAPP_LIB_DIR=/opt/AMDAPP/lib/x86_64
fi

export LD_LIBRARY_PATH=`realpath ./libconfig-1.4.8/lib/.libs`:$AMDAPP_LIB_DIR:$LD_LIBRARY_PATH

LLVM_DIR=./llvm
GLOBALIZER=$LLVM_DIR/build/Release+Asserts/bin/globalize

if [ $# -ne 5 ];
then
    echo "Usage: $0 <spec filepath> <source directory> <critical variables list output (csv)> <GooMF instrumentor metadata output (csv)>"
    exit 1
fi

SPEC_FILEPATH=`realpath $1`
SOURCE_DIR=$2
VAR_FILE=$3
GOOMF_INST_FILE=$4
TRACE=$5

mkdir -p $SOURCE_DIR
SOURCE_DIR=`realpath $2`

RITHMIC_GLOBALS_C=`realpath $SOURCE_DIR`/rithmic_globals.c
RITHMIC_GLOBALS_H=`realpath $SOURCE_DIR`/rithmic_globals.h
TEMP_FILE=$SOURCE_DIR/rithmtempfile.txt
SRCFILES=$(ls $SOURCE_DIR/*.c | sort)
cfgfile=$(ls $SOURCE_DIR/*.csf)
echo "Config file is $cfgfile"
echo "Source files found in $SOURCE_DIR:"
for file_name in $SRCFILES
do
	file_base_name=`basename $file_name`
	flag=`awk -v filename=$file_base_name '{if($0~filename"=")print $0;}' $cfgfile`
	if [[ "$flag" != "" ]]; then
		compiler_string=`awk -F= -v filename=$file_base_name '{if($0~filename"=")print $2;}' $cfgfile`
		echo "$GLOBALIZER $SPEC_FILEPATH $VAR_FILE $GOOMF_INST_FILE $RITHMIC_GLOBALS_C $RITHMIC_GLOBALS_H $file_name -- $compiler_string"
    	$GLOBALIZER $SPEC_FILEPATH $VAR_FILE $GOOMF_INST_FILE $RITHMIC_GLOBALS_C $RITHMIC_GLOBALS_H $TEMP_FILE $file_name -- $compiler_string
	else
		echo "### File " $file_base_name " will not be processed because its NOT found in the config file"
	fi
done
count=1
replacelines=`cat $TEMP_FILE`
while read replacetext;
do
	echo $replacetext
	next=count+1
	if [ $count != 1 ] 
	then
		SED_PARAM="'s/__LOGENTRY__/"$replacetext"/g'"
		echo $SED_PARAM | xargs -I{} sed -e {} $SOURCE_DIR/buffer1.h  >$SOURCE_DIR/buffer2.h
		rm $SOURCE_DIR/buffer1.h
	else
		SED_PARAM="'s/__LOGHEADER__/"$replacetext"/g'"
		echo $SED_PARAM | xargs -I{} sed -e {} $SOURCE_DIR/buffer.h >$SOURCE_DIR/buffer1.h
	fi	
	count=count+1
	echo $SED_PARAM

done < "$TEMP_FILE"
SED_PARAM="'s/__TRACE__/"$TRACE"/g'"
echo $SED_PARAM | xargs -I{} sed -e {} $SOURCE_DIR/buffer2.h  >$SOURCE_DIR/buffer3.h

MOD_SOURCE_DIR="${SOURCE_DIR}/.."
OUT_LOG_DIR=`echo $MOD_SOURCE_DIR| awk '{gsub(/\//,"\/",$0);gsub(/\./,"\.",$0);print $0;}'`

SED_PARAM="'s/__OUTPUTDIR__/"$OUT_LOG_DIR"/g'"
echo $SED_PARAM | xargs -I{} sed -e {} $SOURCE_DIR/buffer3.h  >$SOURCE_DIR/buffer.h

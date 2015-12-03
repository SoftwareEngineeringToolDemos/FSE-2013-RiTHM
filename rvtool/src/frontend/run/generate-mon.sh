#!/bin/bash

JAVA_CMD='java -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAR_PREFIX=ca.uwaterloo.esg.rvtool.monitor

export LD_LIBRARY_PATH=`realpath ./libconfig-1.4.8/lib/.libs`:`realpath ./swig`:$LD_LIBRARY_PATH


if [ $# -eq 8 ]; then
	MON_TEMPLATE=$1
	SPEC_FILEPATH=$2
	MON_OUTPUT_PATH=$3
	ISP=$4
	MAIN_METADATA=$5
	ALG_TYPE=$6
	INVOC_TYPE=$7
	BUFFER_SIZE=$8
	$JAVA_CMD $JAR_PREFIX.MonitorCreator $MON_TEMPLATE $SPEC_FILEPATH $MON_OUTPUT_PATH $ISP $MAIN_METADATA $ALG_TYPE $INVOC_TYPE $BUFFER_SIZE
else
	MON_TEMPLATE=$1
	SPEC_FILEPATH=$2
	$JAVA_CMD $JAR_PREFIX.MonitorCreator $MON_TEMPLATE $SPEC_FILEPATH
fi


echo "------Monitor generation is successful-------"
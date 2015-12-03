#!/bin/bash

JAVA_CMD='java -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAR_PREFIX=ca.uwaterloo.esg.rvtool.TTRV
#echo $#
#if [ $# -ne 8 ]; then
#    echo "Usage: $0 <output header path> <spec (cfg) filepath> <filepath of critical variables list> <GooMF algo type> <GooMF invoke type> <buffer size> <output instrumentation info path> <line mapping file>"
#    exit 1
#fi

#######################################
# header snippet from GooMFOnlineAPI.h 
# that defines different GooMF configs
#######################################
#//type for the trigger that triggers the flush of the program trace
#typedef enum{_GOOMF_enum_no_trigger = 0, _GOOMF_enum_buffer_trigger, _GOOMF_enum_time_trigger} _GOOMF_enum_trigger_type;
#
#//type for the GooMF_flush() invocation - synchronous (blocking) or asynchronous (non-blocking)
#typedef enum{_GOOMF_enum_sync_invocation = 0, _GOOMF_enum_async_invocation} _GOOMF_enum_invocation_type;
#
#//enum for algorithm type
#typedef enum{_GOOMF_enum_alg_seq = 0,
#                _GOOMF_enum_alg_partial_offload,
#                _GOOMF_enum_alg_finite,
#                _GOOMF_enum_alg_infinite} _GOOMF_enum_alg_type;
######################################

HEADER_OUTPUT_PATH=$1       #output dir of GooMFInstrumentor.h
SPEC_FILEPATH=$2            #spec file
VAR_FILEPATH=$3             #filepath of critical variables list
CONTEXT=context
TRIGGER_TYPE=_GOOMF_enum_no_trigger
ALG_TYPE=$4                 #GooMF algorithm type
INVOCATION_TYPE=$5          #GooMF invocation type
BUFFER_SIZE=$6              #buffer size for history in bytes
OUTPUT_INST_INFO=$7         #output instrumentation information filepath
LINE_MAPPING=$8             #line mapping file (.../Tool/ILP/instLines.txt)
STATIC=$9
$JAVA_CMD $JAR_PREFIX.Instrumentor.GoomfInstrumentor \
        "$HEADER_OUTPUT_PATH"  "$SPEC_FILEPATH" "$VAR_FILEPATH" "$CONTEXT" "$TRIGGER_TYPE" \
        "$ALG_TYPE" "$INVOCATION_TYPE" "$BUFFER_SIZE" "$OUTPUT_INST_INFO" "$LINE_MAPPING" "$STATIC"

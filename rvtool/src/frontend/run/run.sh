#!/bin/bash

SCRIPT_DIR=.
TOOL_DIR=.
SRC_DIR=$1
WORK_DIR=$2
SPEC_FILEPATH=$3
if [ $# -eq 4 ];
then
	TRACE=$4
else
	SOLVE_MODE=$4
	ISP=$5
	ALG_TYPE=$6
	INVOC_TYPE=$7
	BUFFER_SIZE=$8
	TRACE=$9
fi	



$SCRIPT_DIR/copy-src.sh $SRC_DIR $WORK_DIR
if [ $# -eq 4 ];
then
	cp $WORK_DIR/*.h "${WORK_DIR}"/src
	cp ./monitor/buffer.h "${WORK_DIR}"/src
	cp ./monitor/MonitorCodeo.template ./monitor/MonitorCode.template
else
	cp ./monitor/static_headers/*  "${WORK_DIR}"/src
fi

WORK_DIR=`realpath $2`
cp $WORK_DIR/*.h $WORK_DIR/src 
#echo "$SCRIPT_DIR/globalize.sh $SPEC_FILEPATH $WORK_DIR/src/ $WORK_DIR/critInst.txt $WORK_DIR/goomfMeta.txt"
$SCRIPT_DIR/globalize.sh $SPEC_FILEPATH $WORK_DIR/src/ $WORK_DIR/critInst.txt $WORK_DIR/goomfMeta.txt $TRACE

$SCRIPT_DIR/GooMFRun.sh $SPEC_FILEPATH $WORK_DIR/src/ $TOOL_DIR/GooMF/ $WORK_DIR/

if [ $# -ne 4 ];
then
	$SCRIPT_DIR/generate-cfg.sh $WORK_DIR -ilp $ISP
else
	$SCRIPT_DIR/generate-cfg.sh $WORK_DIR
fi

if [ $# != 4 ]; then

	CFG=$WORK_DIR/Tool/Graph/graph.txt
	CCFG=$WORK_DIR/Tool/Graph/critical_CFG
	
	INSTRUMENT_SCHEME=$WORK_DIR/Tool/ILP/instLines.txt
	
	case $SOLVE_MODE in
	    'ilp' )
	        $SCRIPT_DIR/run-ttrv.sh $WORK_DIR ilp $ISP 1
	        INSTRUMENT_SCHEME=$WORK_DIR/Tool/ILP/instLines.txt
	        ;;
	    'heu1' )
	        $SCRIPT_DIR/run-ttrv.sh $WORK_DIR heu1 $ISP 1
	        INSTRUMENT_SCHEME=$WORK_DIR/Tool/Heu/instLines_Heu1.txt
	        ;;
	    'heu2' )
	        $SCRIPT_DIR/run-ttrv.sh $WORK_DIR heu2 $ISP 1
	        INSTRUMENT_SCHEME=$WORK_DIR/Tool/Heu/instLines_Heu2.txt
	        ;;
	    'sat' )
	        $SCRIPT_DIR/sat_solver.sh $CFG $ISP
		INSTRUMENT_SCHEME=$WORK_DIR/Tool/SMIRF_SAT_solution
	        ;;
	    'greedy' )
	        $SCRIPT_DIR/greedy_solver.sh $CFG $ISP $WORK_DIR
		INSTRUMENT_SCHEME=$WORK_DIR/Tool/SMIRF_greedy_solution
	        ;;
	esac

	$SCRIPT_DIR/goomf-inst.sh $WORK_DIR/src/GooMFInstrumentor.h $SPEC_FILEPATH $WORK_DIR/goomfMeta.txt \
	    $ALG_TYPE $INVOC_TYPE $BUFFER_SIZE $WORK_DIR/instrumentation.txt $INSTRUMENT_SCHEME 1
	    
	csplit ./monitor/static_headers/MonitorCode.template /--------------/ {1} 
	$SCRIPT_DIR/auto-instrument.sh $WORK_DIR/src/  $WORK_DIR/instrumentation.txt $WORK_DIR/mainMeta.txt xx00 xx01 xx02
	
	$SCRIPT_DIR/generate-mon.sh ./monitor/ThreadMonitor.c $SPEC_FILEPATH $WORK_DIR/src/RiTHMicMonitor.c $ISP $WORK_DIR/mainMeta.txt $ALG_TYPE $INVOC_TYPE $BUFFER_SIZE

else
	INSTRUMENT_SCHEME=`realpath $WORK_DIR/Tool/Pass/instLines.txt`
	ALG_TYPE="1" 
	INVOC_TYPE="2"
	BUFFER_SIZE="3"
	echo "$SCRIPT_DIR/goomf-inst.sh $WORK_DIR/src/GooMFInstrumentor.h $SPEC_FILEPATH $WRK_DIR/goomfMeta.txt $ALG_TYPE $INVOC_TYPE $BUFFER_SIZE $WORK_DIR/instrumentation.txt $INSTRUMENT_SCHEME"
	echo "yogi"
	$SCRIPT_DIR/goomf-inst.sh $WORK_DIR/src/GooMFInstrumentor.h $SPEC_FILEPATH $WORK_DIR/goomfMeta.txt \
	    $ALG_TYPE $INVOC_TYPE $BUFFER_SIZE $WORK_DIR/instrumentation.txt $INSTRUMENT_SCHEME 0
	rm xx0*    
	$SCRIPT_DIR/generate-mon.sh ./monitor/MonitorCode.template $SPEC_FILEPATH
	csplit ./monitor/MonitorCode.template /--------------/ {1}

	$SCRIPT_DIR/auto-instrument.sh $WORK_DIR/src/ $WORK_DIR/instrumentation.txt $WORK_DIR/mainMeta.txt xx00 xx01 xx02
fi

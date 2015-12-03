#!/bin/bash


SHELL_DIR=$PWD

JAVA_CMD='java -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAR_PREFIX=ca.uwaterloo.esg.rvtool.TTRV

LPSOLVE=$SHELL_DIR/solvers/lp_solve/lp_solve


if [ $# -ne 4 ]; then
    echo "Usage: $0 <working dir> <solve mode> <sampling period> <may aliases>"
	exit
fi

if [ ! -d $1 ]; then
    echo "$1 is not a directory"
    exit
fi

if [ ! -d $1/src ]; then
    echo "$1 is not a valid project directory; project directory must contain src/"
    exit
fi

PROGRAM_DIR=$1
SOLVE_MODE=$2
ISP=$3
MAY_ALIAS=$4


DIR="$PROGRAM_DIR/src"
OUT_DIR="$PROGRAM_DIR/Tool"

#if [ $SOLVE_MODE = "ilp" ]; then
#    $GEN_ILP_MODEL $1 -ilp
#else
#    $GEN_ILP_MODEL $1
#fi

if [ $SOLVE_MODE = "ilp" ];
then
    $LPSOLVE $OUT_DIR/ILP/out_histModel.lp &> $OUT_DIR/ILP/out_ilp.txt
    SOLVE_RESULT=$OUT_DIR/ILP/out_ilp.txt
elif [ $SOLVE_MODE = "heu1" ];
then
    $JAVA_CMD $JAR_PREFIX.Heuristics.CFG $SOLVE_MODE $OUT_DIR/Graph/critical_CFG $ISP $OUT_DIR/Heu/Heu1_results.txt
    SOLVE_RESULT=$OUT_DIR/Heu/Heu1_results.txt
elif [ $SOLVE_MODE = "heu2" ];
then
    $JAVA_CMD $JAR_PREFIX.Heuristics.CFG $SOLVE_MODE $OUT_DIR/Graph/critical_CFG $ISP $OUT_DIR/Heu/Heu2_results.txt 500 #TODO: make the number of iterations a modifiable parameter
    SOLVE_RESULT=$OUT_DIR/Heu/Heu2_results.txt
fi

# Extract instrumentation points from solution
$JAVA_CMD $JAR_PREFIX.FindInstLines.FindInstLines "$OUT_DIR" $SOLVE_MODE $SOLVE_RESULT

if [ $SOLVE_MODE = "ilp" ];
then
    INST_LINES=$OUT_DIR/ILP/out_ilp.txt
elif [ $SOLVE_MODE = "heu1" ];
then
    INST_LINES=$OUT_DIR/Heu/instLines_Heu1.txt
elif [ $SOLVE_MODE = "heu2" ];
then
    INST_LINES=$OUT_DIR/Heu/instLines_Heu2.txt
fi  


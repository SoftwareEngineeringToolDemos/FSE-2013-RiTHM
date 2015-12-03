#!/bin/sh

###################ENVIRONMENT VARIABLES################################################
PATH=./yices-1.0.36/bin:$PATH
JAVA_CMD='java -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAVA_PREFIX=ca.uwaterloo.esg.rvtool.SMIRF
########################################################################################
if [ $# -ne 3 ]
then
    echo "Usage: $0 <path to CFG> <sampling period> <working directory>"
    exit 1
fi
cfg=$1
sampling_period=$2
WORKING_DIR=$3
upper_bound=`$JAVA_CMD $JAVA_PREFIX.SAT_Translator $cfg $sampling_period -greedy $WORKING_DIR`

echo "Number of Vertices(Greedy Solution): " $upper_bound

cat sat_solution | grep "(= y" | grep "1)" | cut -d" " -f 2| cut -d"y" -f 2 > $WORKING_DIR/Tool/SMIRF_greedy_solution
rm sat_solution
 

#!/bin/bash

###################ENVIRONMENT VARIABLES################################################
PATH=./solvers/yices-1.0.36/bin:$PATH
JAVA_CMD='java -Xmx1000M -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAVA_PREFIX=ca.uwaterloo.esg.rvtool.SMIRF
WORKING_DIR=./temp/Dot
########################################################################################

function index_finder()
{
	lower_bound=$1
	upper_bound=$2
	index=`expr $lower_bound + $upper_bound`
	echo `expr $index / 2`
}

function smt_creater_2()
{
	yices_file=$2
	tmp_path="tmp"
	original_string=`cat $yices_file|grep "(assert (<= (+ y"`
	substring=${original_string:0:$length_original_string}"$1))"
#	sed "s/$original_string/$substring/g" $yices_file > $tmp_path
	perl -pi -e "s/\Q$original_string/$substring/" $yices_file #> $tmp_path
#	mv $tmp_path $yices_file 
	cat $yices_file| grep "(assert (<= (+ y"
}

if [ $# -ne 2 ]
then
    echo 'Usage: $0 <path to CFG> <sampling period>'
    exit 1
fi
cfg=$1
sampling_period=$2
mkdir -p $WORKING_DIR
max_vertices=`$JAVA_CMD $JAVA_PREFIX.SAT_Translator $cfg $sampling_period -sat $WORKING_DIR`
echo "Number of Vertices: "$max_vertices
lower_bound=0
upper_bound=1
flag=1

yices_file="SAT_expression.ys"
original_string=`cat $yices_file|grep "(assert (<= (+ y"`
length_original_string=`expr ${#original_string} - 3`
#length_original_string=`cat "$original_string"|wc -c`
#length_orginal_string=`expr $length_original_string - 4`

smt_creater_2 $upper_bound $yices_file
output=`yices -e $yices_file|grep -c "unsat"`
while [ $output != 0 ] && [ $upper_bound -le $max_vertices ]
do
	lower_bound=$upper_bound
	echo "Lower Bound: "$lower_bound
	upper_bound=`expr $upper_bound \* 2`
	echo "Upper Bound: "$upper_bound
	smt_creater_2 $upper_bound $yices_file
	output=`yices -e $yices_file|grep -c "unsat"`
done 

if [ $upper_bound -gt $max_vertices ]
then
	upper_bound=$max_vertices
fi

while [ $lower_bound -lt $upper_bound ]
do
	echo $lower_bound $upper_bound
	index=$(index_finder $lower_bound $upper_bound)
	smt_creater_2 $index $yices_file	

	output=`yices -e $yices_file | grep -c "unsat"`
	echo "Output: "$output
	if [ $output -eq 0 ]
	then
		upper_bound=$index
	else
		if [ $index -eq $lower_bound ]
		then
			smt_creater_2 $upper_bound $yices_file
			flag=0 
		fi
		lower_bound=$index
	fi
	if [ $flag -eq 0 ]
	then
		break
	fi
done  

yices -e $yices_file | grep "(= y" | grep "1)" | cut -d" " -f 2| cut -d"y" -f 2 > sat_solution

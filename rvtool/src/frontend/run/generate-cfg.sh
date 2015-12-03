#!/bin/bash

SHELL_DIR=$PWD

LLVM_DIR=$SHELL_DIR/llvm
LLVM_BIN_DIR=$LLVM_DIR/build/Release+Asserts/bin
LLVM_LIB_DIR=$LLVM_DIR/build/Release+Asserts/lib
LLVM_CC=$LLVM_BIN_DIR/clang
LLVM_OPT=$LLVM_BIN_DIR/opt

CLEAN_ALIAS_EXE=$SHELL_DIR/TTRV/clean-alias

JAVA_CMD='java -cp ./jars/rvtool.jar:./jars/opencsv-2.3.jar:./jars/commons-io-2.4.jar'
JAR_PREFIX=ca.uwaterloo.esg.rvtool.TTRV

#echo "Do you require mayaliases to be considered in analysis (1 for YES, 0 for NO) >"
MAY_ALIAS=1
#echo "Enter required sampling period (in cycles ex. 9) >"
#ISP=9
ISP=$3

if [ $# == 3 ]; then
    ISP=$3
else
	ISP="NA"	
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

VAR_FILE=$PROGRAM_DIR/critInst.txt

# Set second arg to -ilp to generate ILP model
if [ $# -gt 1 ]; then
    GEN_ILP=$2
fi


function list_files()
{
	if [ ! -d "$1" ]; 
	then
        echo " its not a dir $1"; 
        return;
	fi
	cfgfile=$(ls $1/src/*.csf)
    files=$(ls $1/src/*.c )
	for i in $files
	do
        file_name=`basename "$i"`
        flag=""
		flag=`awk -v filename=$file_name '{if($0~filename"=")print $0;}' $cfgfile`
		if [[ "$flag" != "" ]]; then
			compiler_string=`awk -F= -v filename=$file_name '{if($0~filename"=")print $2;}' $cfgfile`
	        # Compile file to LLVM IR
			$LLVM_CC -g -emit-llvm $compiler_string $i -c -o "$2/IR/$file_name.bc"
			if [ $? -eq 0 ];
			then
				echo "Byte code Compilation Successful for " $file_name 
			else
				echo "Byte code Compilation failed for " $file_name
			fi
			
	        # Rename the temporary registers of the IR
	        $LLVM_OPT   -load "$LLVM_LIB_DIR/LLVMtool_rename.so" -tool_rename \
	                    -dir "$2" "$2/IR/$file_name.bc" -o "$2/IR/renamed_$file_name.bc"
	        # Conduct alias analysis (Replace -basicaa with any alias analysis you have in LLVM distro)
	        $LLVM_OPT -basicaa -print-alias-sets "$2/IR/renamed_$file_name.bc" &> alias.txt
			$CLEAN_ALIAS_EXE "." "$2"
			rm alias.txt
	        # Find the IR instruction that may affect the aliases but cannot require a name
			$LLVM_OPT   -load "$LLVM_LIB_DIR/LLVMtool_findnulls.so" -tool_findnulls \
	                    "$2/IR/renamed_$file_name.bc" -o "$2/IR/renamed_$file_name.bc"
			cat Nulls.txt >> "$2/Nulls/Nulls.txt"
			rm Nulls.txt
		else
			echo "File " $file_name " will not be processed since not found in the config file"
		fi	
	done
}


# Make the directories required for finding the cirical instructions
mkdir -p $PROGRAM_DIR/Tool/LLVM
mkdir -p $PROGRAM_DIR/Tool/Alias
mkdir -p $PROGRAM_DIR/Tool/Nulls
mkdir -p $PROGRAM_DIR/Tool/Pass
mkdir -p $PROGRAM_DIR/Tool/CFG
mkdir -p $PROGRAM_DIR/Tool/Graph
mkdir -p $PROGRAM_DIR/Tool/ILP
mkdir -p $PROGRAM_DIR/Tool/Heu
mkdir -p $PROGRAM_DIR/Tool/IR
 
touch "$PROGRAM_DIR/Tool/Alias/alias.txt"
touch "$PROGRAM_DIR/Tool/Nulls/Nulls.txt"

DIR="$PROGRAM_DIR"
#/src"
OUT_DIR="$PROGRAM_DIR/Tool"

list_files "$DIR" "$OUT_DIR"


# Extract the variables of interest for LLVM processing
# Extract the variables of interest replace the first $1 with directory of variables.txt
#$JAVA_CMD $JAR_PREFIX.VarExtractor.VarCreator "$PROGRAM_DIR" "$VAR_FILE" "$OUT_DIR"

mv "$VAR_FILE" "$OUT_DIR/LLVM"

# Process the raw data aquired from the LLVM passes
# Extract alias sets 
$JAVA_CMD $JAR_PREFIX.ExtractAlias.Extractor "$OUT_DIR"
# Extract the alias registers that do not have names
$JAVA_CMD $JAR_PREFIX.NullFinder.NullFinder "$OUT_DIR"

#$BC_FILES=`ls $OUT_DIR/IR/*`
for i in $OUT_DIR/IR/*
do
	$LLVM_OPT   -load "$LLVM_LIB_DIR/LLVMtool_critinst.so" -tool_critinst \
                -mayalias $MAY_ALIAS -dir "$OUT_DIR" "$i" -o "$i"
done

# Cleaning out the final result
$JAVA_CMD $JAR_PREFIX.Cleaner.Cleaning "$OUT_DIR"
echo $ISP
if [[ "$ISP" != "NA" ]]; then
	
	echo "///---------------- Starting Creation of Critical CFG -------------------///"
	for i in $(ls $OUT_DIR/IR/*.bc)
	do
		echo $i
		$LLVM_OPT   -load "$LLVM_LIB_DIR/LLVMtool_findlines.so" -tool_findlines \
	                -dir "$OUT_DIR" "$i" -o "$i"
		
	    # Extract raw CFG
		$LLVM_OPT -dot-cfg-only "$i" 
		mv *.dot $OUT_DIR/CFG/
		#cfg_files=$(find . -type f -name *.dot)
		#for j in $cfg_files
		#do
	#		if [ ! -d "$j" ];
	#		then 
	#		    mv "$j" "$OUT_DIR/CFG"
	#		fi
	#	done
	done
	
	# Extract alias sets
	$JAVA_CMD $JAR_PREFIX.FunctionCleaner.FunctionCleaner "$OUT_DIR"
	# Calculates the LSP
	$JAVA_CMD $JAR_PREFIX.CfgParser.CFGParser "$OUT_DIR" $ISP $GEN_ILP
fi



#!/bin/sh


shell_dir=$PWD

if [ "$1" == "" ]; then
    echo "Add the directory of monitored program"
	exit
fi


function list_files()
{
 if !(test -d "$1") 
 then echo " its not a dir $1"; return;
 fi

 cd "$1"
 
 files=$(find *.c)
 for i in $files
 do
	if !(test -d "$i")
	then 
		llvm-gcc -g -emit-llvm $i -c -o "$2/IR/$i.bc" #Compiling to the IR of LLVM
		opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_rename.dylib" -tool_rename -dir "$2" "$2/IR/$i.bc" -o "$2/IR/renamed_$i.bc" #Renaming the temporary registers of the IR
		opt -basicaa -print-alias-sets "$2/IR/renamed_$i.bc" &> alias.txt # Conducting alias analysis (Replace -basicaa with any alias analysis you have in LLVM distribution)
		"$shell_dir/bin/clean.o" "$1" "$2"
		rm alias.txt
		opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_findnulls.dylib" -tool_findnulls "$2/IR/renamed_$i.bc" -o "$2/IR/renamed_$i.bc" #find the IR instructions that may affect the aliases but can not require a name
		cat Nulls.txt >> "$2/Nulls/Nulls.txt"
		rm Nulls.txt
	fi
 done

 for i in *
 do
	if test -d "$i" #if dictionary
	then 
		list_files "$i" "$2" #recursively list files
		cd ..
	fi

 done
}


DIR="."

cd "$1"

mkdir Tool
 
# Make the directories required for fising the cirical instructions
mkdir Tool/LLVM
mkdir Tool/Alias
mkdir Tool/Nulls
mkdir Tool/Pass
mkdir Tool/CFG
mkdir Tool/Graph
mkdir Tool/ILP
mkdir Tool/IR
mkdir Tool/Heu

 
touch "$1/Tool/Alias/alias.txt"
touch "$1/Tool/Nulls/Nulls.txt"


if [ $# -eq 0 ]
then list_files .
exit 0
fi

DIR="$1/src"

list_files "$DIR" "$1/Tool"

 
# Extract the variables of interest for LLVm processing
 
cd "$shell_dir/bin"
echo "Enter file name for monitoring variables >"
read varFile 
java -jar VarExtractor.jar "$1" "$varFile" "$1/Tool" # Extract the variables of interest replace the first $1 with directory of variables.txt

mv "$1/Tool/critInst.txt" "$1/Tool/LLVM"


#Process the raw data aquired from the LLVM passes 

java -jar extractAlias.jar "$1/Tool" #Extract alias sets
java -jar nullFinder.jar "$1/Tool" #Extract the alias registers that do not have names

echo "Do you require mayaliases to be considered in analysis (1 for YES, 0 for NO) >"
read may_alias


cd "$1/Tool/IR"

for i in *
do
	opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_critinst.dylib" -tool_critinst -mayalias $may_alias -dir "$1/Tool" "$i" -o "$i"
done

cd "$shell_dir/bin"

java -jar cleaner.jar "$1/Tool" #Cleaning out the final result

echo "\n\n---------------- Starting Creation of Critical CFG -------------------\n\n"

cd "$1/Tool/IR"

for i in *
do
	opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_findlines.dylib" -tool_findlines -dir "$1/Tool" "$i" -o "$i"
	
	opt -dot-cfg-only "$i" #Extract raw CFG
	cfg_files=$(find *.dot)
	for j in $cfg_files
	do
		if !(test -d "$j")
		then 
		mv "$j" "$1/Tool/CFG"
		fi
	done
done


cd "$shell_dir/bin"
java -jar functionCleaner.jar "$1/Tool" #Extract alias sets


echo "Enter required sampling period (in cycles ex. 9) >"
read ISP
java -jar CFGCreator.jar "$1/Tool" "$ISP" # Calculates the LSP

cd "$1/Tool/ILP"
lp_solve out_histModel.lp &> out_ilp.txt #solving the ILP problem

cd "$shell_dir/bin"


echo "Enter Path to Heuristic 1 Result >"
read HEU1
echo "Enter Path to Heuristic 2 Result >"
read HEU2
java -jar FindInstLines.jar "$1/Tool" "ilp" "$HEU1" "$HEU2" # Extracting instrumentation lines from ilp
java -jar FindInstLines.jar "$1/Tool" "heu" "$HEU1" "$HEU2" # Extracting instrumentation lines from the heuristics



context="context"
echo "Enter trigger type for GOOMF >"
read triggerType
echo "Enter algorithm type for GOOMF >"
read algType
echo "Enter invocation type for GOOMF >"
read invocationType
echo "Enter buffer size for GOOMF >"
read bufferSize
echo "Enter name of program (name.cfg) >"
read fileName

java -jar InstrumentorHelper.jar "$1" "$1/Tool" "$context" "$triggerType" "$algType" "$invocationType" "$bufferSize" "$fileName" "$varFile"




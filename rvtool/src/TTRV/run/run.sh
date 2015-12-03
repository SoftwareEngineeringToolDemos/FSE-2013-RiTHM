#!/bin/sh


shell_dir=$PWD

if [ "$1" == "" ]; then
    echo "Add the directory of monitored program"
	exit
fi

DIR="."

function list_files()
{
 if !(test -d "$1/src") 
 then echo " its not a dir $1/src"; return;
 fi

 cd "$1/src"
 count=$(find . | grep *\.c -c)
 
 # Make the directories required for fising the cirical instructions
 mkdir LLVM
 mkdir Alias
 mkdir Nulls
 mkdir Pass
 mkdir CFG
 mkdir Graph
 mkdir ILP
 mkdir IR
 
 
 if [ $count == 1 ]
 then
	# There exists only one c file
	files=$(find *.c)
	for i in $files
	do
		if !(test -d "$i")
		then 
		llvm-gcc -g -emit-llvm $i -c -o "$1/src/IR/$i.bc" #Compiling to the IR of LLVM
		opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_rename.dylib" -tool_rename "$1/src/IR/$i.bc" -o "$1/src/IR/renamed_$i.bc" #Renaming the temporary registers of the IR
		opt -basicaa -print-alias-sets "$1/src/IR/renamed_$i.bc" &> alias.txt # Conducting alias analysis (Replace -basicaa with any alias analysis you have in LLVM distribution)
		opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_findnulls.dylib" -tool_findnulls "$1/src/IR/renamed_$i.bc" -o "$1/src/IR/renamed_$i.bc" #find the IR instructions that may affect the aliases but can not require a name
		fi
	done
	
 else
	# processing for more than one c file, need linking
	 echo "linking required"
 fi
 
 mv func.txt alias.txt Alias
 mv Nulls.txt Nulls 

}





if [ $# -eq 0 ]
then list_files .
exit 0
fi

DIR="$1"
list_files "$DIR"
 
# Extract the variables of interest for LLVm processing
 
cd "$shell_dir/bin"
echo "Enter file name for monitoring variables >"
read varFile 
java -jar VarExtractor.jar "$1" "$varFile" "$1/src" # Extract the variables of interest replace the first $1 with directory of variables.txt

mv "$1/src/critInst.txt" "$1/src/LLVM"


#Process the raw data aquired from the LLVM passes 

java -jar extractAlias.jar "$1/src" #Extract alias sets
java -jar nullFinder.jar "$1/src" #Extract the alias registers that do not have names

echo "Do you require mayaliases to be considered in analysis (1 for YES, 0 for NO) >"
read may_alias
cd "$1/src"

opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_critinst.dylib" -tool_critinst -mayalias $may_alias "$1/src/IR/renamed_$i.bc" -o "$1/src/IR/renamed_$i.bc"

cd "$shell_dir/bin"

java -jar cleaner.jar "$1/src" #Cleaning out the final result

echo "\n\n---------------- Starting Creation of Critical CFG -------------------\n\n"

cd "$1/src"

opt -load "$LLVM_PATH/Release+Asserts/lib/LLVMtool_findlines.dylib" -tool_findlines "$1/src/IR/renamed_$i.bc" -o "$1/src/IR/renamed_$i.bc"

opt -dot-cfg-only "$1/src/IR/renamed_$i.bc" #Extract raw CFG
cfg_files=$(find *.dot)
	for i in $cfg_files
	do
		if !(test -d "$i")
		then 
		mv "$i" CFG
		fi
	done

cd "$shell_dir/bin"
java -jar functionCleaner.jar "$1/src" #Extract alias sets


echo "Enter required sampling period (in cycles ex. 9) >"
read ISP
java -jar CFGCreator.jar "$1/src" "$ISP" # Calculates the LSP

cd "$1/src/ILP"
lp_solve out_histModel.lp &> out_ilp.txt #solving the ILP problem

cd "$shell_dir/bin"

java -jar FindInstLines.jar "$1/src" # Extracting instrumentation lines


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

java -jar InstrumentorHelper.jar "$1" "$1/src" "$context" "$triggerType" "$algType" "$invocationType" "$bufferSize" "$fileName" "$varFile"

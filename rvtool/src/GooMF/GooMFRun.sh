#!/bin/bash
echo '==================================================='
if [ $# -ne 4 ]; then 
	echo 'Usage: $0 <path to properties file> <path to application dir> <path to GooMF> <output dir of GooMF shared library>'	
	exit 1
fi

PROP_FILE_PATH=`realpath $1`
APPLICATION_PATH=`realpath $2`
GOOMF_PATH=`realpath $3`
GOOMF_LIB_OUT=`realpath $4`
LIBCONFIG_PATH=`realpath ../libconfig-1.4.8/lib/.libs`

echo 'properties file path = '$PROP_FILE_PATH
echo 'application path = '$APPLICATION_PATH
echo 'GooMF library path = '$GOOMF_PATH
echo 'GooMF output lib path = '$GOOMF_LIB_OUT
echo 'libconfig 1.4.8 path = '$LIBCONFIG_PATH

export LD_LIBRARY_PATH=$LIBCONFIG_PATH:/opt/AMDAPP/lib/x86_64:$LD_LIBRARY_PATH

echo '==================================================='
echo 'generating program state and kernels...'
cd $GOOMF_PATH
cd ./GooMFGenerator
./GooMFGenerator -c $PROP_FILE_PATH -o $APPLICATION_PATH -l $GOOMF_PATH/GooMFGenerator/ltl3tools-0.0.7 -f ifs

echo '==================================================='
echo 'copying generated files...'
cp ./GooMF_CPU_monitor.h $GOOMF_PATH'/GooMFLibrary/GooMF_CPU_monitor.h'
cp ./ProgramState.h $APPLICATION_PATH'/ProgramState.h'
cp ./GooMF_GPU_monitor_alg_finite.cl $APPLICATION_PATH'/GooMF_GPU_monitor_alg_finite.cl'
cp ./GooMF_GPU_monitor_alg_infinite.cl $APPLICATION_PATH'/GooMF_GPU_monitor_alg_infinite.cl'
cp ./GooMF_GPU_monitor_alg_partial.cl $APPLICATION_PATH'/GooMF_GPU_monitor_alg_partial.cl'
echo $GOOMF_PATH'/GooMFLibrary/GooMF_CPU_monitor.h'
echo $APPLICATION_PATH'/ProgramState.h'
echo $APPLICATION_PATH'/GooMF_GPU_monitor_alg_finite.cl'
echo $APPLICATION_PATH'/GooMF_GPU_monitor_alg_infinite.cl'
echo $APPLICATION_PATH'/GooMF_GPU_monitor_alg_partial.cl'
echo '==================================================='
echo 'recompiling GooMF shared library...'
cd $GOOMF_PATH/GooMFLibrary
echo 'currently in:'
pwd
echo 'compiling...'
gcc -I/opt/AMDAPP/include -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"GooMF.d" -MT"GooMF.d" -o"GooMF.o" "GooMF.c"
gcc -I/opt/AMDAPP/include -fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"ParamRepo.d" -MT"ParamRepo.d" -o"ParamRepo.o" "ParamRepo.c"
echo 'linking...'
gcc -L/opt/AMDAPP/lib/x86_64 -shared -o"libGooMF.so"  ./GooMF.o ./ParamRepo.o   -lOpenCL -lm
mkdir -p ${GOOMF_LIB_OUT}
cp libGooMF.so ${GOOMF_LIB_OUT}/libGooMF.so
#ldconfig -n /usr/lib
#echo 'ldconfig...'
#echo ''

echo "finished!"
exit 0

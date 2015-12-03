#!/bin/bash

# run this script with sudo

BUILD_DIR=./build
SRC_DIR=./src

function install_from_apt_get {
    PACKAGE_NAME=$1
    PACKAGE_INSTALL=$(dpkg-query -W --showformat='${Status}\n' $PACKAGE_NAME|grep "install ok installed")
    if [ -z "$PACKAGE_INSTALL" ]; then
        sudo apt-get install $PACKAGE_NAME
    else
        echo "$PACKAGE_NAME already installed!"
    fi
}
function get_qwt {
    QWT_URL="http://downloads.sourceforge.net/project/qwt/qwt-beta/6.1.0-rc3/qwt-6.1-rc3.tar.bz2"
    wget -O qwt-6.1-rc3.tar.bz2 $QWT_URL
    tar -xjvf qwt-6.1-rc3.tar.bz2
    cd qwt-6.1-rc3
    qmake qwt.pro
    make
    sudo make install
}
function check_apt_get_deps {
    OS_ARCH=`uname -p`
    if [ "$OS_ARCH" = "x86_64" ]; then
        install_from_apt_get ia32-libs
    fi

    install_from_apt_get realpath
    install_from_apt_get subversion
    install_from_apt_get g++
    install_from_apt_get libqt4-core
    install_from_apt_get libqt4-dev
}

function unpack_solvers {
    SOLVERS_DIR=$BUILD_DIR/solvers
    mkdir -p $SOLVERS_DIR
    LP_SOLVE=$(find $SOLVERS_DIR -type f -name lp_solve)
    YICES=$(find $SOLVERS_DIR -type f -name yices)
    
    OS_ARCH=`uname -p`

    if [ -z $LP_SOLVE ]; then
        LP_SOLVE_URL="http://downloads.sourceforge.net/project/lpsolve/lpsolve/5.5.2.0/lp_solve_5.5.2.0_exe_ux32.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Flpsolve%2F&ts=1351717641&use_mirror=iweb"
        if [ "$OS_ARCH" = "x86_64" ]; then
            LP_SOLVE_URL="http://downloads.sourceforge.net/project/lpsolve/lpsolve/5.5.2.0/lp_solve_5.5.2.0_exe_ux64.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Flpsolve%2Ffiles%2Flpsolve%2F5.5.2.0%2F&ts=1351717708&use_mirror=iweb"
        fi
        mkdir -p $SOLVERS_DIR/lp_solve
        wget -O $SOLVERS_DIR/lp_solve/lp_solve.tar.gz $LP_SOLVE_URL
        pushd .
        cd $SOLVERS_DIR/lp_solve
        tar -xvzf ./lp_solve.tar.gz
        popd
    fi

    if [ -z $YICES ]; then
        YICES_URL="http://yices.csl.sri.com/cgi-bin/yices-newlicense.cgi?file=yices-1.0.36-i686-pc-linux-gnu-static-gmp.tar.gz"
        if [ "$OS_ARCH" = "x86_64" ]; then
            YICES_URL="http://yices.csl.sri.com/cgi-bin/yices-newlicense.cgi?file=yices-1.0.36-x86_64-pc-linux-gnu-static-gmp.tar.gz"
        fi
        echo "==============="
        echo "SRI requires the user to accept their end-user agreement before downloading Yices."
        echo "Save and untar the archive under $SOLVERS_DIR"
        echo "$YICES_URL"
        echo "==============="
    fi

}

function checkout_llvm {
    ABS_BUILD_DIR=`realpath $BUILD_DIR`
    LLVM_CLANG_VERSION=3.2
    LLVM_DIR=llvm
    LLVM_URL=http://llvm.org/releases/$LLVM_CLANG_VERSION/llvm-$LLVM_CLANG_VERSION.src.tar.gz
    CLANG_URL=http://llvm.org/releases/$LLVM_CLANG_VERSION/clang-$LLVM_CLANG_VERSION.src.tar.gz
    
    if [ ! -d $ABS_BUILD_DIR/$LLVM_DIR ]; then
        echo "Setup LLVM and clang (v.$LLVM_CLANG_VERSION)"
        echo "Target directory: $ABS_BUILD_DIR/$LLVM_DIR"
        mkdir -p $ABS_BUILD_DIR/$LLVM_DIR
        pushd .
        cd $ABS_BUILD_DIR
        wget -O llvm.tar.gz $LLVM_URL
        tar -xvzf llvm.tar.gz
        mv ./llvm-$LLVM_CLANG_VERSION.src/* ./$LLVM_DIR/ 
        rm -f llvm.tar.gz
        rm -rf ./llvm-$LLVM_CLANG_VERSION.src*
        cd $LLVM_DIR/tools
        wget -O clang.tar.gz $CLANG_URL
        tar -xvzf clang.tar.gz
        mv ./clang-$LLVM_CLANG_VERSION.src ./clang
        rm -f clang.tar.gz
        
        mkdir -p $ABS_BUILD_DIR/$LLVM_DIR/build
        cd $ABS_BUILD_DIR/$LLVM_DIR/build
        ../configure
	echo "Please verify LLVM makefile.config in build directory. Press any Key to Continue.." 
	read char
        popd

        pushd .
        cd ./src/llvm
        ./copy-llvm-plugins.sh $ABS_BUILD_DIR/llvm
        popd
    fi

    if [ "build" = "$1" ]; then
        pushd .
        cd $ABS_BUILD_DIR/$LLVM_DIR/build
        make
        popd
    fi
}

function build_libconfig {

    LIBCONFIG_VERSION=1.4.8
    LIBCONFIG=libconfig-$LIBCONFIG_VERSION
    LIBCONFIG_DIR=$BUILD_DIR/$LIBCONFIG
    
    echo "Setup libconfig (v.$LIBCONFIG_VERSION)"
    echo "Target directory: $LIBCONFIG_DIR"
    
    if [ ! -d $LIBCONFIG_DIR ]; then
        echo "$LIBCONFIG_DIR does not exist. Download and configuring libconfig..."
        wget -O $BUILD_DIR/$LIBCONFIG.tar.gz  http://www.hyperrealm.com/libconfig/$LIBCONFIG.tar.gz
        mkdir -p $BUILD_DIR
        pushd .
        cd $BUILD_DIR
        tar -xvzf $LIBCONFIG.tar.gz
        cd $LIBCONFIG
        ./configure
        popd
        rm -f $BUILD_DIR/$LIBCONFIG.tar.gz
    fi
    pushd .
    cd $BUILD_DIR/$LIBCONFIG
    make
    popd
    #libraries will be output to libconfig-1.4.8/lib/.libs
}

function unpack_AMDAPPSDK {
    OS_ARCH=`uname -p`
    AMDAPP_VERSION=2.7
    AMDAPP_SDK="AMD-APP-SDK-v$AMDAPP_VERSION-lnx32"
    AMDAPP_BUILD_DIR=$BUILD_DIR/AMDAPP

    if [ -d /opt/AMDAPP ]; then
        echo "AMD-APP-SDK is already installed."
        return
    fi

    if [ "$OS_ARCH" = "x86_64" ]; then
        AMDAPP_SDK="AMD-APP-SDK-v$AMDAPP_VERSION-lnx64"
    fi

    echo "==============="
    echo "AMD requires the user to accept its license agreement before downloading the AMD APP SDK."
    echo "Save and untar the file. Run sudo ./Install-AMD-APP.sh"
    echo "Download link: http://developer.amd.com.php53-23.ord1-1.websitetestlink.com/wordpress/media/files/$AMDAPP_SDK.tgz"
    echo "==============="
    #mkdir -p $AMDAPP_BUILD_DIR/
    #wget  -O $AMDAPP_BUILD_DIR/$AMDAPP_SDK.tgz http://developer.amd.com/Downloads/$AMDAPP_SDK.tgz
    #pushd .
    #cd $AMDAPP_BUILD_DIR
    #tar -xvzf $AMDAPP_SDK.tgz
    #sudo ./Install-AMD-APP.sh 
    #popd
}

function prepare_qtgui_makefile {
    ABS_BUILD_DIR=`realpath $BUILD_DIR`
    GUI_BUILD_DIR=$ABS_BUILD_DIR/frontend/gui
    GUI_OBJ_DIR=$GUI_BUILD_DIR/obj/
    GUI_MOC_DIR=$GUI_BUILD_DIR/moc/
    GUI_UI_H_DIR=$GUI_BUILD_DIR/ui/h/
    GUI_OUTPUT_DIR=$ABS_BUILD_DIR
    
    mkdir -p $GUI_BUILD_DIR
    qmake -o $SRC_DIR/frontend/gui/Makefile "OBJECTS_DIR=$GUI_OBJ_DIR"  \
                    "MOC_DIR=$GUI_MOC_DIR"      \
                    "CONFIG += release"     \
                    "DESTDIR = $GUI_OUTPUT_DIR" \
                    "UI_HEADERS_DIR = $GUI_UI_H_DIR" \
                    $SRC_DIR/frontend/gui/rvtool.pro
} 

function copy_controller_files {
    cd $ABS_BUILD_DIR/../
    mkdir $ABS_BUILD_DIR/monitor
    cp -f ./src/monitor/* $ABS_BUILD_DIR/monitor/
}

mkdir -p $BUILD_DIR

check_apt_get_deps
build_libconfig
checkout_llvm build # pass argument 'build' to build LLVM and clang
unpack_AMDAPPSDK
echo Preparing Qt Makefile
prepare_qtgui_makefile
get_qwt
unpack_solvers
copy_controller_files

#change read/write permissions
sudo chown -R `whoami` ./

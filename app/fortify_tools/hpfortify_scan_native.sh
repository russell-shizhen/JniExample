#!/bin/sh

# Configure NDK version and CMake version
NDK_VERSION=21.0.6113669

CMAKE_VERSION=3.10.2
CMAKE_VERSION_PATH=$CMAKE_VERSION.4988404

PROJECTID="JNI_EXAMPLE"
REPORT_NAME=$PROJECTID"_$(date +'%Y%m%d_%H:%M:%S')"

WORKING_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BUILD_HOME=${WORKING_DIR}/../hpfortify_build
FPR="$BUILD_HOME/$REPORT_NAME.fpr"

# Following exports need to be configured according to host machine.
export ANDROID_SDK_HOME=/Users/shizzhan/Library/Android/sdk
export ANDROID_CMAKE_HOME=$ANDROID_SDK_HOME/cmake/$CMAKE_VERSION_PATH/bin
export ANDROID_NDK_HOME=$ANDROID_SDK_HOME/ndk/$NDK_VERSION
# E.g. JniExample/app/hpfortify/build/CMakeFiles/3.10.2
export CMAKE_FILES_PATH=${BUILD_HOME}/CMakeFiles/$CMAKE_VERSION
export HPFORTIFY_HOME="/Applications/Fortify/Fortify_SCA_and_Apps_20.1.0/bin"
export PATH=$PATH:$ANDROID_SDK_HOME:$ANDROID_NDK_HOME:$ANDROID_CMAKE_HOME:$HPFORTIFY_HOME

echo "[========Start Android JNI C/C++ HP Fortify scanning========]"
echo "[========Build Dir: $BUILD_HOME========]"
echo "[========HP Fortify report path: $FPR========]"

function create_build_folder {
    rm -rf $BUILD_HOME
    mkdir $BUILD_HOME
    cd $BUILD_HOME
}

# The standalone cmake build command can be found from below file. 
# JniExample/app/.cxx/cmake/release/x86/build_command.txt
# This file is generated after running command 
# `➜  JniExample git:(master) ✗ ./gradlew :app:externalNativeBuildRelease`
function configure_cmake_files {

    $ANDROID_CMAKE_HOME/cmake -H$BUILD_HOME \
        -DCMAKE_CXX_FLAGS=-std=c++11 -frtti -fexceptions \
        -DCMAKE_FIND_ROOT_PATH=$BUILD_HOME/.cxx/cmake/release/prefab/x86/prefab \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_SDK_HOME/ndk/$NDK_VERSION/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=x86 \
        -DANDROID_NDK=$ANDROID_SDK_HOME/ndk/$NDK_VERSION \
        -DANDROID_PLATFORM=android-16 \
        -DCMAKE_ANDROID_ARCH_ABI=x86 \
        -DCMAKE_ANDROID_NDK=$ANDROID_SDK_HOME/ndk/$NDK_VERSION \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=$BUILD_HOME/intermediates/cmake/release/obj/x86 \
        -DCMAKE_MAKE_PROGRAM=$ANDROID_SDK_HOME/cmake/$CMAKE_VERSION_PATH/bin/ninja \
        -DCMAKE_SYSTEM_NAME=Android \
        -DCMAKE_SYSTEM_VERSION=16 \
        -B$BUILD_HOME/.cxx/cmake/release/x86 \
        -GNinja ..
}

function build {
    cmake --build .
}

function cleanup {
    rm -rf $BUILD_HOME/CMakeFiles/native-lib.dir
    rm -rf $FPR
    $HPFORTIFY_HOME/sourceanalyzer -clean
}

function replace_compiler_paths {
	FORTIFY_TOOLS_PATH="$WORKING_DIR"
	CLANG_PATH="$ANDROID_SDK_HOME/ndk/$NDK_VERSION/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang"
    CLANGXX_PATH="$ANDROID_SDK_HOME/ndk/$NDK_VERSION/toolchains/llvm/prebuilt/darwin-x86_64/bin/clang++"
	HPFORTIFY_CCPATH="$FORTIFY_TOOLS_PATH/fortify_cc"
	HPFORTIFY_CXXPATH="$FORTIFY_TOOLS_PATH/fortify_cxx"\"
	sed -i '' 's+'$CLANG_PATH'+'$HPFORTIFY_CCPATH'+g' $CMAKE_FILES_PATH/CMakeCCompiler.cmake
    sed -i '' 's+'$CLANG_PATH.*[^")"]'+'$HPFORTIFY_CXXPATH'+g' $CMAKE_FILES_PATH/CMakeCXXCompiler.cmake
}

function scan {
    $HPFORTIFY_HOME/sourceanalyzer -b $PROJECTID -scan -f $FPR

    # copy the file to $WORKING_DIR
    cp $FPR $WORKING_DIR
}


create_build_folder

configure_cmake_files

echo "[========Compile C/C++ using normal compiler ========"]
build

echo "[========Replace the compiler with HP Fortify analyser wrapper compilers ========"]
replace_compiler_paths

echo "[========Clean up the build intermediates and the older build ID and fpr file ========"]
cleanup

echo "[========Recompile C/C++ using HP Fortify analyser wrapper compilers ========"]
build

echo "[========Scan the compiled files and generate final report ========"]
scan

echo "[========Change directory to original working dir ========"]
cd $WORKING_DIR

export XDK_TARGET_PLATFORM=ios
export XDK_COMPILER=llvm

export XDK_TARGET_CPU=x86
if [[ ! "$_ELASTOS64" == "" ]]; then
    export XDK_TARGET_CPU_ARCH=64
    export CMAKE_ARGS=-DIOS_PLATFORM=SIMULATOR64
else
    export XDK_TARGET_CPU_ARCH=32
    export CMAKE_ARGS=-DIOS_PLATFORM=SIMULATOR
fi

export XDK_BUILD_TOOL=cmake


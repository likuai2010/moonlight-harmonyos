#!/bin/bash

ROOT=$(pwd)
INSTALL=$ROOT/install

OHOS_NATIVE_HOME=/Users/macintoshhd/Library/Huawei/Sdk/openharmony/9/native
SYSROOT=$OHOS_NATIVE_HOME/sysroot
LLVM=$OHOS_NATIVE_HOME/llvm

CLANG=$LLVM/bin/clang
CLANGXX=$LLVM/bin/clang++
AR=$LLVM/bin/llvm-ar
AS=$LLVM/bin/llvm-as
NM=$LLVM/bin/llvm-nm
RANLIB=$LLVM/bin/llvm-ranlib
STRIP=$LLVM/bin/llvm-strip
OBJDUMP=$LLVM/bin/llvm-objdump
LD=$LLVM/bin/ld.lld

OHOS_CFLAGS="--target=arm-linux-ohos -march=armv7-a --sysroot=$SYSROOT "
OHOS_CFLAGS+="-I$SYSROOT/usr/include/arm-linux-ohos "
OHOS_CFLAGS+="-L$SYSROOT/usr/lib/arm-linux-ohos "
OHOS_CFLAGS+="-I$TOOLCHAIN/include "
OHOS_CFLAGS+="-L$TOOLCHAIN/lib "

ffmpeg() {
    mkdir -p $INSTALL &&
    cd FFmpeg-n4.1.11 &&
    ./configure \
        --prefix=$INSTALL \
        --arch=armv7-a \
        --target-os=linux \
        --disable-asm \
        --disable-programs \
        --disable-avdevice \
        --enable-cross-compile \
        --enable-small \
        --enable-shared \
        --cc=$CLANG \
        --ld=$LD \
        --strip=$STRIP \
        --extra-cflags="$OHOS_CFLAGS" \
        --extra-ldflags="$OHOS_CFLAGS"
    make j4 && make install
}

# 调用ffmpeg函数
ffmpeg
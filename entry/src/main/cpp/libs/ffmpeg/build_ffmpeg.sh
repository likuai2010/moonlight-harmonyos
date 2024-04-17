#!/bin/bash
ARCH=$1
source build_config.sh $ARCH
LIBS_DIR=$(cd `dirname $0`; pwd)/libs/ffmpeg

PREFIX=$LIBS_DIR/$OHOS_ARCH

echo "PREFIX"=$PREFIX

export CC="$CC"
export CXX="$CXX"
export AR="$AR"
export LD="$LD"
export AS="$AS"
export NM="$NM"
export RANLIB="$RANLIB"
export STRIP="$STRIP"

CFLAGS=$FF_CFLAGS

export CPPFLAGS=$FF_EXTRA_CFLAGS



./configure --prefix=$PREFIX  \
            --arch=${TARGET_ARCH} \
	          --target-os=linux \
            --disable-asm \
            --disable-doc \
            --enable-small \
            --disable-ffmpeg \
            --disable-programs \
            --disable-w32threads \
            --disable-os2threads \
            --disable-network \
            --disable-avdevice \
            --disable-avformat \
            --disable-swresample \
            --disable-swscale \
            --disable-avfilter \
            --enable-cross-compile \
	          --cc=$CC \
            --extra-cflags="$FF_CFLAGS" \
            --extra-ldflags="$FF_CFLAGS" && \
            make && make install
cd ..
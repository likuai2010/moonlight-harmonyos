
ROOT = $(shell pwd)
INSTALL = $(ROOT)/install

OHOS_NATIVE_HOME = /Users/macintoshhd/Library/Huawei/Sdk/openharmony/9/native
# sdk
SYSROOT = $(OHOS_NATIVE_HOME)/sysroot
LLVM = $(OHOS_NATIVE_HOME)/llvm

# compile
CLANG = $(LLVM)/bin/clang
CLANGXX = $(LLVM)/bin/clang++
AR = $(LLVM)/bin/llvm-ar
AS = $(LLVM)/bin/llvm-as
NM = $(LLVM)/bin/llvm-nm
RANLIB = $(LLVM)/bin/llvm-ranlib
STRIP = $(LLVM)/bin/llvm-strip
OBJDUMP = $(LLVM)/bin/llvm-objdump
LD = $(LLVM)/bin/ld.lld

# cflags
OHOS_CFLAGS = --target=arm-linux-ohos -march=armv7-a --sysroot=$(SYSROOT)
OHOS_CFLAGS += -I$(SYSROOT)/usr/include/arm-linux-ohos
OHOS_CFLAGS += -L$(SYSROOT)/usr/lib/arm-linux-ohos
OHOS_CFLAGS += -I$(LLVM)/include
OHOS_CFLAGS += -L$(LLVM)/lib

ffmpeg:
	@mkdir -p $(INSTALL) && \
	cd FFmpeg-n4.1.11 && \
	./configure \
		--prefix=$(INSTALL) \
		--arch=armv7-a \
		--target-os=linux \
		--disable-asm \
		--disable-programs \
		--disable-avdevice \
		--enable-cross-compile \
		--enable-small \
		--enable-shared \
		--cc=$(CLANG) \
		--ld=$(CLANG) \
		--strip=$(STRIP) \
		--extra-cflags="$(OHOS_CFLAGS) $(OHOS_INCS) $(OHOS_LIBS)" \
		--extra-ldflags="$(OHOS_CFLAGS) $(OHOS_INCS) $(OHOS_LIBS)"



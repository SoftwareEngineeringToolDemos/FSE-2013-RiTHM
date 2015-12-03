.PHONY: all clean
.SUFFIXES: .so .o .c

BUILD_DIR=../../build
CC=gcc
OUTPUT_LIB=libclibconfig.so
OUTPUT_DIR=$(BUILD_DIR)/swig
LIBCONFIG_INCLUDE_DIR=$(shell realpath $(BUILD_DIR)/libconfig-1.4.8/lib)
LIBCONFIG_LIB_DIR=$(shell realpath $(BUILD_DIR)/libconfig-1.4.8/lib/.libs)
JNI_INCLUDE_DIR=$(shell find /usr/lib/jvm/ -type f -name jni.h | xargs realpath | xargs dirname | head -n1)

$(OUTPUT_DIR)/$(OUTPUT_LIB): libconfig_wrap.c | $(OUTPUT_DIR)
	$(CC) -shared -fPIC $< -I$(LIBCONFIG_INCLUDE_DIR) -I$(JNI_INCLUDE_DIR) -L$(LIBCONFIG_LIB_DIR) -lconfig -o $@

$(OUTPUT_DIR):
	mkdir -p $@

clean:
	rm -rf $(OUTPUT_DIR)/*


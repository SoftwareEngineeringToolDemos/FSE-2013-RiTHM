.PHONY: clean

INCLUDE_OPENCL=/opt/AMDAPP/include
LIB_OPENCL=/opt/AMDAPP/lib/x86
ifeq ($(shell uname -m),x86_64)
LIB_OPENCL=/opt/AMDAPP/lib/x86_64
endif
CC=gcc
CCFLAGS=-fPIC -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP
LDFLAGS=-shared
LIBS=-lOpenCL -lm

LIB_DIR=lib/

$(LIB_DIR)libGooMF.so: $(LIB_DIR)GooMF.o
	mkdir -p $(LIB_DIR)
	$(CC) -L$(LIB_OPENCL) $(LDFLAGS) -o $@ $< $(LIBS)

$(LIB_DIR)GooMF.o: GooMF.c
	mkdir -p $(LIB_DIR)
	$(CC) -I$(INCLUDE_OPENCL) $(CCFLAGS) -g -MF"GooMF.d" -MT"GooMF.d" -o $@ $<

clean:
	rm -rf $(LIB_DIR)

.SUFFIXES: .h .cpp
.PHONY: all

BUILD_DIR=../../build
LLVM_VER=3.2
llvm_src=$(BUILD_DIR)/llvm
CUR_DIR=$(shell pwd)
SRC=$(shell find `pwd` -type f -name '*.cpp' | sort)
BUILD_SRC=$(subst $(CUR_DIR),$(llvm_src),$(SRC))
HEADER=$(shell find `pwd` -type f -name '*.h' | sort)
BUILD_HEADER=$(subst $(CUR_DIR),$(llvm_src),$(HEADER))

all: $(BUILD_SRC) $(BUILD_HEADER) $(llvm_src)
	$(MAKE) --directory=$(llvm_src)/build $(MAKE_TARGET)

$(llvm_src)/%.cpp: ./%.cpp
	cp $< $@

$(llvm_src)/%.h: ./%.h 
	cp $< $@

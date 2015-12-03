# make MAKE_TARGET=clean will invoke clean target for all components

SRC_DIR   := ./src
BUILD_DIR := ./build

frontend_gui     := $(SRC_DIR)/frontend/gui
frontend_scripts := $(SRC_DIR)/frontend/run
frontend   := $(frontend_gui) $(frontend_scripts)
globalizer := $(SRC_DIR)/globalizer/globalizer_formatter
goomf      := $(SRC_DIR)/GooMF
java       := $(SRC_DIR)/java
llvm       := $(SRC_DIR)/llvm
ttrv       := $(SRC_DIR)/TTRV
monitor    := $(SRC_DIR)/monitor
swig       := $(SRC_DIR)/swig

rithm_components := $(frontend) $(globalizer) $(goomf) $(java) $(llvm) $(ttrv) $(monitor) $(swig)

.PHONY: all
.PHONY: $(rithm_components)

all: $(rithm_components)

$(rithm_components):
	$(MAKE) --directory=$@ $(MAKE_TARGET)


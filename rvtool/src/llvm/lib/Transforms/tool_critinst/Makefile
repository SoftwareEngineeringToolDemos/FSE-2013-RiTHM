##===- lib/Transforms/tool_critinst/Makefile --------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMtool_critinst
LOADABLE_MODULE = 1
USEDLIBS =

ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/tool_critinst.exports
endif
endif

include $(LEVEL)/Makefile.common


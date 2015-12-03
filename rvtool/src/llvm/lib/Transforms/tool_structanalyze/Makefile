##===- lib/Transforms/tool_structanalyze/Makefile --------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMtool_structanalyze
LOADABLE_MODULE = 1
USEDLIBS =

ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/tool_structanalyze.exports
endif
endif

include $(LEVEL)/Makefile.common


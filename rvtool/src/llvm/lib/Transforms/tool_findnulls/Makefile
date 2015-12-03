##===- lib/Transforms/tool_findnulls/Makefile --------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMtool_findnulls
LOADABLE_MODULE = 1
USEDLIBS =

ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/tool_findnulls.exports
endif
endif

include $(LEVEL)/Makefile.common


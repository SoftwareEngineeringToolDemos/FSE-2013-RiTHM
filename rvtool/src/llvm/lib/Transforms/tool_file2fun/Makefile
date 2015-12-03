##===- lib/Transforms/tool_file2fun/Makefile --------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMtool_file2fun
LOADABLE_MODULE = 1
USEDLIBS =

ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/tool_file2fun.exports
endif
endif

include $(LEVEL)/Makefile.common


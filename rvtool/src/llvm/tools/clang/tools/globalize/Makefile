##===-------- tools/toolTemplate/Makefile ------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

CLANG_LEVEL := ../..

TOOLNAME = globalize
NO_INSTALL = 1

# No plugins, optimize startup time.
TOOL_NO_EXPORTS = 1

include $(CLANG_LEVEL)/../../Makefile.config
LINK_COMPONENTS := $(TARGETS_TO_BUILD) asmparser support mc
USEDLIBS = clangTooling.a clangFrontend.a clangSerialization.a clangDriver.a \
					 clangRewriteFrontend.a clangRewriteCore.a \
					 clangParse.a clangSema.a clangAnalysis.a \
					 clangAST.a clangASTMatchers.a clangEdit.a clangLex.a clangBasic.a

include $(CLANG_LEVEL)/Makefile

CXXFLAGS += -I../../../../../../libconfig-1.4.8/lib -fexceptions
LDFLAGS += -L../../../../../../libconfig-1.4.8/lib/.libs
LIBS += -lconfig++

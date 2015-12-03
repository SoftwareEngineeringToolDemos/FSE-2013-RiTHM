##===- lib/Transforms/Makefile -----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../..
PARALLEL_DIRS = Utils Instrumentation Scalar InstCombine IPO Vectorize Hello tool_critinst tool_file2fun tool_findlines tool_findnulls tool_rename tool_structanalyze

include $(LEVEL)/Makefile.config

# No support for plugins on windows targets
ifeq ($(HOST_OS), $(filter $(HOST_OS), Cygwin MingW Minix))
  PARALLEL_DIRS := $(filter-out Hello, $(PARALLEL_DIRS))
endif

include $(LEVEL)/Makefile.common

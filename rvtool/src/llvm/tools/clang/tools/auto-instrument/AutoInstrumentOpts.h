#ifndef AUTOINSTRUMENTOPTS_H
#define AUTOINSTRUMENTOPTS_H


#include "llvm/Support/CommandLine.h"
extern llvm::cl::opt<std::basic_string<char> > InstrumentationScheme;
extern llvm::cl::opt<std::basic_string<char> > OutputMainFunctionMeta;

#endif
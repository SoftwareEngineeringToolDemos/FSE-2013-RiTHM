#ifndef INSTRUMENTFRONTENDACTION_H
#define INSTRUMENTFRONTENDACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "InstrumentASTConsumer.h"
#include "InstrumentationMap.h"

using namespace clang;
using namespace llvm;
using namespace clang::auto_instrument;

class InstrumentFrontendAction : public ASTFrontendAction {
 public:
  InstrumentFrontendAction();
  virtual ~InstrumentFrontendAction();
  virtual ASTConsumer *CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile);
  
 private:
  Rewriter rewriter;
  InstrumentationMap *map;
};

#endif
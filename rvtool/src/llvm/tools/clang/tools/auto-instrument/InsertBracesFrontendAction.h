#ifndef INSERTBRACESFRONTENDACTION_H
#define INSERTBRACESFRONTENDACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "InstrumentationMap.h"
#include <string>
#include "AutoInstrumentOpts.h"
#include "InsertBracesASTConsumer.h"

using namespace clang;
using namespace llvm;

class InsertBracesFrontendAction : public ASTFrontendAction {
 public:
  virtual ASTConsumer *CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile);

 private:
  Rewriter rewriter;
};

class InsertGooMFHeaderFrontEndAction: public ASTFrontendAction
{
public:
	InsertGooMFHeaderFrontEndAction();
	~InsertGooMFHeaderFrontEndAction();
	virtual ASTConsumer *CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile);
private:
  Rewriter rewriter;
  clang::auto_instrument::InstrumentationMap *map;
};
#endif

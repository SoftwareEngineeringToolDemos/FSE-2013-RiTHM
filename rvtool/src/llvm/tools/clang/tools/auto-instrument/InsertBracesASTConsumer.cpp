#include "InsertBracesASTConsumer.h"

using namespace clang;

InsertBracesASTConsumer::InsertBracesASTConsumer(Rewriter &rewriter)
	: rewriter(rewriter), visitor(rewriter) {}

void InsertBracesASTConsumer::HandleTranslationUnit(ASTContext &Context) {
  visitor.TraverseDecl(Context.getTranslationUnitDecl());
  if(rewriter.overwriteChangedFiles()) {
    llvm::errs() << "WARNING: Some files may not have been overwritten with instrumentation\n";
  }
}

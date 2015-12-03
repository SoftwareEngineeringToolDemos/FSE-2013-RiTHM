#include "InstrumentASTConsumer.h"

using namespace clang;
using namespace std;

InstrumentASTConsumer::InstrumentASTConsumer(Rewriter &rewriter, InstrumentationMap &map)
	: rewriter(rewriter), visitor(rewriter, map), map(map) {}

void InstrumentASTConsumer::HandleTranslationUnit(ASTContext &Context) {
//  SourceLocation loc = Context.getSourceManager().getLocForStartOfFile(Context.getSourceManager().getMainFileID());
//  rewriter.InsertText(loc, map.getHeader() + "\n", true, true);
  llvm::errs() << "INFO: Not inserting header here\n"; 
  visitor.TraverseDecl(Context.getTranslationUnitDecl());
  if(rewriter.overwriteChangedFiles()) {
    llvm::errs() << "WARNING: Some files may not have been overwritten with instrumentation\n";
  }
}

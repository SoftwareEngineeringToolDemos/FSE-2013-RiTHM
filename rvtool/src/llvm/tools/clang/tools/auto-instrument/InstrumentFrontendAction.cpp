#include "InstrumentFrontendAction.h"
#include <string>
#include "AutoInstrumentOpts.h"

InstrumentFrontendAction::InstrumentFrontendAction() {
  map = new InstrumentationMap(InstrumentationScheme.c_str());
}

InstrumentFrontendAction::~InstrumentFrontendAction() {
  delete map;
}

ASTConsumer *InstrumentFrontendAction::CreateASTConsumer(
    CompilerInstance &Compiler, StringRef InFile) {
  
  ASTContext *ctx = &Compiler.getASTContext();
  rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
  return new InstrumentASTConsumer(rewriter, *map);
}

#include "InsertBracesFrontendAction.h"

ASTConsumer *InsertBracesFrontendAction::CreateASTConsumer(
    CompilerInstance &Compiler, StringRef InFile) {
  
  ASTContext *ctx = &Compiler.getASTContext();
  rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
  return new InsertBracesASTConsumer(rewriter);
}

class InsertGooMFHeaderASTConsumer : public ASTConsumer {
public:
  InsertGooMFHeaderASTConsumer(Rewriter &rewriter, string headertxt):
  rewriter(rewriter), headertxt(headertxt){}
  virtual void HandleTranslationUnit(ASTContext &Context){
	    SourceLocation loc = Context.getSourceManager().getLocForStartOfFile(Context.getSourceManager().getMainFileID());
	    llvm::errs() << headertxt << " header is here \n";	
	    if(rewriter.InsertText(loc, "#include \"GooMFInstrumentor.h\"\n", true, true))
	    	llvm::errs()<<"GooMF Header written successfully \n";

	    if(rewriter.InsertText(loc, "#include \"buffer.h\"\n", true, true))
	    	llvm::errs()<<"Buffer Header written successfully \n";

	    if(rewriter.overwriteChangedFiles()) {
				  llvm::errs() << "WARNING: Some files may not have inlined include statement for RiTHMic header\n";
			}
  }

private:
  Rewriter &rewriter;
  string headertxt;
};

ASTConsumer *InsertGooMFHeaderFrontEndAction::CreateASTConsumer
(CompilerInstance &Compiler, StringRef InFile){
	 ASTContext *ctx = &Compiler.getASTContext();
	  rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
	  return new InsertGooMFHeaderASTConsumer(rewriter,map->getHeader());
}

InsertGooMFHeaderFrontEndAction::InsertGooMFHeaderFrontEndAction()
{
	map = new  clang::auto_instrument::InstrumentationMap(InstrumentationScheme.c_str());
}
InsertGooMFHeaderFrontEndAction::~InsertGooMFHeaderFrontEndAction()
{
	delete map;
}

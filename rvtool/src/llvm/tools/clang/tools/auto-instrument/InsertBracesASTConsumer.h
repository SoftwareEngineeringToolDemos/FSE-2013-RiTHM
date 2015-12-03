#ifndef INSERTBRACESASTCONSUMER_H
#define INSERTBRACESASTCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "InsertBracesASTVisitor.h"

using namespace clang;

class InsertBracesASTConsumer : public ASTConsumer {
public:
	InsertBracesASTConsumer(Rewriter &rewriter);

  virtual void HandleTranslationUnit(ASTContext &Context); 

private:
  Rewriter &rewriter;
	InsertBracesASTVisitor visitor;
};

#endif /* INSERTBRACESASTCONSUMER_H */

#ifndef INSTRUMENTASTCONSUMER_H
#define INSTRUMENTASTCONSUMER_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/DeclGroup.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "InstrumentASTVisitor.h"
#include "InstrumentationMap.h"

using namespace clang;
using namespace std;
using namespace clang::auto_instrument;

class InstrumentASTConsumer : public ASTConsumer {
public:
	InstrumentASTConsumer(Rewriter &rewriter, InstrumentationMap &map);

  virtual void HandleTranslationUnit(ASTContext &Context);

private:
  Rewriter &rewriter;
  InstrumentASTVisitor visitor;
  InstrumentationMap &map;
};

#endif /* INSTRUMENTASTCONSUMER_H */

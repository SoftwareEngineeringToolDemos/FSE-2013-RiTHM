#ifndef INSERTBRACESASTVISITOR_H
#define INSERTBRACESASTVISITOR_H

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Rewrite/Core/Rewriter.h"

using namespace clang;

class InsertBracesASTVisitor : public RecursiveASTVisitor<InsertBracesASTVisitor> {
public:
	InsertBracesASTVisitor(Rewriter &R);

	bool VisitIfStmt(IfStmt *ifstmt);
	bool VisitWhileStmt(WhileStmt *whilestmt);
	bool VisitForStmt(ForStmt *forstmt);

private:
	Rewriter &rewriter;

	void InsertBracesAroundStmt(const Stmt *s);
    SourceRange getStmtRangeWithTokenEnd(
        const SourceManager& sm,
        const LangOptions& options,
        const Stmt *S,
        const tok::TokenKind tKind);
};

#endif /* INSERTBRACESASTVISITOR_H */

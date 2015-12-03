#ifndef INSTRUMENTASTVISITOR_H
#define INSTRUMENTASTVISITOR_H

#include "clang/AST/ParentMap.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include <string>
#include <map>

#include "InstrumentationMap.h"
#include "AutoInstrumentOpts.h"

using namespace clang;
using namespace std;
using namespace clang::auto_instrument;

class InstrumentASTVisitor : public RecursiveASTVisitor<InstrumentASTVisitor> {
public:
	InstrumentASTVisitor(Rewriter &R, InstrumentationMap &map);

	bool VisitDeclStmt(DeclStmt *declStmt);
	bool VisitBinaryOperator(BinaryOperator *bop);
	bool VisitUnaryOperator(UnaryOperator *uop);
	bool VisitReturnStmt(ReturnStmt *rstmt);
	bool VisitFunctionDecl(FunctionDecl *f);

private:
	Rewriter &rewriter;
	ParentMap *fnMap;
	InstrumentationMap &map;

	bool inMain;
  string currentFcn;

	static const string PROGRAM_MAIN;

	struct LocationTuple {
		unsigned line1;
		unsigned col1;
		unsigned line2;
		unsigned col2;
	};

	LocationTuple getLocTuple(Stmt *s);
	void checkAndInstrument(Stmt *stmt, Stmt *subExpr, string varName);
	void insertInstrumentation(unsigned lineNum, string varName, Stmt *s,string codelines);
	string getSourceRangeAsString(SourceRange range);
    SourceRange getStmtRangeWithTokenEnd(
        const SourceManager& sm,
        const LangOptions& options,
        const Stmt *S,
        const tok::TokenKind tKind);
    SourceRange getStmtRangeWithoutTokenEnd(
        const SourceManager& sm,
        const LangOptions& options,
        const Stmt *S,
        const tok::TokenKind tKind);
    SourceRange getStmtRange(
        const SourceManager& sm,
        const LangOptions& options,
        const Stmt *S,
        const tok::TokenKind tKindi,
        const bool includeToken);
};

#endif /* INSTRUMENTASTVISITOR_H */

#include "InsertBracesASTVisitor.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace llvm;

InsertBracesASTVisitor::InsertBracesASTVisitor(Rewriter &R)
		: rewriter(R) {}

bool InsertBracesASTVisitor::VisitIfStmt(IfStmt *ifstmt) {
  if (ifstmt->getThen() && !isa<CompoundStmt>(ifstmt->getThen())) {
		InsertBracesAroundStmt(ifstmt->getThen());
	}

	if (ifstmt->getElse() && !isa<CompoundStmt>(ifstmt->getElse())) {
		InsertBracesAroundStmt(ifstmt->getElse());
	} 
	return true;
}

bool InsertBracesASTVisitor::VisitWhileStmt(WhileStmt *whilestmt) {
	if (!isa<CompoundStmt>(whilestmt->getBody()) 
			&& !isa<NullStmt>(whilestmt->getBody())) {
		InsertBracesAroundStmt(whilestmt->getBody());
	}
	return true;
}

bool InsertBracesASTVisitor::VisitForStmt(ForStmt *forstmt) {
	if (!isa<CompoundStmt>(forstmt->getBody())
			&& !isa<NullStmt>(forstmt->getBody())) {
		InsertBracesAroundStmt(forstmt->getBody());
	}
	return true;
}

void InsertBracesASTVisitor::InsertBracesAroundStmt(const Stmt *s) {
    SourceRange range = getStmtRangeWithTokenEnd(
        rewriter.getSourceMgr(),
        rewriter.getLangOpts(),
        s,
        tok::semi
    );

	if (range.isInvalid()) {
		errs() << "WARNING: Invalid source range for brace insertion!\n";
		return;
	}

	rewriter.InsertText(range.getBegin(), "{", true, true);
	rewriter.InsertText(range.getEnd(), "}", true, true);
}

// Get the source range of the specified Stmt, ensuring that the ending token is
// included, if necessary - since the clang ranges do not guarantee this.
SourceRange InsertBracesASTVisitor::getStmtRangeWithTokenEnd(
        const SourceManager& sm,
        const LangOptions& options,
        const Stmt *S,
        const tok::TokenKind tKind)
{
    clang::SourceLocation SLoc = sm.getExpansionLoc(S->getLocStart());
    clang::SourceLocation ELoc = sm.getExpansionLoc(S->getLocEnd());

    clang::SourceLocation Loc = sm.getExpansionLoc(ELoc);
    std::pair<clang::FileID, unsigned> LocInfo = sm.getDecomposedLoc(Loc);
    llvm::StringRef Buffer = sm.getBufferData(LocInfo.first);
    const char *StrData = Buffer.data()+LocInfo.second;

    clang::Lexer TheLexer(sm.getLocForStartOfFile(LocInfo.first),
            options,
            Buffer.begin(),
            StrData,
            Buffer.end());
    clang::Token TheTok;
    while (!TheLexer.LexFromRawLexer(TheTok)) {
        if (TheTok.is(tKind)) {
            return SourceRange(
                SLoc, 
                clang::Lexer::getLocForEndOfToken(TheTok.getLocation(), 0, sm, options)
            );
        }
    }

    return SourceRange(SLoc,ELoc);
}

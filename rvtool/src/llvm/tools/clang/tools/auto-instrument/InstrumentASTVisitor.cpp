#include "InstrumentASTVisitor.h"

#include <sstream>
#include <fstream>

#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

using namespace clang;
using namespace llvm;
using namespace std;
using namespace clang::auto_instrument;

#include "AutoInstrumentOpts.h"

InstrumentASTVisitor::InstrumentASTVisitor(Rewriter &R, InstrumentationMap &map)
	: rewriter(R), fnMap(0), map(map), inMain(false), currentFcn() {}

bool InstrumentASTVisitor::VisitDeclStmt(DeclStmt *declStmt) {
	errs() << "<----- Visiting DeclStmt\n";
	unsigned i;
	DeclGroupRef DR = declStmt->getDeclGroup();
	if (fnMap && fnMap->hasParent(declStmt)) {
		Stmt *parent = fnMap->getParent(declStmt);
		errs() << "*** DeclStmt parent type: " << parent->getStmtClassName() << "\n";
	}
	for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
			b != e;
			++b) {
		errs() << "***** decl kind: " << (*b)->getDeclKindName() << "\n";
		if(isa<VarDecl>(*b)) {
			VarDecl *vd = (VarDecl *)(*b);
			errs() << "--- Found variable declaration: " << vd->getNameAsString() << "\n"
			     << "    -> Declaration Context: " << (vd->getDeclContext())->getDeclKindName() << "\n"
			     << "    -> has init: " << vd->hasInit() << "\n";

			if (vd->hasInit()) {
				bool found_flag = false;
				LocationTuple stmtLoc = getLocTuple(declStmt);
				errs() << "VarDecl with init Found on lines: " << stmtLoc.line1 << "-" << stmtLoc.line2 << "\n";
				errs() << "Var declared: " << vd->getNameAsString() << "\n";
				string decName = vd->getNameAsString();
				for (unsigned lineNum = stmtLoc.line1; lineNum <= stmtLoc.line2; ++lineNum) {
					if(map.getMap().find(lineNum) != map.getMap().end())
					{
						for(i = 0;i <(map.getMap())[lineNum]->varName.size();i++)
							if(decName.compare((map.getMap())[lineNum]->varName.at(i) ) == 0)
							{
								found_flag = true;
								break;
							}
						if(found_flag)
						{
							string codelines = (map.getMap())[lineNum]->instrumentation.at(i);
							insertInstrumentation(lineNum, vd->getNameAsString(), declStmt,codelines);
						}
					}
				}
			}

		}
	}
	return true;
} 

bool InstrumentASTVisitor::VisitBinaryOperator(BinaryOperator *bop) {
	//errs() << "<----- Visiting BinaryOperator\n";
	//errs() << "BinaryOp Type: " << bop->getOpcodeStr() << "\n";

	if (! (bop->isAssignmentOp()) ) return true;

	Expr *lhs = bop->getLHS();

	LocationTuple stmtLoc = getLocTuple(bop);
	errs()	<< "Binary Assignment Operation Found on lines: " 
					<< stmtLoc.line1 << ":" << stmtLoc.col1 << "-" << stmtLoc.line2 << ":" << stmtLoc.col2 << "\n";

	string vName;
	if (isa<ArraySubscriptExpr>(bop->getLHS())) {
		errs() << "    Array subscript expr\n";
		ArraySubscriptExpr *asExpr = (ArraySubscriptExpr *) (lhs);
		errs() << "          Base: " << getSourceRangeAsString(asExpr->getBase()->getSourceRange()) << "\n"
					 << "           Idx: " << getSourceRangeAsString(asExpr->getIdx()->getSourceRange()) << "\n";
		vName = getSourceRangeAsString(asExpr->getLHS()->getSourceRange());
	} else if (isa<MemberExpr>(lhs)) {
		errs() << "    Member expr\n";
		MemberExpr *mExpr = (MemberExpr *) (lhs );
		DeclarationNameInfo dNameInfo =  mExpr->getMemberNameInfo() ;
		errs() << "          Base: " << getSourceRangeAsString(mExpr->getBase()->getSourceRange()) << "\n"
					 << "        Member: " << dNameInfo.getAsString() << "\n";

		vName = getSourceRangeAsString(mExpr->getBase()->getSourceRange());

		if (isa<ArraySubscriptExpr>(mExpr->getBase())) {
			ArraySubscriptExpr * asExpr = (ArraySubscriptExpr *)(mExpr->getBase());
			errs() << "    Array name: " << getSourceRangeAsString(asExpr->getBase()->getSourceRange()) << "\n"
						 << "           Idx: " << getSourceRangeAsString(asExpr->getIdx()->getSourceRange()) << "\n";
			vName = getSourceRangeAsString(asExpr->getLHS()->getSourceRange());
		}
	} else if(isa<DeclRefExpr>(lhs)) {
		errs() << "       decl name: " << ( (DeclRefExpr *) lhs )->getFoundDecl()->getNameAsString() << "\n";
		vName = ( (DeclRefExpr *) (lhs) )->getFoundDecl()->getNameAsString();
	}


	errs() << "    LHS source: " << getSourceRangeAsString(lhs->getSourceRange()) << "\n";
	
	checkAndInstrument(bop, bop->getLHS(), vName);

	return true;
}

bool InstrumentASTVisitor::VisitUnaryOperator(UnaryOperator *uop) {
	errs() << "<----- Visiting UnaryOperator\n";
	errs() << "UnaryOp Type: " << UnaryOperator::getOpcodeStr(uop->getOpcode()) << "\n";
	errs() << "UnaryOp SubExpr Type: " << uop->getSubExpr()->getStmtClassName() << "\n";
	errs() << "UnaryOp SubExpr: " << getSourceRangeAsString(uop->getSubExpr()->getSourceRange()) << "\n";

	string vName;
	Expr *subExpr = uop->getSubExpr();
	if (isa<DeclRefExpr>(subExpr)) {
		vName = ((DeclRefExpr *)subExpr)->getFoundDecl()->getNameAsString();
	} else if (isa<ArraySubscriptExpr>(subExpr)) {
		vName = getSourceRangeAsString(((ArraySubscriptExpr *)subExpr)->getLHS()->getSourceRange());
	} else if (isa<MemberExpr>(subExpr)) {
		MemberExpr *mExpr = (MemberExpr *)subExpr;
		vName = getSourceRangeAsString(mExpr->getBase()->getSourceRange());

		if (isa<ArraySubscriptExpr>(mExpr->getBase())) {
			ArraySubscriptExpr * asExpr = (ArraySubscriptExpr *)(mExpr->getBase());
			vName = getSourceRangeAsString(asExpr->getLHS()->getSourceRange());
		}
	}

	checkAndInstrument(uop, uop->getSubExpr(), vName);

	return true;
}

bool InstrumentASTVisitor::VisitReturnStmt(ReturnStmt *rstmt) {
	if (inMain) {
		rewriter.InsertText(rstmt->getLocStart(),map.getEnd(),true,true);
	}
	return true;
}

bool InstrumentASTVisitor::VisitFunctionDecl(FunctionDecl *f) {
	inMain = false;
  currentFcn = f->getNameInfo().getName().getAsString();
    errs() << "Currently visiting the function: " << currentFcn << "\n";

	if(f->hasBody()) {
		Stmt *FuncBody = f->getBody();
		if (fnMap) {
			free(fnMap);
		}
		fnMap = new ParentMap(FuncBody);
	}

	if(f->isMain()) {

		if(f->hasBody()) {
			inMain = true;
		}

		// Generate main function meta data and output to file
		ofstream outputMainMeta(OutputMainFunctionMeta.c_str());
		if (outputMainMeta.is_open()) {
			switch(f->getNumParams()) {
				case 0:
					outputMainMeta << "args,none\n";
					break;
				case 2:
					outputMainMeta << "args,std\n";
					break;
				default:
					break;
			}
			outputMainMeta << "return," << f->getResultType().getAsString() << "\n";
			outputMainMeta.close();
		}

		// Rename main function
		rewriter.ReplaceText(f->getNameInfo().getSourceRange(), PROGRAM_MAIN);
	}

	return true;
}

InstrumentASTVisitor::LocationTuple InstrumentASTVisitor::getLocTuple(Stmt *s) {
	LocationTuple tuple = {
		rewriter.getSourceMgr().getExpansionLineNumber(s->getLocStart()),
		rewriter.getSourceMgr().getExpansionColumnNumber(s->getLocStart()),
		rewriter.getSourceMgr().getExpansionLineNumber(s->getLocEnd()),
		rewriter.getSourceMgr().getExpansionColumnNumber(s->getLocEnd())
	};
	return tuple;
}

void InstrumentASTVisitor::checkAndInstrument(Stmt *stmt, Stmt *subExpr, string varName) {
	LocationTuple stmtLoc = getLocTuple(stmt);
	map.printMap();
	bool found_flag = false;
	unsigned i;
	for (unsigned lineNum = stmtLoc.line1; lineNum <= stmtLoc.line2; ++lineNum)
	{
		if(map.getMap().find(lineNum) != map.getMap().end())
		{
			for(i = 0;i <(map.getMap())[lineNum]->varName.size();i++)
				if(varName.compare((map.getMap())[lineNum]->varName.at(i) ) == 0)
				{
					found_flag = true;
					break;
				}
			if(found_flag)
			{
				ArraySubscriptExpr *asExpr =( (ArraySubscriptExpr *) subExpr );
				if (isa<ArraySubscriptExpr>(subExpr)) {
					varName += "[";
					varName += getSourceRangeAsString(asExpr->getRHS()->getSourceRange());
					varName += "]";
				}
				string codelines = (map.getMap())[lineNum]->instrumentation.at(i);
				insertInstrumentation(lineNum, varName, stmt,codelines);
				break;
			}
		}
	}
}

void InstrumentASTVisitor::insertInstrumentation(unsigned lineNum, string varName, Stmt *s,string inst) {
	if (map.getMap().find(lineNum) == map.getMap().end()) {
		errs() << "WARNING: Could not instrument [** " << varName << " **] at line: " << lineNum << "\n";
		return;
	}
//	string inst = (map.getMap())[lineNum]->instrumentation;

	LocationTuple stmtLoc = getLocTuple(s);

	errs()	<< "Instrumenting [** " << varName << " **] after " 
					<< stmtLoc.line1 << ":" << stmtLoc.col1 << "-" << stmtLoc.line2 << ":" << stmtLoc.col2 << "\n";

	Stmt *parent, *child;
	for (child = s, parent = fnMap->hasParent(s) ? fnMap->getParent(s) : NULL;
			parent != NULL;
			child = parent, parent = fnMap->hasParent(parent) ? fnMap->getParent(parent) : NULL) {

		if (isa<ForStmt>(parent)) {
			ForStmt *pStmt = (ForStmt *)parent;
			if (pStmt->getInit() == child) {
                errs() << "INFO: for stmt init\n";
                SourceRange range = getStmtRangeWithoutTokenEnd(
                    rewriter.getSourceMgr(),
                    rewriter.getLangOpts(),
                    child,
                    tok::semi
                );

                if (range.getEnd().isValid()) {
                    rewriter.InsertText(range.getEnd(), ", " + inst.substr(0,inst.find_last_of(";")), true, true);
                }
			} else if (pStmt->getInc() == child) {
				errs() << "INFO: for stmt increment\n";
                SourceRange range = getStmtRangeWithoutTokenEnd(
                    rewriter.getSourceMgr(),
                    rewriter.getLangOpts(),
                    pStmt,
                    tok::r_brace
                );

                if (range.getEnd().isValid()) {
                    rewriter.InsertText(range.getEnd(), inst, true, true);
                }
			} else if (pStmt->getCond() == child) {
                errs() << "INFO: for stmt condition\n";
            }
            return;
		} else if (isa<IfStmt>(parent)) {
			IfStmt *pStmt = (IfStmt *)parent;
			if (pStmt->getCond() == child) {
				if ( pStmt->getThen() && isa<CompoundStmt>(pStmt->getThen()) ) {
                    errs() << "INFO: if stmt condition -- if clause\n";
					CompoundStmt *cStmt = (CompoundStmt *) (pStmt->getThen());
					SourceLocation eLoc = ( *(cStmt->body_begin()) )->getLocStart();
					if (eLoc.isValid()) {
						rewriter.InsertText(eLoc, inst, true, true);
					}
				}

				if ( pStmt->getElse() && isa<CompoundStmt>(pStmt->getElse()) ) {
                    errs() << "INFO: if stmt condition -- else clause\n";
					CompoundStmt *cStmt = (CompoundStmt *) (pStmt->getElse());
					SourceLocation eLoc = ( *(cStmt->body_begin()) )->getLocStart();
					if (eLoc.isValid()) {
						rewriter.InsertText(eLoc, inst, true, true);
					}
				}
				return;
			}
		} else if (isa<WhileStmt>(parent)) {
			WhileStmt *pStmt = (WhileStmt *)parent;
			if (pStmt->getCond() == child) {
                errs() << "INFO: while stmt condition\n";
				SourceLocation eLoc = pStmt->getBody()->getLocStart();
				if (isa<CompoundStmt>(pStmt->getBody())) {
					CompoundStmt *cStmt = (CompoundStmt *) (pStmt->getBody());
					eLoc = ( *(cStmt->body_begin()) )->getLocStart();
				}
				if (eLoc.isValid()) {
					rewriter.InsertText(eLoc, inst, true, true);
				}

                SourceRange range = getStmtRangeWithTokenEnd(
                    rewriter.getSourceMgr(),
                    rewriter.getLangOpts(),
                    pStmt,
                    tok::r_brace
                );
	            
                if (range.getEnd().isValid()) {
		            rewriter.InsertText(range.getEnd(), inst, true, true);
	            }

				return;
			}
		} else if (isa<DoStmt>(parent)) {
			DoStmt *pStmt = (DoStmt *)parent;
			if (pStmt->getCond() == child) {
				errs() << "INFO: do-while stmt condition\n";
                CompoundStmt *cStmt = (CompoundStmt *) (pStmt->getBody());
				SourceLocation eLoc = ( *(cStmt->body_begin()) )->getLocStart();
				if (eLoc.isValid()) {
					rewriter.InsertText(eLoc, inst, true, true);
				}

                SourceRange range = getStmtRangeWithTokenEnd(
                    rewriter.getSourceMgr(),
                    rewriter.getLangOpts(),
                    pStmt,
                    tok::semi
                );

	            if (range.getEnd().isValid()) {
		            rewriter.InsertText(range.getEnd(), inst, true, true);
	            }
                
				return;
			}
	    } else if (isa<CompoundStmt>(parent)) {
            errs() << "INFO: normal case\n";
            SourceRange range = getStmtRangeWithTokenEnd(
                rewriter.getSourceMgr(),
                rewriter.getLangOpts(),
                s,
                tok::semi
            );

	        if (range.getEnd().isValid()) {
		        rewriter.InsertText(range.getEnd(), inst, true, true);
	        }
            return;
        }
	}
	
	errs()	<< "WARNING: Could not instrument [** " << varName << " **] after " 
					<< stmtLoc.line1 << ":" << stmtLoc.col1 << "-" << stmtLoc.line2 << ":" << stmtLoc.col2 << "\n";
}

string InstrumentASTVisitor::getSourceRangeAsString(SourceRange range) {
	SourceManager &sourceManager = rewriter.getSourceMgr();
	if (range.getBegin().isMacroID() || range.getEnd().isMacroID()) {
		// handle this case
		return string();
	} else if (!sourceManager.isFromSameFile(range.getBegin(), range.getEnd())) {
		// handle this case
		return string();
	} else {
		range.setEnd(Lexer::getLocForEndOfToken(range.getEnd(),0,sourceManager, rewriter.getLangOpts()));
		const char *begin = sourceManager.getCharacterData(range.getBegin());
		const char *end   = sourceManager.getCharacterData(range.getEnd());
		return string(begin, end - begin);
  	}
}

// Get the source range of the specified Stmt, ensuring that the ending token is
// included, if necessary - since the clang ranges do not guarantee this.
SourceRange InstrumentASTVisitor::getStmtRangeWithTokenEnd(
        const clang::SourceManager& sm,
        const clang::LangOptions& options,
        const clang::Stmt *S,
        const tok::TokenKind tKind)
{
    return getStmtRange(sm, options, S, tKind, true);
}

SourceRange InstrumentASTVisitor::getStmtRangeWithoutTokenEnd(
        const clang::SourceManager& sm,
        const clang::LangOptions& options,
        const clang::Stmt *S,
        const tok::TokenKind tKind)
{
    return getStmtRange(sm, options, S, tKind, false);
}

SourceRange InstrumentASTVisitor::getStmtRange(
        const clang::SourceManager& sm,
        const clang::LangOptions& options,
        const clang::Stmt *S,
        const tok::TokenKind tKind,
        const bool includeToken)
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
            if (includeToken) {
                return SourceRange(SLoc, clang::Lexer::getLocForEndOfToken(TheTok.getLocation(), 0, sm, options));
            } else {
                return SourceRange(SLoc, TheTok.getLocation());
            }
        }
    }

    return SourceRange(SLoc,ELoc);
}

const string InstrumentASTVisitor::PROGRAM_MAIN = "program_main"; 

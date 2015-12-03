#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Lex/Lexer.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "clang/AST/Decl.h"
//#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ParentMap.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include <string>
#include <map>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>

#include "libconfig.h++"

using namespace libconfig;

using namespace std;

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;
using namespace llvm;

namespace clang{
namespace globalize{
string source_file_name;
class Commentchecker: public clang::CommentHandler
{
public:

	Commentchecker(Rewriter &r) : rewriter(r) {}
	virtual bool HandleComment (Preprocessor &PP, SourceRange Comment)
	{
		clang::SourceManager &sm = PP.getSourceManager();
		clang::FileID fid = sm.getFileID(Comment.getBegin());
//		CharSourceRange S= CharSourceRange()
		StringRef S = Lexer::getSourceText(CharSourceRange(SourceRange(Comment.getBegin(),Comment.getEnd()),true),sm,PP.getLangOpts());

		if(S.str().compare("/*RITHM-monitor*/") == 0)
		{
//  		if(found != std::string::npos)
			if(rewriter.isRewritable(Comment.getEnd()))
			{
				llvm::errs()<<S<<"\n";
				;

				string line("\n"),code("");
				StringRef coderef;
				std::ifstream monitorfunc("/home/yogi/rvtool/test/example/bs66/src/MonitorCode.txt");
				while (getline(monitorfunc,line))
				{
					llvm::errs()<<	line<<"\n";
					code = code + "\n" + line;
				}
				monitorfunc.close();
				coderef = code;
				llvm::errs()<<code <<"this is code \n";
				rewriter.InsertText(Comment.getEnd(),code,true,true);
				if(rewriter.overwriteChangedFiles()) {
					llvm::errs()<<"error in rewrite \n";
				}
			}
		}
  		char x;
//  		scanf("%c",&x);
		return false;
	}
private:
	Rewriter &rewriter;
}; 
/* Added by Yogi Joshi
 * validates redeclarations of the same data item in a particular function
 * returns true if no redeclarations of a data item in the same function
 */
int error_count = 0, warning_count = 0;
bool validate_redeclarations(const VarDecl *V)
{
	VarDecl *OldV = NULL, *xx=NULL;
	int count = 0;
	OldV = (VarDecl*)V;
	FunctionDecl *fDecl = dyn_cast<FunctionDecl>(OldV->getDeclContext());
	if(fDecl)
	{
		for(DeclContext::decl_iterator iter = fDecl->decls_begin();iter != fDecl->decls_end();iter++ )
		{
			VarDecl *var = dyn_cast<VarDecl>(*iter);
			if(var)
				if(V->getNameAsString() == var->getNameAsString())
					count++;
		}
	}
	if (count > 1)
	{
		llvm::errs() << "\tWARNING: The variable " << V->getNameAsString() << " declared multiple times"
				<< " in " << fDecl->getNameInfo().getAsString() << "\n";
		llvm::errs() << "\tWARNING: It will not be globalized unless the line number of definition "
				<< "is specified in the configuration file !!\n";
		warning_count++;
		return false;
	}
	return true;
}
/* Added by Yogi Joshi
 * Display WARNING if a variable of the same name is not defined at a line defied in the config
 * file.
 * In the end of the run, if a local critical variable in the config file remains unprocessed & the
 * corresponding function is there in the sourcefile being processed then a ERROR is displayed
 * This does not apply to global variables because they could have been declared in any source
 * file
 */
void display_error(const VarDecl *V, const FunctionDecl *f, string linenum)
{
	llvm::errs() << "\tWARNING: The variable " <<  V->getNameAsString()
												 << " is defined at " << linenum << " in "
												 << f->getNameInfo().getAsString()
												 << "\n"
												 << "\tConfig file may require correction \n";
	warning_count++;
}
struct CriticalVariableEntry {
	string name;
	string type;
	string scope;
	string function;
	string linenum;
	bool processed;
	CriticalVariableEntry() : name(), type(), scope(), function(), linenum(),processed(false) {}
	CriticalVariableEntry(CriticalVariableEntry &entry)
	: name(entry.name), type(entry.type), scope(entry.scope), function(entry.scope), linenum(entry.linenum) {
		processed = false;
	}
	CriticalVariableEntry(string name, string type, string scope)
	: name(name), type(type), scope(scope), function() {
		processed = false;function="";linenum="";
	}
	CriticalVariableEntry(string name, string type, string scope, string function)
	: name(name), type(type), scope(scope), function(function) {
		processed = false;linenum="";
	}
	CriticalVariableEntry(string name, string type, string scope, string function, string linenum)
	: name(name), type(type), scope(scope), function(function), linenum(linenum) {
		processed  = false;
	}
	void markas_processed() { processed = true; }
	bool get_processed(){return this->processed;}
	string get_name(){return this->name;}
	void dump()
	{
		llvm::errs() << "Variable : name -> \'" << name << "\'"
				<< " type -> \'" << type << "\'"
				<< " scope -> \'" << scope << "\'"
				<< " function -> \'" << function << "\'"
				<< " linenumber -> \'" << linenum << "\'"
				<< " processed -> \'" << processed << "\'"
				<<"\n ";
	}
};

CriticalVariableEntry* findCriticalVariable(
		list<CriticalVariableEntry*> &criticalVariables,
		const string &name,
		const string &scope,
		const string &function = "",
		const string &linenum  = "")
{
	for(list<CriticalVariableEntry*>::iterator iter = criticalVariables.begin();
			iter != criticalVariables.end();
			iter++) {
		CriticalVariableEntry *entry = *iter;
		if(name.compare(entry->name) == 0
				&& scope.compare(entry->scope) == 0
				&& function.compare(entry->function) == 0
				&& linenum.compare(entry->linenum) == 0 ) {
			return entry;
		}
	}
	return NULL;
}

list<CriticalVariableEntry *> criticalVariables;
StatementMatcher DeclRefExprMatcher = 
		declRefExpr().bind("declRefExpr");

StatementMatcher DeclStmtMatcher =
		declStmt().bind("declStmt");

DeclarationMatcher fDeclMatcher = decl().bind("FunctionDecl");

/// \brief Obtain the original source code text from a SourceRange.
StringRef getStringFromRange(SourceManager &SourceMgr,
		const LangOptions &LangOpts,
		SourceRange Range) {
	if (SourceMgr.getFileID(Range.getBegin()) !=
			SourceMgr.getFileID(Range.getEnd())) {
		llvm::errs() << "ERROR: Range spans across multiple files\n";
		error_count++;
		return NULL;
	}

	CharSourceRange SourceChars(Range, true);
	return Lexer::getSourceText(SourceChars, SourceMgr, LangOpts);
}

// Get the source range of the specified Stmt, ensuring that the ending token is
// included, if necessary - since the clang ranges do not guarantee this.
SourceRange getStmtRangeWithTokenEnd(
		const clang::SourceManager& sm,
		const clang::LangOptions& options,
		const clang::Stmt *S,
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
		if (TheTok.is(tKind)) return SourceRange(SLoc, TheTok.getLocation());
	}

	return SourceRange(SLoc,ELoc);
}

SourceRange getDeclRangeWithTokenEnd(
		const clang::SourceManager& sm,
		const clang::LangOptions& options,
		const clang::Decl *S,
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
		if (TheTok.is(tKind)) return SourceRange(SLoc, TheTok.getLocation());
	}

	return SourceRange(SLoc,ELoc);
}

SourceRange getExpandedSourceRange(const clang::SourceManager& sm, SourceRange srcRange) {
	clang::SourceLocation SLoc = sm.getSpellingLoc(srcRange.getBegin());
	clang::SourceLocation ELoc = sm.getSpellingLoc(srcRange.getEnd());
	return SourceRange(SLoc,ELoc);
}

class DeclRefExprGlobalizeCallback : public MatchFinder::MatchCallback {
public:
	DeclRefExprGlobalizeCallback(Replacements *Replace, list<CriticalVariableEntry*> &criticalVariables)
	: Replace(Replace), criticalVariables(criticalVariables) {}

	virtual void run(const MatchFinder::MatchResult &Result) {
		ASTContext *Context = Result.Context;

		if (const DeclRefExpr *drExpr = Result.Nodes.getStmtAs<DeclRefExpr>("declRefExpr")) {
			DeclarationNameInfo nameInfo = drExpr->getNameInfo();
			llvm::errs() << "Found Decl Ref Expr: " << nameInfo.getAsString() << "\n";

//			llvm::errs() << "\tEntire Statement: " << getStringFromRange(
//					Context->getSourceManager(),
//					Context->getLangOpts(),
//					getExpandedSourceRange(Context->getSourceManager(),drExpr->getSourceRange())) << "\n";
//			llvm::errs()<< getStringFromRange(
//								Context->getSourceManager(),
//								Context->getLangOpts(),
//								getStmtRangeWithTokenEnd(
//										Context->getSourceManager(),
//										Context->getLangOpts(),
//										drExpr,
//										clang::tok::semi))<<"\n";

			const ValueDecl *valueDecl = drExpr->getDecl();
			if (isa<VarDecl>(valueDecl)) {
//				valueDecl->print(llvm::errs());
				llvm::errs()<<"\n";
//				SourceLocation end = clang::Lexer::getLocForEndOfToken(drExpr->getLocEnd(),0,Context->getSourceManager(),Context->getLangOpts());
//				llvm::errs()<< std::string( Context->getSourceManager().getCharacterData(drExpr->getLocStart()),
//											Context->getSourceManager().getCharacterData(end)-Context->getSourceManager().getCharacterData(drExpr->getLocStart()));

				llvm::errs()<<"\n";
				const VarDecl *varDecl = dyn_cast<VarDecl>(valueDecl);
				const DeclContext *declCtx = varDecl->getDeclContext();

				char result[10];
				unsigned int line = Context->getSourceManager().getSpellingLineNumber(varDecl->getLocation())-1;
				sprintf((char*)result,"%d",line);
				string *var_line =  new string(result);
				llvm::errs() << "\tDefined at Line "
						<< *var_line<<"\n";
				CriticalVariableEntry *variable;

				bool mul_decl_validation = validate_redeclarations(varDecl);

				if (declCtx->isFunctionOrMethod())
				{
//					bool pp_set_flag = 0;
					const FunctionDecl *fDecl = dyn_cast<FunctionDecl>(declCtx);
					llvm::errs() << "\tVariable declared in function: " << fDecl->getNameInfo().getAsString() << "\n";
//					for (clang::DeclContext::decl_iterator DI = Context->getTranslationUnitDecl()->decls_begin(),
//							DE = Context->getTranslationUnitDecl()->decls_end();
//							DI != DE;
//							DI++) {
//						if(FunctionDecl *fdec = dyn_cast<clang::FunctionDecl>(*DI))
//							if(fdec->getNameAsString().compare(fDecl->getNameInfo().getAsString()))
//								if((*DI)->hasBody())
//								{
//									ParentMap PP = ParentMap((*DI)->getBody());
//									llvm::errs()<<"set parentmap successfully\n";
//									pp_set_flag = true;
////									if(S)
////									{
//										llvm::errs()<<"success \n";
//										llvm::errs()<< getStringFromRange(
//											Context->getSourceManager(),
//											Context->getLangOpts(),
//											getExpandedSourceRange(Context->getSourceManager(),drExpr->getSourceRange())) << "\n";
////									}
//									break;
//								}
//					}

					if(mul_decl_validation)
					{
						variable = findCriticalVariable(criticalVariables,
								varDecl->getNameAsString(),
								"local",
								fDecl->getNameInfo().getAsString());
					}
					else
					{
						variable = findCriticalVariable(criticalVariables,
								varDecl->getNameAsString(),
								"local",
								fDecl->getNameInfo().getAsString(),
								*var_line);
						if(!variable)
							display_error(varDecl,fDecl,*var_line);
					}

					if(variable)
					{
						llvm::errs() << "\tis a critical variable!\n";
						string replaceText;
						if(!mul_decl_validation)
							replaceText = fDecl->getNameInfo().getAsString() + "_" +  *var_line + "_" +
							varDecl->getNameAsString();
						else
							replaceText = fDecl->getNameInfo().getAsString() + "_" + varDecl->getNameAsString();

						variable->markas_processed();
						Replace->insert(
								Replacement(Context->getSourceManager(),
										CharSourceRange::getTokenRange(
										getExpandedSourceRange(Context->getSourceManager(),drExpr->getSourceRange())),
												replaceText )
						);
					}
//					drExpr->setDecl(varDecl->setDeclName(DeclarationName()))


				}
				else
				{
					llvm::errs() << "\tVariable defined elsewhere\n";
					variable = findCriticalVariable(criticalVariables,
							varDecl->getNameAsString(),
							"global");
					if(variable)
						variable->markas_processed();
				}
				delete var_line;
				llvm::errs() << "\tVariable name: " << varDecl->getNameAsString() << "\n";
				llvm::errs() << "\tVariable type: " << varDecl->getType().getAsString() << "\n";
				if(isa<ArrayType>(varDecl->getType().getTypePtr())) {
					llvm::errs() << "\tis an array\n";
				} else if(isa<PointerType>(varDecl->getType().getTypePtr())) {
					llvm::errs() << "\tis a pointer\n";
				}

				if(varDecl->isStaticLocal()) {
					llvm::errs() << "\tis a Local Static Variable\n";
				} else if (varDecl->isLocalVarDecl()) {
					llvm::errs() << "\tis a Local Non-static Variable\n";
				} else if (varDecl->isFileVarDecl()) {
					llvm::errs() << "\tis a File-scoped Variable\n";
					if (varDecl->isThisDeclarationADefinition()) {
						llvm::errs() << "\tis a Definition\n";
					} else {
						llvm::errs() << "\tis only a Declaration\n";
					}
				}
			}
		}
	}

private:
	Replacements *Replace;
	list<CriticalVariableEntry*> &criticalVariables;
};

class DeclStmtGlobalizeCallback : public MatchFinder::MatchCallback {
public:
	DeclStmtGlobalizeCallback(Replacements *Replace, list<CriticalVariableEntry*> &criticalVariables)
	: Replace(Replace), criticalVariables(criticalVariables) {}

	virtual void run(const MatchFinder::MatchResult &Result) {
		ASTContext *Context = Result.Context;
		if (const DeclStmt *declStmt = Result.Nodes.getStmtAs<DeclStmt>("declStmt")) {
//			llvm::errs() << "Found declaration statement: "
//					<< "\tEntire Statement: " << getStringFromRange(
//							Context->getSourceManager(),
//							Context->getLangOpts(),
//							getExpandedSourceRange(
//									Context->getSourceManager(),
//									declStmt->getSourceRange())
//					)
//					<< "\n";

			// Declaration statement only contains one declaration
			if(declStmt->isSingleDecl()) {
				declStmt->printPretty(llvm::errs(),NULL,PrintingPolicy(Context->getLangOpts()));
				llvm::errs() << "\tDeclStmt has single declaration\n";
				if(isa<VarDecl>(declStmt->getSingleDecl())) {
					const VarDecl *varDecl = dyn_cast<VarDecl>(declStmt->getSingleDecl());
					llvm::errs() << "\t\tVariable defined: " << varDecl->getNameAsString() << "\n";
					const DeclContext *declCtx = varDecl->getDeclContext();

					char result[10];
					unsigned int line = Context->getSourceManager().getSpellingLineNumber(varDecl->getLocation())-1;
					sprintf((char*)result,"%d",line);
					string *var_line =  new string(result);
					llvm::errs() << "\tDefined at Line "
							<< *var_line <<"\n";
					CriticalVariableEntry *variable = NULL;
					bool mul_decl_validation = validate_redeclarations(varDecl);
					if (declCtx->isFunctionOrMethod())
					{
						const FunctionDecl *fDecl = dyn_cast<FunctionDecl>(declCtx);
						llvm::errs() << "\t\tVariable defined in function: " << fDecl->getNameInfo().getAsString() << "\n";

						if(mul_decl_validation)
						{
							variable = findCriticalVariable(criticalVariables,
									varDecl->getNameAsString(),
									"local",
									fDecl->getNameInfo().getAsString());
						}
						else
						{
							variable = findCriticalVariable(criticalVariables,
									varDecl->getNameAsString(),
									"local",
									fDecl->getNameInfo().getAsString(),
									*var_line);
							if(!variable)
								display_error(varDecl,fDecl,*var_line);
						}

						if(variable)
						{
							llvm::errs() << "\tis a critical variable!\n";
							string replaceText;
							variable->markas_processed();
							if (varDecl->hasInit()) {
								if(!mul_decl_validation)
									replaceText = (fDecl->getNameInfo().getAsString() + "_" + *var_line + "_"
											+ varDecl->getNameAsString()
											+ " = " + getStringFromRange(
													Context->getSourceManager(),
													Context->getLangOpts(),
													varDecl->getInit()->getSourceRange())
													+ ";").str();
								else
									replaceText = (fDecl->getNameInfo().getAsString() + "_" + varDecl->getNameAsString()
											+ " = " + getStringFromRange(
													Context->getSourceManager(),
													Context->getLangOpts(),
													varDecl->getInit()->getSourceRange())
													+ ";").str();


							} else {
								replaceText = "";
							}

							Replace->insert(
									Replacement(Context->getSourceManager(),
											CharSourceRange::getTokenRange(
													getStmtRangeWithTokenEnd(
															Context->getSourceManager(),
															Context->getLangOpts(),
															declStmt,
															clang::tok::semi)),
															replaceText));
						}
					} else {
						// Variable not defined in a function
						variable = findCriticalVariable(criticalVariables,
								varDecl->getNameAsString(),
								"global");
						if(variable)
							variable->markas_processed();
					}
					delete var_line;
				}
			} else
			{    // Declaration statement contains multiple declarations
				llvm::errs() << "\tDeclStmt has multiple declarations\n";
				DeclGroupRef declGroupRef = declStmt->getDeclGroup();
				declStmt->printPretty(llvm::errs(),NULL,PrintingPolicy(Context->getLangOpts()));
				if (isa<VarDecl>( *(declGroupRef.begin()) )) {
					VarDecl *firstVarDecl = dyn_cast<VarDecl>(*(declGroupRef.begin()));

					string noncriticalVars = "";
					string criticalVarsWithInit = "";
					VarDecl *prevVar = 0;
					for (DeclGroupRef::iterator iter = declGroupRef.begin();
							iter != declGroupRef.end();
							iter++) {
						if (isa<VarDecl>(*iter))
						{
							VarDecl *varDecl = dyn_cast<VarDecl>(*iter);
							llvm::errs() << "\t\tVariable defined: " << varDecl->getNameAsString() << "\n";
							varDecl->print(llvm::errs());
							const DeclContext *declCtx = varDecl->getDeclContext();

							char result[10];
							unsigned int line = Context->getSourceManager().getSpellingLineNumber(varDecl->getLocation())-1;
							sprintf((char*)result,"%d",line);
							string *var_line =  new string(result);
							llvm::errs() << "\tDefined at Line "
									<< *var_line<<"\n";
							CriticalVariableEntry *variable = NULL;

							bool mul_decl_validation = validate_redeclarations(varDecl);
							if (declCtx->isFunctionOrMethod())
							{
								const FunctionDecl *fDecl = dyn_cast<FunctionDecl>(declCtx);
								llvm::errs() << "\t\tVariable defined in function: " << fDecl->getNameInfo().getAsString() << "\n";
								if(mul_decl_validation)
								{
									variable = findCriticalVariable(criticalVariables,
											varDecl->getNameAsString(),
											"local",
											fDecl->getNameInfo().getAsString());
								}
								else
								{
									variable = findCriticalVariable(criticalVariables,
											varDecl->getNameAsString(),
											"local",
											fDecl->getNameInfo().getAsString(),
											*var_line);
									if(!variable)
										display_error(varDecl,fDecl,*var_line);
								}
								if(variable)
								{
									variable->markas_processed();
									llvm::errs() << "\t\tis a critical variable!\n";
									if(varDecl->hasInit())
									{
										if(!mul_decl_validation)
											criticalVarsWithInit += (fDecl->getNameInfo().getAsString() + "_" +  *var_line + "_"
													+ varDecl->getNameAsString()
													+ " = " + getStringFromRange(
															Context->getSourceManager(),
															Context->getLangOpts(),
															varDecl->getInit()->getSourceRange())
															+ ";\n").str();
										else
											criticalVarsWithInit += (fDecl->getNameInfo().getAsString() + "_"
													+ varDecl->getNameAsString()
													+ " = " + getStringFromRange(
															Context->getSourceManager(),
															Context->getLangOpts(),
															varDecl->getInit()->getSourceRange())
															+ ";\n").str();
									}
								}
								else
								{
									llvm::errs() << "\t\tis not a critical variable !\n";
									if(!prevVar)
									{
										noncriticalVars += getStringFromRange(
												Context->getSourceManager(),
												Context->getLangOpts(),
												varDecl->getSourceRange());
									}
									else
									{
										SourceRange s;
										if(prevVar->hasInit())
											s = SourceRange(prevVar->getLocEnd().getLocWithOffset(1), varDecl->getLocEnd());
										else
											s = SourceRange(prevVar->getLocEnd().
													getLocWithOffset(prevVar->getName().str().length()
													), varDecl->getLocEnd());
										noncriticalVars += getStringFromRange(
												Context->getSourceManager(),
												Context->getLangOpts(),
												s);
									}
//									noncriticalVars += varDecl->getNameAsString();
//									if(varDecl->hasInit()) {
//										noncriticalVars +=  (" = " + getStringFromRange(
//												Context->getSourceManager(),
//												Context->getLangOpts(),
//												varDecl->getInit()->getSourceRange())
//										).str();
//									}
//									noncriticalVars += ", ";
								}
								delete var_line;

							}
							else
							{
								variable = findCriticalVariable(criticalVariables,
										varDecl->getNameAsString(),
										"global");
								if(variable)
									variable->markas_processed();
							}
						}
						prevVar = dyn_cast<VarDecl>(*iter);
					}

					// Trim and format list of non-critical variables (if applicable)
//					if (noncriticalVars.length() > 0) {
//						noncriticalVars = firstVarDecl->getType().getAsString() + " "
//								+ noncriticalVars.substr(0, noncriticalVars.length() - 2)
//								+ ";\n";
//					}
					if (noncriticalVars.length() > 0)
					{
						unsigned found =  noncriticalVars.find_first_not_of(" ");
						if (found != std::string::npos)
						{
							if( noncriticalVars.at(found) == ',')
							{
								noncriticalVars[found] = ' ';
								noncriticalVars = firstVarDecl->getType().getAsString() + " "
										+ noncriticalVars.substr(0, noncriticalVars.length());
							}
						}
						if(noncriticalVars[noncriticalVars.length()-1] != ',')
							noncriticalVars+=";\n";
						else
						{
							noncriticalVars[noncriticalVars.length()-1]= ';';
							noncriticalVars+="\n";
						}

					}

					Replace->insert(
							Replacement(Context->getSourceManager(),
									CharSourceRange::getTokenRange(
											getStmtRangeWithTokenEnd(
													Context->getSourceManager(),
													Context->getLangOpts(),
													declStmt,
													clang::tok::semi)),
													noncriticalVars + criticalVarsWithInit )
					);

				}
			}
		}
	}

private:
	Replacements *Replace;
	list<CriticalVariableEntry*> &criticalVariables;
};
class InsertGlobalizeHeaderVisitor : public clang::RecursiveASTVisitor<InsertGlobalizeHeaderVisitor>
{
private:

	Rewriter &rewriter;
	bool processed;
public:
	InsertGlobalizeHeaderVisitor(Rewriter &rewriter, bool processed):rewriter(rewriter),processed(processed){}

	bool VisitFunctionDecl(FunctionDecl *f)
	{
//		llvm::errs()<<"traversing "<<f->getNameAsString()<<"\n";
		if(processed)
			return true;
		std::stringstream code;
		for (list<CriticalVariableEntry *>::iterator iter = criticalVariables.begin();
				iter != criticalVariables.end();
				iter++)
		{
			code.str()="";
			if((*iter)->name.compare("")==0)
				break;
			if (((*iter)->scope).compare("global") == 0)
			{
				if(!((*iter)->type.compare("int")==0 ||
						(*iter)->type.compare("float")==0 ||
						(*iter)->type.compare("double")==0 ||
						(*iter)->type.compare("char")==0 ||
						(*iter)->type.compare("short")==0 ||
						(*iter)->type.compare("unsigned")==0 ||
						(*iter)->type.compare("long long")==0
				))
				{
					code<< "extern " << (*iter)->type << " " << (*iter)->name << ";\n";
					std::pair< FileID, unsigned > loc = rewriter.getSourceMgr().getDecomposedLoc(f->getLocStart());
//					const FileEntry *entry = rewriter.getSourceMgr().getFileEntryForID(loc.first);
//					so
//					entry->getName()
					if(loc.first ==  rewriter.getSourceMgr().getMainFileID())
					{
						if(rewriter.isRewritable(f->getLocStart()))
						{
							llvm::errs()<<"Inserting extern here \n";
							rewriter.InsertTextBefore(f->getLocStart(),code.str());
							if(rewriter.overwriteChangedFiles())
								llvm::errs()<<code.str()<<"not written \n";
							else
								processed = true;
						}
						if(rewriter.overwriteChangedFiles()) {
							llvm::errs()<<code.str()<<"not written \n";
						}
					}
				}
			}
			else
			{
				if(!((*iter)->type.compare("int")==0 ||
						(*iter)->type.compare("float")==0 ||
						(*iter)->type.compare("double")==0 ||
						(*iter)->type.compare("char")==0 ||
						(*iter)->type.compare("short")==0 ||
						(*iter)->type.compare("unsigned")==0 ||
						(*iter)->type.compare("long long")==0
				))
				{
					if(((*iter)->linenum).compare("") == 0)
						code << "extern " << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->name << ";\n";
					else
						code << "extern " << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->linenum << "_"
						<< (*iter)->name << ";\n";
					std::pair< FileID, unsigned > loc = rewriter.getSourceMgr().getDecomposedLoc(f->getLocStart());
					if(loc.first ==  rewriter.getSourceMgr().getMainFileID())
					{
						if(rewriter.isRewritable(f->getLocStart()))
						{
							llvm::errs()<<"Inserting extern here \n";
							rewriter.InsertTextBefore(f->getLocStart(),code.str());
							if(rewriter.overwriteChangedFiles())
								llvm::errs()<<code.str()<<"not written \n";
							else
								processed = true;
						}
						if(rewriter.overwriteChangedFiles()) {
							llvm::errs()<<code.str()<<"not written \n";
						}
					}
				}
			}
		}
		return true;
	}

};
class InsertGlobalizeHeaderConsumer : public clang::ASTConsumer {
private:
	Rewriter &rewriter;
public:
	InsertGlobalizeHeaderConsumer(Rewriter &rewriter) : rewriter(rewriter)
	{
	}
	virtual void HandleTranslationUnit(clang::ASTContext &Context) {
		// Traversing the translation unit decl via a RecursiveASTVisitor
		// will visit all nodes in the AST.
		SourceLocation loc = Context.getSourceManager().getLocForStartOfFile(Context.getSourceManager().getMainFileID());

//		if(Context.getTranslationUnitDecl())
//			visitor.TraverseDecl(Context.getTranslationUnitDecl());

		rewriter.InsertText(loc, "#include \"rithmic_globals.h\"\n", true, true);
//		rewriter.InsertText(loc, "#include \"buffer.h\"\n", true, true);
		if(rewriter.overwriteChangedFiles()) {
			llvm::errs() << "WARNING: Some files may not have inlined include statement for RiTHMic header\n";
			warning_count++;
		}

	}

};
class InsertDirectExternConsumer: public clang::ASTConsumer{
private:
	Rewriter &rewriter;
	InsertGlobalizeHeaderVisitor visitor;
public:
	InsertDirectExternConsumer(Rewriter &R): rewriter(R),visitor(R,false){}
	virtual void HandleTranslationUnit(clang::ASTContext &Context) {
		// Traversing the translation unit decl via a RecursiveASTVisitor
		// will visit all nodes in the AST.
		SourceLocation loc = Context.getSourceManager().getLocForStartOfFile(Context.getSourceManager().getMainFileID());

		if(Context.getTranslationUnitDecl())
			visitor.TraverseDecl(Context.getTranslationUnitDecl());


	}

};
class InsertDirectExternAction : public clang::ASTFrontendAction {
public:
	virtual clang::ASTConsumer *CreateASTConsumer(
			clang::CompilerInstance &Compiler, llvm::StringRef InFile) {

		ASTContext *ctx = &Compiler.getASTContext();
		rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
		return new InsertDirectExternConsumer(rewriter);
	}

private:
	Rewriter rewriter;
};
class InsertGlobalizeHeaderAction : public clang::ASTFrontendAction {
public:
	virtual clang::ASTConsumer *CreateASTConsumer(
			clang::CompilerInstance &Compiler, llvm::StringRef InFile) {

		ASTContext *ctx = &Compiler.getASTContext();
		rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
//		C = new Commentchecker(rewriter);
//		Compiler.getPreprocessor().addCommentHandler(C);
		source_file_name=ctx->getSourceManager().getFilename(ctx->getSourceManager().getLocForStartOfFile(ctx->getSourceManager().getMainFileID()));
		return new InsertGlobalizeHeaderConsumer(rewriter);
	}
	~InsertGlobalizeHeaderAction()
	{
	} 
private:
	Rewriter rewriter;
};


} // end namespace globalize
} // end namespace clang

using namespace clang::globalize;

bool outputCriticalVarsList(std::string &filepath, list<CriticalVariableEntry *> &criticalVariables) {
	ofstream criticalVarsFile;
	criticalVarsFile.open(filepath.c_str());

	if (!criticalVarsFile.is_open()) return false;

	for (list<CriticalVariableEntry *>::iterator iter = criticalVariables.begin();
			iter != criticalVariables.end();
			iter++) {
		if (((*iter)->scope).compare("global") == 0) {
			criticalVarsFile << "globalFUNC," << (*iter)->name << "\n";
		} else {
			if(((*iter)->linenum).compare("") == 0)
				criticalVarsFile << "globalFUNC," << (*iter)->function << "_" << (*iter)->name << "\n";
			else
				criticalVarsFile << "globalFUNC," << (*iter)->function << "_" << (*iter)->linenum << "_" << (*iter)->name << "\n";
		}
	}

	criticalVarsFile.close();
	return true;
}

bool outputGOOMFMeta(std::string &filepath, list<CriticalVariableEntry *> &criticalVariables) {
	ofstream goomfMetaFile;
	goomfMetaFile.open(filepath.c_str());

	if (!goomfMetaFile.is_open()) return false;

	for (list<CriticalVariableEntry *>::iterator iter = criticalVariables.begin();
			iter != criticalVariables.end();
			iter++) {
		if (((*iter)->scope).compare("global") == 0) {
			goomfMetaFile << (*iter)->name << "," << (*iter)->type << ",globalFUNC" << "\n";
		} else {
			if(((*iter)->linenum).compare("") == 0)
				goomfMetaFile << (*iter)->function << "_" << (*iter)->name << "," << (*iter)->type << ",globalFUNC" << "\n";
			else
				goomfMetaFile << (*iter)->function << "_" << (*iter)->linenum << "_" <<
				(*iter)->name << "," << (*iter)->type << ",globalFUNC" << "\n";
		}
	}

	goomfMetaFile.close();
	return true;
}

bool outputGlobals(std::string &cfilepath, std::string &hfilepath, list<CriticalVariableEntry *> &criticalVariables) {
	ofstream cfile, hfile;
	cfile.open(cfilepath.c_str());
	if(!cfile.is_open()) return false;

	hfile.open(hfilepath.c_str());
	if(!hfile.is_open()) {
		cfile.close();
		return false;
	}

	// header guard
	hfile << "#ifndef RITHMIC_GLOBALS_H\n#define RITHMIC_GLOBALS_H\n";
	cfile << "# include <stdio.h>\n"<<"FILE * logFile=0;\n";
	for (list<CriticalVariableEntry *>::iterator iter = criticalVariables.begin();
			iter != criticalVariables.end();
			iter++) {
		if (((*iter)->scope).compare("global") == 0)
		{
			if(((*iter)->type.compare("int")==0 ||
				 (*iter)->type.compare("float")==0 ||
			     (*iter)->type.compare("double")==0 ||
			     (*iter)->type.compare("char")==0 ||
			     (*iter)->type.compare("short")==0 ||
			     (*iter)->type.compare("unsigned")==0||
					(*iter)->type.compare("long long")==0
				 ))
			{
				hfile << "extern " << (*iter)->type << " " << (*iter)->name << ";\n";
			}
			cfile << "extern " << (*iter)->type << " " << (*iter)->name << ";\n";
		}
		else
		{
			if(((*iter)->type.compare("int")==0 ||
							 (*iter)->type.compare("float")==0 ||
						     (*iter)->type.compare("double")==0 ||
						     (*iter)->type.compare("char")==0 ||
						     (*iter)->type.compare("short")==0 ||
						     (*iter)->type.compare("unsigned")==0||
								(*iter)->type.compare("long long")==0
							 ))
			{
				if(((*iter)->linenum).compare("") == 0)
					hfile << "extern " << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->name << ";\n";
				else
					hfile << "extern " << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->linenum << "_"
					<< (*iter)->name << ";\n";
			}
			if(((*iter)->linenum).compare("") == 0)
				cfile << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->name << ";\n";
			else
				cfile << (*iter)->type << " " << (*iter)->function << "_" << (*iter)->linenum << "_"
				<< (*iter)->name << ";\n";
		}
	}

	hfile << "#endif /* RITHMIC_GLOBALS_H */\n";

	cfile.close(); hfile.close();
	return true;
}
/*
 * Added by Yogi Joshi
 * A new frontend action to collect a list of all the function declarations in the
 * program
 * A few global variables to keep track of errors and warnings
 */

list<string> FuncList;
class FuncFinder : public MatchFinder::MatchCallback {
public :
	virtual void run(const MatchFinder::MatchResult &Result) {
		if (const FunctionDecl *FD = Result.Nodes.getNodeAs<clang::FunctionDecl>("FunctionDecl"))
			FuncList.push_back(FD->getNameInfo().getAsString());
	}
};

// Set up the command line options
cl::opt<std::string> BuildPath(
		cl::Positional,
		cl::desc("<build-path>"));

cl::opt<std::string> InputSpecFilepath(
		cl::Positional,
		cl::desc("<input-spec>"),
		cl::Required);

cl::opt<std::string> CriticalVariableListFilepath(
		cl::Positional,
		cl::desc("<critical-var-list>"),
		cl::Required);

cl::opt<std::string> GOOMFMetaFilepath(
		cl::Positional,
		cl::desc("<goomf-meta-info>"),
		cl::Required);

cl::opt<std::string> RiTHMicGlobalDefinition(
		cl::Positional,
		cl::desc("<rithmic-global-defn>"),
		cl::Required);

cl::opt<std::string> RiTHMicGlobalHeader(
		cl::Positional,
		cl::desc("<rithmic-global-header>"),
		cl::Required);

cl::opt<std::string> Bufferheader(
		cl::Positional,
		cl::desc("<buffer.h tempfile>"),
		cl::Required);

cl::list<std::string> SourcePaths(
		cl::Positional,
		cl::desc("<source0> [... <sourceN>]"),
		cl::OneOrMore);



int main(int argc, const char **argv) {
	int result;

	stringstream bufferprinter;
	std::ofstream bufferheader;


	llvm::OwningPtr<CompilationDatabase> Compilations(
			FixedCompilationDatabase::loadFromCommandLine(argc, argv));
	cl::ParseCommandLineOptions(argc, argv);

	bufferheader.open(Bufferheader.c_str(),std::ofstream::out);
	if (!Compilations) {  // Couldn't find a compilation DB from the command line
		std::string ErrorMessage;
		Compilations.reset(
				!BuildPath.empty() ?
						CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage) :
						CompilationDatabase::autoDetectFromSource(SourcePaths[0], ErrorMessage)
		);

		//  Still no compilation DB? - bail.
		if (!Compilations)
			error_count++;
			llvm::report_fatal_error(ErrorMessage);
	}
	//CommonOptionsParser OptionsParser(argc, argv);

	// Ensure that clang can parse the source files
	//ClangTool SyntaxTool(OptionsParser.GetCompilations(), OptionsParser.GetSourcePathList());
//	llvm::errs() << "========== Checking Syntax of Source File(s) =========\n";
//	ClangTool SyntaxTool(*Compilations, SourcePaths);
//	if ( (result = SyntaxTool.run(
//			newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
//		error_count++;
//		llvm::errs() << "Error parsing files\n";
//		return result;
//	}

	// Construct list of critical variables from spec
	llvm::errs() << "========== Parsing libconfig File for Critical Variables =========\n";

	Config cfg;

	try {
		cfg.readFile(InputSpecFilepath.c_str());
	} catch (const FileIOException &fioex) {
		llvm::errs() << "I/O error while reading the file: " << InputSpecFilepath << "\n";
		error_count++;
		return (EXIT_FAILURE);
	} catch (const ParseException &pex) {
		llvm::errs() << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                								 << " - " << pex.getError() << "\n";
		error_count++;
		return(EXIT_FAILURE);
	}

	Setting &root = cfg.getRoot();

	if (!root.exists("program_variables"))
	{
		llvm::errs() << "WARNING: No critical variables specified in the file: "
				<< InputSpecFilepath << "\n";
		warning_count++;
	}
	else
	{
		try {
			const Setting &variables = root["program_variables"];
			int variableCount = variables.getLength();
			string var, type, scope, function, linenum;

			for(int i = 0; i < variableCount; i++) {
				const Setting &variable = variables[i];

				if(!( variable.lookupValue("name", var)
						&& variable.lookupValue("type", type)
						&& variable.lookupValue("scope", scope)
				)) {
					llvm::errs() << "WARNING: Config file has missing data for "<< var
							<<":" << type << ":" << scope;
					warning_count++;
					continue;
				}
				//set the linenum to blank if not provided in the config
				if(!variable.lookupValue("linenum", linenum))
					linenum="";

				if (scope.compare("local") == 0
						&& !variable.lookupValue("function", function)) {
					llvm::errs() << "WARNING: Missing function scope of local variable '" << var << "'\n";
					warning_count++;
					continue;
				}

				if (scope.compare("local") == 0)
				{
					CriticalVariableEntry *entry = new CriticalVariableEntry(var, type, scope, function,linenum);
					criticalVariables.push_back(entry);
				} else if (scope.compare("global") == 0) {
					CriticalVariableEntry *entry = new CriticalVariableEntry(var, type, scope);
					criticalVariables.push_back(entry);
				}

			}
		} catch (const SettingNotFoundException &nfex) { /* Ignore. */ }
	}

	for (list<CriticalVariableEntry *>::iterator iter = criticalVariables.begin();
			iter != criticalVariables.end();
			iter++) {
		llvm::errs() << "<" << (*iter)->name << "," << (*iter)->type << "," << (*iter)->scope
				<< "," << (*iter)->function << ">\n";
	}

	outputCriticalVarsList(CriticalVariableListFilepath,criticalVariables);
	outputGOOMFMeta(GOOMFMetaFilepath,criticalVariables);
	outputGlobals(RiTHMicGlobalDefinition, RiTHMicGlobalHeader, criticalVariables);

	llvm::errs() << "========== Insert Global Header Include Statement ==========\n";
	ClangTool InsertHeaderTool(*Compilations, SourcePaths);
	result = InsertHeaderTool.run(newFrontendActionFactory<InsertGlobalizeHeaderAction>());

	llvm::errs() << "========== Insert Extern Statements ==========\n";
	ClangTool InsertExternTool(*Compilations, SourcePaths);
	result = InsertExternTool.run(newFrontendActionFactory<InsertDirectExternAction>());

	llvm::errs() << "========== Globalizing Source File(s) =========\n";
	RefactoringTool GlobalizeTool(*Compilations, SourcePaths);
	ast_matchers::MatchFinder GlobalizeFinder;
	DeclRefExprGlobalizeCallback declRefExprGlobalizeCallback(
			&GlobalizeTool.getReplacements(), criticalVariables);
	GlobalizeFinder.addMatcher(DeclRefExprMatcher,&declRefExprGlobalizeCallback);

	DeclStmtGlobalizeCallback declStmtGlobalizeCallback(
			&GlobalizeTool.getReplacements(), criticalVariables);
	GlobalizeFinder.addMatcher(DeclStmtMatcher, &declStmtGlobalizeCallback);

	result = GlobalizeTool.run(newFrontendActionFactory(&GlobalizeFinder));

	ClangTool FuncTool(*Compilations, SourcePaths);
	FuncFinder F;
	MatchFinder M;
	M.addMatcher(fDeclMatcher,&F);
	FuncTool.run(newFrontendActionFactory(&M));
	stringstream varnames;
	bufferprinter<<"fprintf(logFile,\\\"\\%ld,";
	varnames<<"line_num,";
	// Free dynamically allocated memory for critical variable list
	bool funcflag = false;
	for (list<CriticalVariableEntry*>::iterator itr = criticalVariables.begin();
			itr != criticalVariables.end();
			itr++) {
		funcflag = false;
		for(list<string>::iterator iter = FuncList.begin();
				iter != FuncList.end();
				iter++)
		{
			if((*iter).compare((*itr)->function)==0)
				funcflag = true;
		}
		if(!(*itr)->processed && funcflag && (*itr)->scope.compare("local")==0)
		{
			llvm::errs() << "ERROR: The following variable is not processed:\n"
						 << "Please verify whether configuration file entry is correct \n";
			error_count++;
			(*itr)->dump();
		}
		else
		{
			if	(	(*itr)->type.compare("int") == 0 ||
					(*itr)->type.compare("unsigned") == 0 ||
					(*itr)->type.compare("short") == 0
				)
				bufferprinter<<" \\%d,";
			if	(	(*itr)->type.compare("char") == 0
				)
				bufferprinter<<" \\%c,";
			if	(	(*itr)->type.compare("float") == 0
			)
				bufferprinter<<" \\%f,";
			if	(	(*itr)->type.compare("long long") == 0 ||
					(*itr)->type.compare("apr_time_t") == 0
			)
				bufferprinter<<" \\%lld,";
			if	(	(*itr)->type.compare("long") == 0
						)
							bufferprinter<<" \\%ld,";
			varnames<<"event\\."<<(*itr)->name<<",";

		}
		delete *itr;
	}
	bufferprinter.str().erase(bufferprinter.str().find_last_of(","));
	bufferprinter<<"\\\","<<varnames.str().erase(varnames.str().find_last_of(","))<<");";

	llvm::errs()<<bufferprinter.str()<<" written in "<<Bufferheader.c_str()<<"\n";
	bufferheader.flush();
	bufferheader<<"fprintf(logFile,\\\""<<varnames.str()<<"File Name\\\");"<<"\n";
	bufferheader<<bufferprinter.str()<<"\n";
	bufferheader.close();
	llvm::errs() << "========== Summarizing ==========\n";
	if(error_count == 0)
		llvm::errs() << " Globalizer completed successfully on " << SourcePaths[0] << "\n"
					 << " Number of ERRORS " << error_count << "\n"
					 << " Number of Warnings " << warning_count << "\n";

	else
		llvm::errs() << " Globalizer completed unsuccessfully on " << SourcePaths[0] << "\n"
		 	 	 	 << " Number of ERRORS " << error_count << "\n"
					 << " Number of Warnings " << warning_count << "\n";

	return 0;
}

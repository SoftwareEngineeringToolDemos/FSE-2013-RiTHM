#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/FrontendAction.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Host.h"

#include "InsertBracesFrontendAction.h"
#include "InstrumentFrontendAction.h"

#include "InstrumentationMap.h"
#include "AutoInstrumentOpts.h"
#include "clang/Lex/Preprocessor.h"
#include <fstream> 

using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace std;
using namespace clang::auto_instrument;

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
//static cl::extrahelp MoreHelp("\nMore help text...");

cl::opt<std::string> BuildPath(
  cl::Positional,
  cl::desc("[<build-path>]"));

cl::opt<std::string> InstrumentationScheme(
  cl::Positional,
  cl::desc("<instrumentation-scheme>"),
  cl::Required);

cl::opt<std::string> OutputMainFunctionMeta(
  cl::Positional,
  cl::desc("<main-function-meta>"),
  cl::Required);

cl::opt<std::string> Includefile(
  cl::Positional,
  cl::desc("<source0> [... <sourceN>]"),
  cl::Required);

cl::opt<std::string> CallInstFile(
  cl::Positional,
  cl::desc("<source0> [... <sourceN>]"),
  cl::Required);

cl::opt<std::string> Monitorcodefile(
  cl::Positional,
  cl::desc("<source0> [... <sourceN>]"),
  cl::Required);

cl::list<std::string> SourcePaths(
  cl::Positional,
  cl::desc("<source0> [... <sourceN>]"),
  cl::OneOrMore);




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

		if(S.str().compare("/*RITHM-monitor-code*/") == 0)
		{
			if(rewriter.isRewritable(Comment.getEnd()))
			{
				llvm::errs()<<"Inserting Monitor code below \n"
							<<S<<"\n";

				string line("\n"),code("");
				StringRef coderef;
				std::ifstream monitorfunc;
				monitorfunc.open(Monitorcodefile.c_str());
//				monitorfunc.open((Monitorcodefile.c_str()));
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
		if(S.str().compare("/*RITHM-monitor-call*/") == 0)
		{
			if(rewriter.isRewritable(Comment.getEnd()))
			{
				llvm::errs()<<"Inserting Monitor call instruction below \n"
						<<S<<"\n";

				string line("\n"),code("");
				StringRef coderef;
				std::ifstream monitorcall(CallInstFile.c_str());
				while (getline(monitorcall,line))
				{
					llvm::errs()<<	line<<"\n";
					code = code + "\n" + line;
				}
				monitorcall.close();
				coderef = code;
				rewriter.InsertText(Comment.getEnd(),code,true,true);
				if(rewriter.overwriteChangedFiles()) {
					llvm::errs()<<"error in rewrite \n";
				}
			}
		}
		if(S.str().compare("/*RITHM-includes*/") == 0)
		{
			if(rewriter.isRewritable(Comment.getEnd()))
			{
				llvm::errs()<<"Inserting includes below\n"
						<<S<<"\n";

				string line("\n"),code("");
				StringRef coderef;
				std::ifstream includefile(Includefile.c_str());
				while (getline(includefile,line))
				{
					llvm::errs()<<	line<<"\n";
					code = code + "\n" + line;
				}
				includefile.close();
				coderef = code;
				llvm::errs()<<code <<"this is code \n";
				rewriter.InsertText(Comment.getEnd(),code,true,true);
				if(rewriter.overwriteChangedFiles()) {
					llvm::errs()<<"error in rewrite \n";
				}
			}
		}
//  		scanf("%c",&x);
		return false;
	}
private:
	Rewriter &rewriter;
};
class InsertCustomCodeConsumer : public clang::ASTConsumer {
public:
	InsertCustomCodeConsumer(Rewriter &rewriter) : rewriter(rewriter) {}
	virtual void HandleTranslationUnit(clang::ASTContext &Context) {
		// Traversing the translation unit decl via a RecursiveASTVisitor
		// will visit all nodes in the AST.

	}
private:
	Rewriter &rewriter;
};

class InsertCustomCodeAction : public clang::ASTFrontendAction {
public:
	virtual clang::ASTConsumer *CreateASTConsumer(
			clang::CompilerInstance &Compiler, llvm::StringRef InFile) {

		ASTContext *ctx = &Compiler.getASTContext();
		rewriter = Rewriter(ctx->getSourceManager(), ctx->getLangOpts());
		C = new Commentchecker(rewriter);
		Compiler.getPreprocessor().addCommentHandler(C);
		return new InsertCustomCodeConsumer(rewriter);
	}
	~InsertCustomCodeAction()
	{
		delete C;
	}
private:
	Rewriter rewriter;
	Commentchecker *C;
};

int main(int argc, const char **argv) {

  int result;

  llvm::OwningPtr<CompilationDatabase> Compilations(
        FixedCompilationDatabase::loadFromCommandLine(argc, argv));
  cl::ParseCommandLineOptions(argc, argv);

  if(!Compilations) { // Couldn't find a compilation DB from the command line
    string ErrorMessage;
    Compilations.reset(
      !BuildPath.empty() ?
        CompilationDatabase::autoDetectFromDirectory(BuildPath, ErrorMessage) :
        CompilationDatabase::autoDetectFromSource(SourcePaths[0], ErrorMessage)
      );

    if (!Compilations) // Still no compilation DB? - bail.
      llvm::report_fatal_error(ErrorMessage);
  }

  // Ensure that clang can parse the source files first
//  llvm::errs() << "========== Checking Syntax of Source File(s) =========\n";
//  ClangTool SyntaxTool(*Compilations, SourcePaths);
//  if ( (result = SyntaxTool.run(
//        newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
//    llvm::errs() << "Error parsing files\n";
//    return result;
//  }

  // Insert braces in source files to ensure that instrumented source compiles
//  llvm::errs() << "========== Inserting Braces in Source File(s) =========\n";
//  ClangTool InsertBracesTool(*Compilations, SourcePaths);
//  result = InsertBracesTool.run(newFrontendActionFactory<InsertBracesFrontendAction>());

//  llvm::errs() << "---------- Checking Syntax of Modified Source File(s) ---------\n";
//  ClangTool AfterBracesSyntaxTool(*Compilations, SourcePaths);
//  if ( (result = AfterBracesSyntaxTool.run(
//        newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
//    llvm::errs() << "Error parsing files after inserting braces\n";
//    return result;
//  }


  llvm::errs() << "========== Auto-instrumenting Source File(s) =========\n";
  ClangTool InstrumentTool(*Compilations, SourcePaths);
  result = InstrumentTool.run(newFrontendActionFactory<InstrumentFrontendAction>());

  llvm::errs() << "========== Inserting CustomCode to Source File(s) =========\n";
  ClangTool InsertCustomCode(*Compilations, SourcePaths);
  result = InsertCustomCode.run(newFrontendActionFactory<InsertCustomCodeAction>());
  llvm::errs() << "========== Inserting GooMFHeader to Source File(s) =========\n";
  ClangTool InsertGooMFtool(*Compilations, SourcePaths);
  result = InsertGooMFtool.run(newFrontendActionFactory<InsertGooMFHeaderFrontEndAction>());
  llvm::errs() << "---------- Checking Syntax of Instrumented Source File(s) ---------\n";
  ClangTool AfterInstrumentSyntaxTool(*Compilations, SourcePaths);
  if ( (result = AfterInstrumentSyntaxTool.run(
        newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
    llvm::errs() << "Error parsing files after auto-instrumenting\n";
    return result;
  }
  ClangTool AfterInstrumentSyntaxTool1(*Compilations, SourcePaths);
   if ( (result = AfterInstrumentSyntaxTool1.run(
         newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
     llvm::errs() << "Error parsing files after auto-instrumenting\n";
     return result;
   }
   ClangTool AfterInstrumentSyntaxTool2(*Compilations, SourcePaths);
   if ( (result = AfterInstrumentSyntaxTool2.run(
		   newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
	   llvm::errs() << "Error parsing files after auto-instrumenting\n";
	   return result;
   }
   ClangTool AfterInstrumentSyntaxTool3(*Compilations, SourcePaths);
   if ( (result = AfterInstrumentSyntaxTool3.run(
		   newFrontendActionFactory<clang::SyntaxOnlyAction>())) ) {
	   llvm::errs() << "Error parsing files after auto-instrumenting\n";
	   return result;
   }
  llvm::errs()<<"\n";
  llvm::errs()<<"========== Auto-instrumentation completed  =========\n";
  llvm::errs()<<"The instrumentation has been done successfully\n";
  llvm::errs()<<"====================================================\n";
  return 0;
}

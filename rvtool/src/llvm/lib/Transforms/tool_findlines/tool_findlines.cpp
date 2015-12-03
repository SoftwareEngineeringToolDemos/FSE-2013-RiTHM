//===- tool_findlines.cpp - Finds points of function calls" ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/DebugInfo.h"
#include "llvm/Function.h"
#include "llvm/Module.h"
#include "llvm/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h" 
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/Support/CallSite.h"
#include "llvm/IntrinsicInst.h"
#include "llvm/Type.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include <fstream>


using namespace llvm;
using namespace std;


namespace {
	
	static cl::opt<string> directory("dir", cl::Hidden, cl::Required,cl::desc("directory of tool files"));

  struct tool_findlines : public ModulePass {
    static char ID; // Pass identification, replacement for typeid

    tool_findlines() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) {
		errs() << ">>>>>>>>>>>>>>> Start Extracting Function Calls for Merging CFGs" << "\n";

		
		std::string  error = "SOOMF: cant write FunctionCall";
		std::string fileName = directory;
		fileName.append("/LLVM/FunctionCalls.txt");
		static  raw_fd_ostream outFile(fileName.c_str(),error,0);
		
		for(Module::iterator mi = M.begin(), mp = M.end(); mi != mp; ++mi)
		{
			Function *F = &*mi;
			
			if(F->getName().str().find("llvm") == std::string::npos)
			{
				
				outFile << F->getName().str() << "\n";
				
				for (Function::iterator fi = F->begin() , fe = F->end(); fi !=fe; ++fi) {
					BasicBlock *BB = &*fi;
					for (BasicBlock::iterator bi = BB->begin() , be = BB->end(); bi !=be; ++bi) {
						Instruction *Inst = &*bi;
						if(isa<CallInst>(Inst) & !(isa<IntrinsicInst>(Inst)))
						{
							Function* calledFun = ((CallInst *)Inst)->getCalledFunction();
							if (MDNode *N = Inst->getMetadata("dbg")) {  // Here I is an LLVM instruction
								DILocation Loc(N);                      // DILocation is in DebugInfo.h
								unsigned Line = Loc.getLineNumber();
								if (calledFun == NULL) {
									outFile <<"->" << Line << "::" << *Inst <<"\n";
								}
								else
								{
									outFile <<"->" << Line << "::" << calledFun->getName().str() <<"\n";
								}

							}
						}
					}
				}
			}
			
		}

		outFile.close();
		
		errs() << ">>>>>>>>>>>>>>> Finished Extracting Function Calls for Merging CFGs" << "\n";


		return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char tool_findlines::ID = 0;
static RegisterPass<tool_findlines> X("tool_findlines", "finds function calls", false, false);


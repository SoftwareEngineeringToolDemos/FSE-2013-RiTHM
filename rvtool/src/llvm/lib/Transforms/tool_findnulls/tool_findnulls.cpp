//===- tool_findnulls.cpp - Extracting instructions requrired that do not have names " ---------------===//
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
#include <stdio.h>
#include <stdlib.h>

using namespace llvm;
using namespace std;


namespace {
	
	
  struct tool_findnulls : public ModulePass {
    static char ID; // Pass identification, replacement for typeid

    tool_findnulls() : ModulePass(ID) {}
	  

    virtual bool runOnModule(Module &M) {
		
		errs() << ">>>>>>>>>>>>>>>> Start Finding Null Instructions\n";

		std::string  error = "hello";
		static  raw_fd_ostream testFile("Nulls.txt",error,0);

		int op_counter = 0;

		
		
		for(Module::iterator mi = M.begin(), mp = M.end(); mi != mp; ++mi)
		{

			
			Function *F = &*mi;
			
			if(F->getName().str().find("llvm") == std::string::npos)
			{
				
				
				for (Function::iterator fi = F->begin() , fe = F->end(); fi !=fe; ++fi) {
					BasicBlock *BB = &*fi;
					for (BasicBlock::iterator bi = BB->begin() , be = BB->end(); bi !=be; ++bi) {
						Instruction *Inst = &*bi;
						if(Inst->getOpcode() == 28)
						{
							op_counter = 0;
							for (User::op_iterator op_i = Inst->op_begin(), op_e = Inst->op_end(); op_i != op_e; ++op_i) {
								op_counter++;
								Value *v = *op_i;
								
								if (op_counter == 2 && v->hasName() == false) {
//									testFile << *Inst << ",";
									if (MDNode *N = Inst->getMetadata("dbg")) {  // Here I is an LLVM instruction
										DILocation Loc(N);                      // DILocation is in DebugInfo.h
										unsigned tempLine = Loc.getLineNumber();
//										testFile <<">>"<< tempLine << "\n";
									}
								}
							}
						}
						if(Inst->getOpcode() == 27)
						{
							op_counter = 0;
							for (User::op_iterator op_i = Inst->op_begin(), op_e = Inst->op_end(); op_i != op_e; ++op_i) {
								op_counter++;
								Value *v = *op_i;

								
								if (v->hasName() == false) {
//									testFile << *Inst << ",";
									if (MDNode *N = Inst->getMetadata("dbg")) {  // Here I is an LLVM instruction
										DILocation Loc(N);                      // DILocation is in DebugInfo.h
										unsigned tempLine = Loc.getLineNumber();
//										testFile <<">>"<< tempLine << "\n";
									}

								}
							}

						}
					}
				}
			}
		}
				

		errs() << ">>>>>>>>>>>>>>>> Finished Finding Null Instructions\n";

		return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char tool_findnulls::ID = 0;
static RegisterPass<tool_findnulls> X("tool_findnulls", "finds critical instructions of given input file", false, false);


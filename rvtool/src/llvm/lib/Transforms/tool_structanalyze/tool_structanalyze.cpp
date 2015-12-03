//===- tool_structanalyze.cpp - Extracting instructions requrired that do not have names " ---------------===//
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
	
	
  struct tool_structanalyze : public ModulePass {
    static char ID; // Pass identification, replacement for typeid

    tool_structanalyze() : ModulePass(ID) {}
	  

    virtual bool runOnModule(Module &M) {
		
		std::string  error = "hello";
		static  raw_fd_ostream testFile("Nulls.txt",error,0);
		
		
		for(Module::iterator mi = M.begin(), mp = M.end(); mi != mp; ++mi)
		{

			
			Function *F = &*mi;
			
			if(F->getName().str().find("llvm") == std::string::npos)
			{
				
				
				for (Function::iterator fi = F->begin() , fe = F->end(); fi !=fe; ++fi) {
					BasicBlock *BB = &*fi;
					for (BasicBlock::iterator bi = BB->begin() , be = BB->end(); bi !=be; ++bi) {
						Instruction *Inst = &*bi;
						errs() << *Inst << " CODE " << Inst->getOpcode() <<"\n";
						
						for (User::op_iterator op_i = Inst->op_begin(), op_e = Inst->op_end(); op_i != op_e; ++op_i) {
							Value *v = *op_i;
							
							errs() << "---- OPERANDS " << *v << "  TYPE " << v->getType()->getTypeID() << "\n";
													
							if(v->hasName())
							{
								errs () << " has name \n";
							}
							
						}
											
						
					}
				}
			}
		}
				
		return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char tool_structanalyze::ID = 0;
static RegisterPass<tool_structanalyze> X("tool_structanalyze", "finds critical instructions of given input file", false, false);


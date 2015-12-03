//===- tool_file2fun.cpp - Maps C files to their functions " ---------------===//
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
#include <dirent.h>

using namespace llvm;
using namespace std;


namespace {
	
  static cl::opt<std::string> dirName("dir", cl::Hidden, cl::Required,cl::desc("directory of src files"));

	
  struct tool_file2fun : public ModulePass {
    static char ID; // Pass identification, replacement for typeid

    tool_file2fun() : ModulePass(ID) {}
	  

    virtual bool runOnModule(Module &M) {
		
		errs() << ">>>>>>>>>>>>>>>> Start Mapping File to its Functions\n";

		std::string  error = "hello";
		static  raw_fd_ostream testFile("file2fun.txt",error,0);

		int op_counter = 0;
		
		DIR *dir;
		
		struct dirent *ent;
		
		dir = opendir(dirName.c_str());
		if (dir != NULL) {
			
			/* print all the files and directories within directory */
			while ((ent = readdir (dir)) != NULL) {
				errs() << ent->d_name << " ....... " << ent->d_type << "\n";
				
				std::string tempDir = dirName;
				tempDir = tempDir.append("/");
				tempDir = tempDir.append(ent->d_name);
				
				DIR* dir2 = opendir(tempDir.c_str());
				if(dir2 != NULL)
				{
					errs() << " is type DIR" << "\n";

				}
				else {
					errs() << " is type FILE" << "\n";
				}

									
			}
			closedir (dir);
		} else {
			/* could not open directory */
			errs() << "could not open directory" << "\n";
		}

		
		
		/*for(Module::iterator mi = M.begin(), mp = M.end(); mi != mp; ++mi)
		{

			
			Function *F = &*mi;
			
			if(F->getNameStr().find("llvm") == std::string::npos)
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
									testFile << *Inst << ",";
									if (MDNode *N = Inst->getMetadata("dbg")) {  // Here I is an LLVM instruction
										DILocation Loc(N);                      // DILocation is in DebugInfo.h
										unsigned tempLine = Loc.getLineNumber();
										testFile <<">>"<< tempLine << "\n";										
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
									testFile << *Inst << ",";
									if (MDNode *N = Inst->getMetadata("dbg")) {  // Here I is an LLVM instruction
										DILocation Loc(N);                      // DILocation is in DebugInfo.h
										unsigned tempLine = Loc.getLineNumber();
										testFile <<">>"<< tempLine << "\n";
									}

								}
							}

						}
					}
				}
			}
		}*/
				

		errs() << ">>>>>>>>>>>>>>>> Start Mapping File to its Functions\n";

		return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char tool_file2fun::ID = 0;
static RegisterPass<tool_file2fun> X("tool_file2fun", "maps C files to their functions", false, false);


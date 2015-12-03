//===- tool_rename.cpp - Renaming the temporary registers of IR " ---------------===//
//
//                     The LLVM Compiler Infrastructure.
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
#include "llvm/Operator.h"
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

	struct critInstructions {
		std::string func;
		std::string variable;
	};
	std::vector<critInstructions*> critInstVector;

	static cl::opt<std::string> dir("dir", cl::Hidden, cl::Required,cl::desc("enter dir path"));

	struct tool_rename : public ModulePass {
	  
    static char ID; // Pass identification, replacement for typeid
	  

    tool_rename() : ModulePass(ID) {}
	  
    bool search_crit_var(string name)
    {
    	for(unsigned long i = 0; i < critInstVector.size()-1;i++)
    	{
    		if(critInstVector.at(i)->variable.compare(name) == 0)
    			return true;
    	}
    	return false;
    }
    virtual bool runOnModule(Module &M) {

    	static std::string critInst;
    	std::string varName;
    	string critfilename = dir;
    	critfilename.append("/Pass/critInst.txt");
    	static  std::ifstream critFile(critfilename.c_str());
    	size_t found;


    	if (critFile.is_open())
    	{
    		while ( !critFile.eof() )
    		{
    			getline (critFile,critInst);
    			found = critInst.find_first_of(",");
    			critInstructions *ci = new critInstructions();
    			ci->func = critInst.substr(0,found);
    			ci->variable = critInst.substr(found+1);
    			critInstVector.push_back(ci);

    		}
    		critFile.close();
    	}
		
		errs() << ">>>>>>>>>>>>>>>>> Start Renaming registers" << "\n";
		
		static  std::string FileName = dir;
		FileName.append("/Alias/func.txt");
		static  std::ofstream outFile;
		outFile.open(FileName.c_str(),ios::app);
		
		//---------------------------- renaming the insturctions
		for(Module::iterator mi = M.begin(), mp = M.end(); mi != mp; ++mi)
		{
			Function *F = &*mi;
			if (F->getName().str().find("llvm") == std::string::npos) {
				outFile << F->getName().str() << "\n";

			}

			for (Function::iterator fi = F->begin() , fe = F->end(); fi !=fe; ++fi) {
				BasicBlock *BB = &*fi;
				for (BasicBlock::iterator bi = BB->begin() , be = BB->end(); bi !=be; ++bi) {
					Value *val = &*bi;
					Instruction *inst = &*bi;
					if (val->hasName() == false
							&& inst->getOpcode() != Instruction::Call
							&& inst->getOpcode() != Instruction::Store
							&& inst->getOpcode() != Instruction::Br
							&& inst->getOpcode() != Instruction::Ret
							&& !(val->getType()->isVoidTy()))
					{
						Twine *tw = new Twine("_rvtool");
						val->setName(*tw);
					}
				}
			}

		}
		outFile.close();
		//----------------------------
		errs() << ">>>>>>>>>>>>>>>>> Renaming Complete" << "\n";


		return false;
    }

    // We don't modify the program, so we preserve all analyses
    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
  };
}

char tool_rename::ID = 0;
static RegisterPass<tool_rename> X("tool_rename", "renaming instructions", false, false);


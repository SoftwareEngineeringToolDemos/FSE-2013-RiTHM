//===-- CFGPrinter.h - CFG printer external interface -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines external functions that can be called to explicitly
// instantiate the CFG printer.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_ANALYSIS_CFGPRINTER_H
#define LLVM_ANALYSIS_CFGPRINTER_H

#include "llvm/Constants.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/GraphWriter.h"

#include "llvm/DebugInfo.h"
#include <iostream>
#include <sstream>

namespace llvm {
template<>
struct DOTGraphTraits<const Function*> : public DefaultDOTGraphTraits {

  DOTGraphTraits (bool isSimple=false) : DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(const Function *F) {
    return "CFG for '" + F->getName().str() + "' function";
  }

  static std::string getSimpleNodeLabel(const BasicBlock *Node,
                                        const Function *) {
    if (!Node->getName().empty())
      return Node->getName().str();

    std::string Str;
    raw_string_ostream OS(Str);

    WriteAsOperand(OS, Node, false);
    return OS.str();
  }

  static std::string getCompleteNodeLabel(const BasicBlock *Node, 
                                          const Function *) {
    std::string Str;
    raw_string_ostream OS(Str);

    if (Node->getName().empty()) {
      WriteAsOperand(OS, Node, false);
      OS << ":";
    }

    OS << *Node;
    std::string OutStr = OS.str();
    if (OutStr[0] == '\n') OutStr.erase(OutStr.begin());

    // Process string output to make it nicer...
    for (unsigned i = 0; i != OutStr.length(); ++i)
      if (OutStr[i] == '\n') {                            // Left justify
        OutStr[i] = '\\';
        OutStr.insert(OutStr.begin()+i+1, 'l');
      } else if (OutStr[i] == ';') {                      // Delete comments!
        unsigned Idx = OutStr.find('\n', i+1);            // Find end of line
        OutStr.erase(OutStr.begin()+i, OutStr.begin()+Idx);
        --i;
      }

    return OutStr;
  }

  ///---------------------------------- ADDED BY SAM
	std::string getNodeLines(const BasicBlock *Node,
							 const Function *Graph) {
		int counter = 0;
		std::string output ="\n ";
		std::string temp ="";
		std::string temp2 ="";
		std::string begin ="";
		std::string weight = "\n";
		std::string before = "";
		int cweight = 1;
		for(BasicBlock::const_iterator i = Node->begin(), p = Node->end(); i != p; ++i)
		{
		
			if (MDNode *N = i->getMetadata("dbg")) {

				DILocation Loc(N);
				unsigned Line = Loc.getLineNumber();
				std::stringstream ss;
				ss << Line;
				if(counter == 0)
				{
					begin.append(ss.str());
					before=ss.str();
					counter++;
				} 
				temp=ss.str();
				if(before.compare(temp) == 0)
				{
					cweight++;
				}
				else {
					weight.append(",");
					weight.append(before);
					weight.append("@");
					std::stringstream ssweight;
					ssweight << cweight;
					
					weight.append(ssweight.str());
					cweight =0;
				}

				before=ss.str();
			}
		}
		
		weight.append(",");
		weight.append(temp);
		weight.append("@");
		std::stringstream ssweight;
		ssweight << cweight;
		
		weight.append(ssweight.str());
		cweight =0;
		weight.append("\n");
		
		
		for(BasicBlock::const_iterator i = Node->begin(), p = Node->end(); i != p; ++i)
		{
			
			if (MDNode *N = i->getMetadata("dbg")) {
				
				DILocation Loc(N);
				unsigned Line = Loc.getLineNumber();
				std::stringstream ss1;
				ss1 << Line;
				if(ss1.str().compare(temp)!=0 && ss1.str().compare(begin)!=0)
				{
					temp2 = ss1.str();
				}
			}
		}
		if (temp2.empty()) {
			output.append(begin);
			output.append("--");
			output.append(temp);
			output.append(weight);

		}
		else
		{
			output.append(begin);
			output.append("--");
			output.append(temp2);
			output.append("--");
			output.append(temp);
			output.append(weight);
		}
		return output;

	}
  ///---------------------------------- ADDED BY SAM


  std::string getNodeLabel(const BasicBlock *Node,
                           const Function *Graph) {
    if (isSimple())
      return getSimpleNodeLabel(Node, Graph);
    else
      return getCompleteNodeLabel(Node, Graph);
  }

  static std::string getEdgeSourceLabel(const BasicBlock *Node,
                                        succ_const_iterator I) {
    // Label source of conditional branches with "T" or "F"
    if (const BranchInst *BI = dyn_cast<BranchInst>(Node->getTerminator()))
      if (BI->isConditional())
        return (I == succ_begin(Node)) ? "T" : "F";
    
    // Label source of switch edges with the associated value.
    if (const SwitchInst *SI = dyn_cast<SwitchInst>(Node->getTerminator())) {
      unsigned SuccNo = I.getSuccessorIndex();

      if (SuccNo == 0) return "def";
      
      std::string Str;
      raw_string_ostream OS(Str);
      SwitchInst::ConstCaseIt Case =
          SwitchInst::ConstCaseIt::fromSuccessorIndex(SI, SuccNo); 
      OS << Case.getCaseValue()->getValue();
      return OS.str();
    }    
    return "";
  }
};
} // End llvm namespace

namespace llvm {
  class FunctionPass;
  FunctionPass *createCFGPrinterPass ();
  FunctionPass *createCFGOnlyPrinterPass ();
} // End llvm namespace

#endif

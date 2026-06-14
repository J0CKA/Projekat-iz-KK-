//KLASNI INTERFEJS
#ifndef LLVM_PROJECT_OURCFG_H
#define LLVM_PROJECT_OURCFG_H

#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace llvm;

//deklaracija privatnih i javnih funkcija
class OurCFG {
private:
  std::string FunctionName;
  std::unordered_set<BasicBlock *>Visited;
  std::unordered_map<BasicBlock *, std::vector<BasicBlock *>> AdjacencyList;
  void CreateCFG(Function &F);
  void DumpBasicBlock(BasicBlock *, raw_fd_stream &);
public:
  OurCFG(Function &F);
  void DFS(BasicBlock *); 
  bool isReachable(BasicBlock *); 
  void DumpToFile();
};

#endif 

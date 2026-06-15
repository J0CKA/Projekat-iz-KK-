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
class OurCFG { //Control Flow Graph (Graf kontrole toka)
private:
  std::string FunctionName; //ime date fje
  std::unordered_set<BasicBlock *>Visited;  //skup posećenih čvorova - BB
  std::unordered_map<BasicBlock *, std::vector<BasicBlock *>> AdjacencyList; //mapa koja za svaki čvor-BB čuva skup naslednika
  void CreateCFG(Function &F);  //fja koja konstruiše graf
  void DumpBasicBlock(BasicBlock *, raw_fd_stream &);
public:
  OurCFG(Function &F); //konstruktor
  void DFS(BasicBlock *);  //fja za DFS obilazak CFG
  bool isReachable(BasicBlock *);  //fja za proveru da li se može stići do datog bloka
  void DumpToFile(); //to .dot
};

#endif 

#include "OurCFG.h"

//konstruktor klase - inicijalizuje CFG
OurCFG::OurCFG(Function &F)
{
  FunctionName = F.getName().str(); //čuva kao string ime fje
  CreateCFG(F); //gradi graf
}

//konstruiše graf na osnovu strukture funkcije
void OurCFG::CreateCFG(llvm::Function &F)
{
  for ( BasicBlock &BB : F) { //prolazi kroz sve bazne blokove unutar funkcije
    AdjacencyList[&BB] = {}; //inicijalizuje praznu listu naslednika za dati blok
    for ( BasicBlock *Successor : successors(&BB)) {
        AdjacencyList[&BB].push_back(Successor); //dodaje naslednika
    }

    //STARI NAČIN 
//    for (auto &Instr : BB) {
//      if ( BranchInst *BranchInstr = dyn_cast<BranchInst>(&Instr)) {
//        AdjacencyList[&BB].push_back(BranchInstr->getSuccessor(0));
//        if (BranchInstr->isConditional()) {
//          AdjacencyList[&BB].push_back(BranchInstr->getSuccessor(1));
//        }
//      }
//    }
  }
}
//rekurzivni obilazak grafa u dubinu počevši od zadatog bloka
void OurCFG::DFS(llvm::BasicBlock *CurrentBlock)
{
  Visited.insert(CurrentBlock); //trenutni blok označava kao posećen ubacivanjem u kup

  for (BasicBlock *Successor : AdjacencyList[CurrentBlock]) {
    if (Visited.find(Successor) == Visited.end()) { //ako naslednik još uvek nije posećen, rekurzivno pokreće DFS za njega
      DFS(Successor);
    }
  }
}

//proverava da li je zadati bazni blok dostižan (da li ga je DFS posetio)
bool OurCFG::isReachable(llvm::BasicBlock *BB)
{
  return Visited.find(BB) != Visited.end();
}

//generiše .dot fajl koji služi za vizuelizaciju grafa pomoću Graphviz alata
void OurCFG::DumpToFile()
{
  std::error_code error;
  raw_fd_stream File("our" + FunctionName + ".dot", error);


  File << "digraph \"CFG for '" << FunctionName << "' function\" {\n";
  File << "\tlabel=\"Ovo je neki nas tekst\";\n\n";

  for ( auto &p : AdjacencyList) {
    DumpBasicBlock(p.first, File);
  }

  File << "}\n";
}

//void OurCFG::DumpBasicBlock( llvm::BasicBlock *Current, llvm::raw_fd_stream &File)
//{
//  bool hasMultipleSuccessors = false;
//
//  File << "\tNode" << Current << " [shape=record,color=\"#b70d28ff\", style=filled, fillcolor=\"#b70d2870\",label=\"{";
//
//  for ( Instruction &Instr : *Current) {
//    if (Instr.isTerminator()) {
//      if (BranchInst *BrInstr = dyn_cast<BranchInst>(&Instr)) {
//        if (BrInstr->isConditional()) {
//        hasMultipleSuccessors = true;
//        File << Instr << "\\l|{<s0>True|<s1>False}}\"];\n";
//      }
//      }
//      else {
//        File << Instr << "\\l}\"];\n";
//      }
//    }
//    else {
//      File << Instr << "\\l ";
//    }
//  }
//
//  int successorNum = 0;
//  for ( BasicBlock *Successor : AdjacencyList[Current]) {
//    if (hasMultipleSuccessors) {
//      File << "\tNode" << Current << ":s" << successorNum++ << " -> Node" << Successor << ";\n";
//    }
//    else {
//      File << "\tNode" << Current << " -> Node" << Successor << ";\n";
//    }
//  }
//}

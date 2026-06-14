#include "llvm/Pass.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "OurCFG.h"

using namespace llvm;

namespace {
struct OurDeadCodeElimination : public FunctionPass {
  std::unordered_map<Value *, bool> Variables; 
  std::unordered_map<Value *, Value *> VariablesMap;

  std::vector<Instruction *> InstructionsToRemove;
  bool InstructionEliminated;

  static char ID;
  OurDeadCodeElimination() : FunctionPass(ID) {}

  void handleOperand(Value *Operand) 
  {
    Variables[Operand] = true;
    Variables[VariablesMap[Operand]] = true; 
  }

  void eliminateDeadInstructions(Function &F) 
  {
    InstructionsToRemove.clear(); 

    for (BasicBlock &BB : F) { 
      for (Instruction &I : BB) {
        if (I.getType()->getTypeID() != Type::VoidTyID && !isa<CallInst>(&I)) {
          Variables[&I] = false;
        }
        if (isa<LoadInst>(&I)) {
          VariablesMap[&I] = I.getOperand(0);
        }

        if (isa<StoreInst>(&I)) {
          if (Variables.find(I.getOperand(0)) != Variables.end()) {
            handleOperand(I.getOperand(0));
          }
        }
        else {
          for (size_t i = 0; i < I.getNumOperands(); i++) {
            if (Variables.find(I.getOperand(i)) != Variables.end()) {
              handleOperand(I.getOperand(i));
            }
          }
        }
      }
    }

    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (isa<StoreInst>(&I)) {
          if (Variables.find(I.getOperand(1)) != Variables.end() && !Variables[I.getOperand(1)]) {
            InstructionsToRemove.push_back(&I);
          }
        }
        else if (Variables.find(&I) != Variables.end() && !Variables[&I]) {
          InstructionsToRemove.push_back(&I);
        }
      }
    }

    if (InstructionsToRemove.size() > 0) {
      InstructionEliminated = true;
    }

    for (Instruction *Instr : InstructionsToRemove) {
      Instr->eraseFromParent();
    }
  }

  void eliminateUnreachableInstructions(Function &F) 
  {
    std::vector<BasicBlock *> UnreachableBlocks;
    OurCFG *CFG = new OurCFG(F);
    CFG->DFS(&F.front()); 

    for (BasicBlock &BB : F) { 
      if (!CFG->isReachable(&BB)) {
        UnreachableBlocks.push_back(&BB);
      }
    }

    if (UnreachableBlocks.size() > 0) {
      InstructionEliminated = true;
    }

    for (BasicBlock *UnreachableBlock : UnreachableBlocks) { 
      UnreachableBlock->eraseFromParent();
    }
  }

  bool runOnFunction(Function &F) override { 
    do {
      InstructionEliminated = false;
      eliminateDeadInstructions(F);
      eliminateUnreachableInstructions(F);
    } while(InstructionEliminated); 
    return true;
  }
};
}  

char OurDeadCodeElimination::ID = 0;
static RegisterPass<OurDeadCodeElimination> X("our-constant-folding", "OurDeadCodeElimination pass", false, false);

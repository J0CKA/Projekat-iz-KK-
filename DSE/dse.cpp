#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace llvm;

namespace {

struct DSEPass : public PassInfoMixin<DSEPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        bool promena = false;
        
        std::unordered_map<BasicBlock*, std::unordered_set<Value*>> ziveNaUlazu;
        std::unordered_map<BasicBlock*, std::unordered_set<Value*>> ziveNaIzlazu;
        
        std::vector<BasicBlock*> radnaLista;
        std::unordered_set<BasicBlock*> zaObradu;

        for (BasicBlock &BB : llvm::reverse(F)) {
            radnaLista.push_back(&BB);
            zaObradu.insert(&BB);
        }
        std::reverse(radnaLista.begin(), radnaLista.end());

        while (!radnaLista.empty()) {
            BasicBlock *BB = radnaLista.back();
            radnaLista.pop_back();
            zaObradu.erase(BB);

            std::unordered_set<Value*> novoStanjeIzlaza;
            for (BasicBlock *suc : successors(BB)) {
                auto It = ziveNaUlazu.find(suc);
                if (It != ziveNaUlazu.end()) {
                    novoStanjeIzlaza.insert(It->second.begin(), It->second.end());
                }
            }
            ziveNaIzlazu[BB] = novoStanjeIzlaza;

  
            std::unordered_set<Value*> tekucaZivost = novoStanjeIzlaza;
            for (auto I = BB->rbegin(); I != BB->rend(); ++I) {
                Instruction &Inst = *I;

                if (auto *LI = dyn_cast<LoadInst>(&Inst)) {
                    tekucaZivost.insert(LI->getPointerOperand());
                } 
                else if (auto *SI = dyn_cast<StoreInst>(&Inst)) {
                    tekucaZivost.erase(SI->getPointerOperand());
                } 
                else if (auto *CI = dyn_cast<CallInst>(&Inst)) {
                    if (!CI->onlyReadsMemory()) {
                        tekucaZivost.clear();
                    }
                }
            }

            if (ziveNaUlazu[BB] != tekucaZivost) {
                ziveNaUlazu[BB] = tekucaZivost;

                for (BasicBlock *pred : predecessors(BB)) {
                    if (zaObradu.find(pred) == zaObradu.end()) {
                        radnaLista.push_back(pred);
                        zaObradu.insert(pred);
                    }
                }
            }
        }

        for (BasicBlock &BB : F) {
            std::unordered_set<Value*> zivaMemorija = ziveNaIzlazu[&BB];
            std::vector<StoreInst*> zaBrisanje;

            for (auto I = BB.rbegin(); I != BB.rend(); ++I) {
                Instruction &Inst = *I;

                if (auto *LI = dyn_cast<LoadInst>(&Inst)) {
                    zivaMemorija.insert(LI->getPointerOperand());
                    continue;
                }

                if (auto *SI = dyn_cast<StoreInst>(&Inst)) {
                    Value *Ptr = SI->getPointerOperand();

                    if (zivaMemorija.find(Ptr) == zivaMemorija.end()) {
                        zaBrisanje.push_back(SI);
                    } else {
                        zivaMemorija.erase(Ptr);
                    }
                    continue;
                }

                if (auto *CI = dyn_cast<CallInst>(&Inst)) {
                    if (!CI->onlyReadsMemory()) {
                        zivaMemorija.clear();
                    }
                }
            }

            for (StoreInst *SI : zaBrisanje) {
                SI->eraseFromParent();
                promena = true;
            }
        }

        return (promena ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }
};

}

llvm::PassPluginLibraryInfo getDSEPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "dse", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "dse") {
                        FPM.addPass(DSEPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getDSEPluginInfo();
}

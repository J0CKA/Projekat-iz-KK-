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

        std::unordered_map<BasicBlock*, std::unordered_set<Value*>> ziveNaIzlazu;
        
        std::vector<BasicBlock*> radnaLista;
        std::unordered_set<BasicBlock*> zaObradu;

        for (auto BBIt = F.rbegin(); BBIt != F.rend(); ++BBIt) {
            radnaLista.push_back(&*BBIt);
            zaObradu.insert(&*BBIt);
        }

        while (!radnaLista.empty()) {
            BasicBlock *BB = radnaLista.back();
            radnaLista.pop_back();
            zaObradu.erase(BB);

            std::unordered_set<Value*> novoStanjeIzlaza;

            for (BasicBlock *suc : successors(BB)) {
                auto It = ziveNaIzlazu.find(suc);
                if (It != ziveNaIzlazu.end()) {
                    novoStanjeIzlaza.insert(It->second.begin(), It->second.end());
                }
            }

            if (ziveNaIzlazu[BB] != novoStanjeIzlaza) {
                ziveNaIzlazu[BB] = novoStanjeIzlaza;

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
                errs() << "Uklanja instrukciju: " << *SI << "\n";
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

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/Local.h" 
#include <vector>

using namespace llvm;

namespace {

struct DCEPass : public PassInfoMixin<DCEPass> {
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        bool promena = false;

        
        if (removeUnreachableBlocks(F)) {
            errs() << "Uklonjeni su nedostižni bazični blokovi.\n";
            promena = true;
        }

       
        std::vector<Instruction*> radnaLista;

       
        for (auto &BB : F) {
            for (auto &I : BB) {
                radnaLista.push_back(&I);
            }
        }

       
        while (!radnaLista.empty()) {
            Instruction *instrukcija = radnaLista.back();
            radnaLista.pop_back();

            
            if (!instrukcija->use_empty() || instrukcija->isTerminator() || instrukcija->mayHaveSideEffects())
                continue;

            errs() << "Brišemo mrtvu instrukciju: " << *instrukcija << "\n";

            
            for (unsigned i = 0; i < instrukcija->getNumOperands(); ++i) {
                if (Instruction *vezaneInstrukcije = dyn_cast<Instruction>(instrukcija->getOperand(i))) {
                    radnaLista.push_back(vezaneInstrukcije);
                }
            }

            
            instrukcija->eraseFromParent();
            promena = true;
        }

        
        return (promena ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }
};

} 


llvm::PassPluginLibraryInfo getDCEPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "dce", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "dce") {
                        FPM.addPass(DCEPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getDCEPluginInfo();
}

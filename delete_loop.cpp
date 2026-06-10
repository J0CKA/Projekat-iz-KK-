#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Dominators.h"

#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

#include "llvm/Transforms/Utils/LoopUtils.h"

#include "llvm/Support/raw_ostream.h"

#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include <vector>

using namespace llvm;

namespace {

struct BrisanjeMrtvihPetljiPass
    : public PassInfoMixin<BrisanjeMrtvihPetljiPass> {

    bool ImaBocneEfekte(Loop *Petlja) {
        for (BasicBlock *BB : Petlja->blocks()) {
            for (Instruction &Inst : *BB) {

                if (Inst.mayHaveSideEffects())
                    return true;

                if (isa<CallBase>(&Inst))
                    return true;
            }
        }

        return false;
    }

    bool KoristiSeVanPetlje(Loop *Petlja) {
        
        SmallVector<BasicBlock *, 4> IzlazniBlokovi;
        Petlja->getExitBlocks(IzlazniBlokovi);

        for (BasicBlock *BB : Petlja->blocks()) {
            for (Instruction &Inst : *BB) {
                for (User *U : Inst.users()) {
                    if (auto *KorisnikInst = dyn_cast<Instruction>(U)) {
                        
                        
                        if (!Petlja->contains(KorisnikInst)) {
                            
                           
                            if (auto *PN = dyn_cast<PHINode>(KorisnikInst)) {
                                bool JeIzlazniPHI = false;
                                for (BasicBlock *EB : IzlazniBlokovi) {
                                    if (PN->getParent() == EB) {
                                        JeIzlazniPHI = true;
                                        break;
                                    }
                                }
                                if (JeIzlazniPHI) {
                                    continue; 
                                }
                            }
                            
                            
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    PreservedAnalyses run(Function &F,
                          FunctionAnalysisManager &FAM) {

        LoopInfo &LI =
            FAM.getResult<LoopAnalysis>(F);

        ScalarEvolution &SE =
            FAM.getResult<ScalarEvolutionAnalysis>(F);

        DominatorTree &DT =
            FAM.getResult<DominatorTreeAnalysis>(F);

        bool Izmenjeno = false;

        std::vector<Loop *> MrtvePetlje;

        for (Loop *Petlja : LI) {

            if (!Petlja->getSubLoops().empty())
                continue;

            const SCEV *BrojIteracija =
                SE.getBackedgeTakenCount(Petlja);

            if (isa<SCEVCouldNotCompute>(BrojIteracija))
                continue;

            if (ImaBocneEfekte(Petlja))
                continue;

            if (KoristiSeVanPetlje(Petlja))
                continue;

            MrtvePetlje.push_back(Petlja);
        }

        for (Loop *Petlja : MrtvePetlje) {

            errs() << "[LoopDeletion] Pronadjena i brise se mrtva petlja!\n";

            
            deleteDeadLoop(
                Petlja,
                &DT,
                &SE,
                &LI
            );

            Izmenjeno = true;
        }

        if (Izmenjeno)
            return PreservedAnalyses::none();

        return PreservedAnalyses::all();
    }
};

} 

llvm::PassPluginLibraryInfo
getBrisanjeMrtvihPetljiPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "loop-deletion",
        LLVM_VERSION_STRING,

        [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                   FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                    if (Name == "loop-deletion") {

                        FPM.addPass(
                            BrisanjeMrtvihPetljiPass());

                        return true;
                    }

                    return false;
                });
        }
    };
}

extern "C"
LLVM_ATTRIBUTE_WEAK
::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {

    return getBrisanjeMrtvihPetljiPluginInfo();
}

// LLVM header-i za rad sa IR-om, funkcijama, instrukcijama i pass sistemom
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/Local.h"  // za removeUnreachableBlocks
#include <vector>

using namespace llvm;

namespace {

// Definišemo novi LLVM pass koji radi na nivou FUNCTION
struct DCEPass : public PassInfoMixin<DCEPass> {

    // Glavna funkcija pass-a
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {

        // Flag koji prati da li smo nešto promenili u funkciji
        bool promena = false;

        // 1. Uklanjanje nedostižnih basic blokova (dead blocks)
        if (removeUnreachableBlocks(F)) {
            errs() << "Uklonjeni su nedostižni bazični blokovi.\n";
            promena = true;
        }

        // Lista instrukcija koje ćemo obraditi (worklist algoritam)
        std::vector<Instruction*> radnaLista;

        // 2. Ubacujemo sve instrukcije iz funkcije u listu
        for (auto &BB : F) {              // prolazimo kroz sve basic block-ove
            for (auto &I : BB) {          // prolazimo kroz sve instrukcije u bloku
                radnaLista.push_back(&I); // čuvamo pointer na instrukciju
            }
        }

        // 3. Worklist algoritam (obrada instrukcija)
        while (!radnaLista.empty()) {

            // uzmi poslednju instrukciju iz liste (LIFO - stack ponašanje)
            Instruction *instrukcija = radnaLista.back();
            radnaLista.pop_back();

            // 4. PROVERA da li instrukciju treba zadržati:

            // Ako:
            // - ima upotrebu (neko je koristi)
            // - ili je terminator (branch, return)
            // - ili ima side effect (npr. store, call)
            // onda je NE smemo brisati
            if (!instrukcija->use_empty() ||
                instrukcija->isTerminator() ||
                instrukcija->mayHaveSideEffects())
                continue;

            // debug ispis (šta brišemo)
            errs() << "Brišemo mrtvu instrukciju: " << *instrukcija << "\n";

            // 5. Dodajemo zavisne instrukcije u worklist
            // (ako ova instrukcija koristi neke druge instrukcije)
            for (unsigned i = 0; i < instrukcija->getNumOperands(); ++i) {

                // ako operand jeste instrukcija → dodaj je u listu
                if (Instruction *vezaneInstrukcije =
                        dyn_cast<Instruction>(instrukcija->getOperand(i))) {
                    radnaLista.push_back(vezaneInstrukcije);
                }
            }

            // 6. Brisanje instrukcije iz IR-a
            instrukcija->eraseFromParent();

            // označavamo da je promena napravljena
            promena = true;
        }

        // 7. LLVM mora da zna da li smo menjali IR:
        // - none() = sve analize moraju ponovo da se izračunaju
        // - all() = ništa se nije promenilo
        return (promena ? PreservedAnalyses::none()
                        : PreservedAnalyses::all());
    }
};

} // kraj namespace-a


// ===============================
// REGISTRACIJA PLUGIN-a
// ===============================

// Ovo vraća informacije o plugin-u LLVM-u
llvm::PassPluginLibraryInfo getDCEPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "dce",                     // ime pass-a
        LLVM_VERSION_STRING,

        // registracija pipeline callback-a
        [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                   FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {

                    // ako korisnik napiše "-passes=dce"
                    if (Name == "dce") {
                        FPM.addPass(DCEPass()); // dodaj naš pass
                        return true;
                    }
                    return false;
                });
        }
    };
}

// LLVM entry point za plugin (obavezno)
extern "C"
LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getDCEPluginInfo();
}

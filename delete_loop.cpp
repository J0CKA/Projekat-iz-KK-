/*Ovaj pass traži petlje koje:
-nemaju bočne efekte (ne menjaju memoriju / ne rade I/O / ne zovu funkcije)
-ne koriste se van petlje
-imaju poznat broj iteracija*/

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

namespace {// pravimo anonimni namespace da se klase ne sudaraju sa drugim pass-evima

struct BrisanjeMrtvihPetljiPass
    : public PassInfoMixin<BrisanjeMrtvihPetljiPass> { //definicija LLVM pass-a

    bool ImaBocneEfekte(Loop *Petlja) { // provera da li petlja sadrzi bocne efekte
        for (BasicBlock *BB : Petlja->blocks()) { //prolazimo kroz sve basic blokove u ovoj petlji
            for (Instruction &Inst : *BB) { //prolazimo kroz sve instrukcije u svakom bloku

                if (Inst.mayHaveSideEffects()) // ako instrukcija ima efekte -> NE SME SE BRISATI
                    return true;

                if (isa<CallBase>(&Inst)) //ako je funkcijski poziv -> POTENCIJALNI BOCNI EFEKAT
                    return true;
            }
        }

        return false; // petlja je CISTA 
    }

    bool KoristiSeVanPetlje(Loop *Petlja) { // Sakupimo sve legitimne izlazne blokove ove petlje
        SmallVector<BasicBlock *, 4> IzlazniBlokovi;
        Petlja->getExitBlocks(IzlazniBlokovi); //uzimamo samo blokove gde se izlazi iz petlje

        for (BasicBlock *BB : Petlja->blocks()) {
            for (Instruction &Inst : *BB) {
                for (User *U : Inst.users()) { //gledamo ko koristi rezultat instrukcije
                    if (auto *KorisnikInst = dyn_cast<Instruction>(U)) { //proveravamo da li je korisnik instrukcija
                        
                        // Ako je korisnik van petlje
                        if (!Petlja->contains(KorisnikInst)) {
                            
                            // Ako je taj korisnik PHI čvor u nekom od izlaznih blokova petlje,
                            // to je bezbedno za brisanje jer LLVM to ume automatski da sredi!
                            if (auto *PN = dyn_cast<PHINode>(KorisnikInst)) {
                                bool JeIzlazniPHI = false;
                                for (BasicBlock *EB : IzlazniBlokovi) { 
                                    if (PN->getParent() == EB) {
                                        JeIzlazniPHI = true; //izlazni PHI se ne računa kao “problem” -> IGNORISE SE
                                        break;
                                    }
                                }
                                if (JeIzlazniPHI) {
                                    continue; // Preskačemo, ovo nam ne blokira brisanje petlje
                                }
                            }
                            
                            // U svakom drugom slučaju, vrednost stvarno beži van i NE SMEMO BRISATI
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) { //ovo llvm poziva za svaku funkciju

        LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
        // LI -> gde su petlje

        ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
        // SE -> koliko puta se izvršavaju

        DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
        // DT -> dominacija blokova (za bezbedno brisanje)

        bool Izmenjeno = false;

        std::vector<Loop *> MrtvePetlje; // lista petlji koje će biti obrisane

        for (Loop *Petlja : LI) {// sve petlje u funkciji

            if (!Petlja->getSubLoops().empty())
                continue; // preskacemo ugnjezdene petlje -- samo najunutrasnjije obradjujemo

            const SCEV *BrojIteracija =
                SE.getBackedgeTakenCount(Petlja); // racunamo broj iteracija petlje

            if (isa<SCEVCouldNotCompute>(BrojIteracija))
                continue; // ako ne moze da izracuna -> NE DIRAJ PETLJU

            if (ImaBocneEfekte(Petlja))
                continue; // ako ima bocne efekte -> NE BRISI 

            if (KoristiSeVanPetlje(Petlja))
                continue; // ako rezultat izlazi iz petlje -> NE BRISI

            MrtvePetlje.push_back(Petlja); //ako je mrtva -> KANDIDAT ZA BRISANJE
        }

        for (Loop *Petlja : MrtvePetlje) { // prolazimo kroz mrtve petlje

            // LLVM funkcija koja uklanja petlju iz CFG-a, ažurira dominatore i loop info i uklanja blokove
            deleteDeadLoop(
                Petlja,
                &DT,
                &SE,
                &LI
            );

            Izmenjeno = true; //oznacavamo da je doslo do promene
        }

        if (Izmenjeno)
            return PreservedAnalyses::none(); //ako je nešto obrisano -> analiza više nije validna

        return PreservedAnalyses::all();// ako ništa nije promenjeno -> sve analize ostaju validne
    }
};

} // namespace

llvm::PassPluginLibraryInfo
getBrisanjeMrtvihPetljiPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "loop-deletion",
        LLVM_VERSION_STRING,

        [](PassBuilder &PB) {

            PB.registerPipelineParsingCallback( //omogućava da se pass zove iz CLI-a
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

//LLVM traži ovu funkciju kada učita plugin
extern "C"
LLVM_ATTRIBUTE_WEAK
::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {

    return getBrisanjeMrtvihPetljiPluginInfo(); //vraća definiciju plugina
}

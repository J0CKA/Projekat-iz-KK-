#include "llvm/Pass.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "OurCFG.h"

using namespace llvm;

namespace
{
    // definicija starog LLVM passa koji radi nad pojedinačnim funkcijama
    struct OurDeadCodeElimination : public FunctionPass
    {
        // mapa koja prati da li je neka instrukcija živa - true, mrtva - false
        std::unordered_map<Value *, bool> Variables;
        // mapa koja povezuje LoadInst sa memorijskom lokacijom (pokazivačem) sa koje čita
        std::unordered_map<Value *, Value *> VariablesMap;

        std::vector<Instruction *> InstructionsToRemove; // instrukcije označene za brisanje
        bool InstructionEliminated;                      // da li je obrisana bar 1

        static char ID; // jedinstveni identifikator passa
        OurDeadCodeElimination() : FunctionPass(ID) {}

        // pomoćna funkcija koja označava operanda i njegovu povezanu lokaciju kao žive
        void handleOperand(Value *Operand)
        {
            Variables[Operand] = true;               // živ operand
            Variables[VariablesMap[Operand]] = true; // živa i lokacija
        }

        //funkcija koja je glavna za brisanje mrtvih instrukcija
        void eliminateDeadInstructions(Function &F)
        {
            InstructionsToRemove.clear(); //čisti se lista za brisanje pre nove analize

            for (BasicBlock &BB : F) //prolazak kroz sve bazične blokove
            {
                for (Instruction &I : BB) //prolazak kroz sve instrukcije
                {
                    if (I.getType()->getTypeID() != Type::VoidTyID && !isa<CallInst>(&I)) //ako instrukcija vraća vrednost (nije Void) i nije poziv funkcije - pretpostavljamo da je MRTVA (false)
                    {
                        Variables[&I] = false;
                    }
                    if (isa<LoadInst>(&I)) //ako je instrukcija 'load', mapiramo je sa njenim nultim operandom
                    {
                        VariablesMap[&I] = I.getOperand(0);
                    }

                    if (isa<StoreInst>(&I)) //ako je instrukcija 'store', proveravamo njen nulti operand
                    {
                        if (Variables.find(I.getOperand(0)) != Variables.end()) //ako se vrednost prati, označavamo je kao živu
                        {
                            handleOperand(I.getOperand(0)); 
                        }
                    }
                    else //ya sve ostale instrukcije prolazimo kroz sve njihove operande
                    {
                        for (size_t i = 0; i < I.getNumOperands(); i++)
                        {
                            //ako se operand nalazi u mapi, označavamo ga kao živog
                            if (Variables.find(I.getOperand(i)) != Variables.end()) 
                            {
                                handleOperand(I.getOperand(i)); 
                            }
                        }
                    }
                }
            }
            //DRUGI PROLAZ: Selektovanje instrukcija za brisanje na osnovu sakupljenih informacija o živosti
            for (BasicBlock &BB : F)
            {
                for (Instruction &I : BB)
                {
                    if (isa<StoreInst>(&I))
                    {
                        if (Variables.find(I.getOperand(1)) != Variables.end() && !Variables[I.getOperand(1)])
                        {
                            InstructionsToRemove.push_back(&I);
                        }
                    }
                    else if (Variables.find(&I) != Variables.end() && !Variables[&I])
                    {
                        InstructionsToRemove.push_back(&I);
                    }
                }
            }

            //proveravamo da li ima instrukcija za brisanje
            if (InstructionsToRemove.size() > 0)
            {
                InstructionEliminated = true;
            }

            //brišemo instrukcije
            for (Instruction *Instr : InstructionsToRemove)
            {
                Instr->eraseFromParent();
            }
        }
        //funkcija koja pronalazi i briše bazne blokove do kojih ne stiže
        void eliminateUnreachableInstructions(Function &F)
        {
            std::vector<BasicBlock *> UnreachableBlocks;
            OurCFG *CFG = new OurCFG(F);
            CFG->DFS(&F.front());

            for (BasicBlock &BB : F)
            {
                if (!CFG->isReachable(&BB))
                {
                    UnreachableBlocks.push_back(&BB);
                }
            }

            //proveravamo da li ima nedostižnih blokova
            if (UnreachableBlocks.size() > 0)
            {
                InstructionEliminated = true;
            }
            //brisanje nedostižnih blokova

            for (BasicBlock *UnreachableBlock : UnreachableBlocks)
            {
                UnreachableBlock->eraseFromParent();
            }
        }

        //glavna ulazna tačka passa koja se izvršava nad svakom funkcijom u programu
        //neefikasna -> ima prostora za optimizaciju
        //sve dok ima mrtvih instrukcija ona ispituje ponovo kako bi uklonila i one korišćene, ali korišćene za mrtve
        bool runOnFunction(Function &F) override
        {
            do
            {
                InstructionEliminated = false;
                eliminateDeadInstructions(F);
                eliminateUnreachableInstructions(F);
            } while (InstructionEliminated);
            return true;
        }
    };
}

char OurDeadCodeElimination::ID = 0; //inicijalizacija ID-ja passa
//registracija passa kod LLVM-a kako bi mogao da se pozove preko 'opt'
static RegisterPass<OurDeadCodeElimination> X("our-constant-folding", "OurDeadCodeElimination pass", false, false);

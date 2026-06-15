//Cilj - da pronađe i ukloni instrukcije upisa u memoriju (store) čija se vrednost nikada kasnije ne učita (load) 
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

struct DSEPass : public PassInfoMixin<DSEPass> { //definicija DSE optimizacionog passa
                                                 //PassInfoMixin - šablon koji struktura nasleđuje

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) { //glavna funkcija koja se izvršava nad svakom funkcijom
        bool promena = false; //oznacava da li je program modifikovan
        
        //preko unordered mape se za svaki basic block čuva skup živih memorijskih lokacija
        std::unordered_map<BasicBlock*, std::unordered_set<Value*>> ziveNaUlazu;  //mem lokacije koje moraju biti žive na ulazu u blok
        std::unordered_map<BasicBlock*, std::unordered_set<Value*>> ziveNaIzlazu; //mem lokacije koje moraju biti žive na izlazu iz bloka
        
        std::vector<BasicBlock*> radnaLista; //lista BB koji treba da se analiziraju
        std::unordered_set<BasicBlock*> zaObradu; //skup koji čuva sve različite BB, kako ne bismo ubacivali u radnulistu BB više puta istovremeno

        for (BasicBlock &BB : llvm::reverse(F)) { //analiza živosti se radi unazad - od kraja funkcije do početka
            radnaLista.push_back(&BB);  //dodaje blok u radnu listu
            zaObradu.insert(&BB);
        }
        std::reverse(radnaLista.begin(), radnaLista.end());

        //analiziranje BB
        while (!radnaLista.empty()) { //za svaki BB iz radne liste
            BasicBlock *BB = radnaLista.back(); //uzima poslednji
            radnaLista.pop_back();  //skida ga sa liste
            zaObradu.erase(BB);  //brise ga i iz skupa

            //računa one koje žive na izlazu datog BB prroveravajući koje žive na ulazu svih naslednika datog BB
            std::unordered_set<Value*> novoStanjeIzlaza;
            for (BasicBlock *suc : successors(BB)) { //daje sve BB koji se izvrsavaju nakon datog
                auto It = ziveNaUlazu.find(suc); //skup svih koje žive na ulazu naslednika trenutnog BB
                if (It != ziveNaUlazu.end()) {  //ako je potrebna nasledniku -> ziva je
                    novoStanjeIzlaza.insert(It->second.begin(), It->second.end());
                }
            }
            ziveNaIzlazu[BB] = novoStanjeIzlaza; //smešta ih u ziveNaIzlazu

            //na osnovu onih koje žive na izlazu datog BB, formira se skup onih koji žive na ulazu datog bloka
            std::unordered_set<Value*> tekucaZivost = novoStanjeIzlaza;
            for (auto I = BB->rbegin(); I != BB->rend(); ++I) {  //krećemo se unazad
                Instruction &Inst = *I; //uzima svaku instrukciju

                if (auto *LI = dyn_cast<LoadInst>(&Inst)) { //ako je instrukcija load - koristi se pa je dodaje u skup živih 
                    tekucaZivost.insert(LI->getPointerOperand());
                } 
                else if (auto *SI = dyn_cast<StoreInst>(&Inst)) {//ako je instrukcija store - lokacija se definiše pa više nije živa PRE ovog upisa
                    tekucaZivost.erase(SI->getPointerOperand());
                } 
                else if (auto *CI = dyn_cast<CallInst>(&Inst)) {//ako je instrukcija poziv funkcije
                    if (!CI->onlyReadsMemory()) {//ako funkcija menja memoriju
                        tekucaZivost.clear(); //pretpostavljamo da prepisuje sve lokacije (ubija njihovu živost unazad)
                    }
                }
            }

            if (ziveNaUlazu[BB] != tekucaZivost) { //ako se skup živih promenio to utiče i na njegove prethodnike
                ziveNaUlazu[BB] = tekucaZivost;

                //vraća se na prethodnike radi ponovne obrade
                for (BasicBlock *pred : predecessors(BB)) { 
                    if (zaObradu.find(pred) == zaObradu.end()) {
                        radnaLista.push_back(pred);
                        zaObradu.insert(pred);
                    }
                }
            }
        }

        for (BasicBlock &BB : F) {
            std::unordered_set<Value*> zivaMemorija = ziveNaIzlazu[&BB]; //trenutno žive memorijske lokacije na izlazu iz bloka
            std::vector<StoreInst*> zaBrisanje; //store instrukcije - za uklanjanje

            for (auto I = BB.rbegin(); I != BB.rend(); ++I) {
                Instruction &Inst = *I;

                if (auto *LI = dyn_cast<LoadInst>(&Inst)) { //load instrukcija
                    zivaMemorija.insert(LI->getPointerOperand()); //mem lok postaje ziva
                    continue;
                }

                if (auto *SI = dyn_cast<StoreInst>(&Inst)) { //store instrukcija
                    Value *Ptr = SI->getPointerOperand();

                    if (zivaMemorija.find(Ptr) == zivaMemorija.end()) { //ako se vrednost nikada kasnije ne čita, store je mrtav
                        zaBrisanje.push_back(SI);
                    } else {
                        zivaMemorija.erase(Ptr);
                    }
                    continue;
                }

                if (auto *CI = dyn_cast<CallInst>(&Inst)) { //call instrukcija - potencijalno prepisuje svu memoriju => svi store pre njega koji nisu pročitani postaju bezbedni za brisanje
                    if (!CI->onlyReadsMemory()) {
                        zivaMemorija.clear();
                    }
                }
            }

            for (StoreInst *SI : zaBrisanje) {
                SI->eraseFromParent(); //brisanje instrukcije
                promena = true; //promena u kodu se desila
            }
        }
        //ako je kod izmenjen - nijedna analiza nije očuvana, inače su sve prethodne analize i dalje važeće
        return (promena ? PreservedAnalyses::none() : PreservedAnalyses::all());
    }
};

}
//informacije potrebne za registraciju plugina
llvm::PassPluginLibraryInfo getDSEPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "dse", LLVM_VERSION_STRING, //metapodaci
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback( //registruje pass pod imenom dse
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "dse") {    //pokreće se kada korisnik navede -passes=dse
                        FPM.addPass(DSEPass()); //dodaje DSE pass u pipeline optimizaciju
                        return true;
                    }
                    return false;
                });
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return getDSEPluginInfo();  //ulazna tačka plugina koju LLVM učitava
}

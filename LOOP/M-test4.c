#include <stdio.h>

// Ova petlja ne radi ništa korisno, vrti se tačno 100 puta i računa u prazno.
int idealna_mrtva_petlja() {
    int privremena_suma = 0;

    for (int i = 0; i < 100; i++) {
        int add1 = privremena_suma + 5;
        int mul1 = add1 * 2;
        privremena_suma = mul1 - i;
    }
    return 42;  //vraca const
}

//menja memoriju
void petlja_sa_store_efektom(int niz) {
    for (int i = 0; i < 50; i++) {
        niz[i] = 10;
    }
}

//ne brise se
int petlja_gde_vrednost_bezi_van() {
    int koristan_rezultat = 0;

    for (int i = 0; i < 10; i++) {
        koristan_rezultat += 3;
    }

    int izlaz = koristan_rezultat + 5; 
    return izlaz;
}

// Unutrašnja petlja je mrtva i nema podpetlje -> tvoj pass je briše.
// Spoljašnja ima podpetlju, pa je pass preskače zbog getSubLoops().empty() provere.
void ugnjezdene_petlje() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 20; j++) {
            int mrtav_racun = j + 2;
        }
    }
} 

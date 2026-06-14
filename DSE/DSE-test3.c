#include <stdbool.h>

int main_test(bool cond1, bool mode_cond2) {
    int x;
    x = 1; //mrtva

    if (cond1) {
        x = 2; //živa
    } else {
        x = 3; //živa
          
        if (mode_cond2) {
            x = 4; //živa 
        }
    }

    int v = x; 
    return v;  
}

#include <stdbool.h>

int main_test(bool cond1, bool mode_cond2) {
    // entry:
    int x;
    x = 1; // DEAD STORE 

    if (cond1) {
        // bb1:
        x = 2; // ALIVE 
    } else {
        // bb2:
        x = 3; // ALIVE
          
        if (mode_cond2) {
            // bb3:
            x = 4; // ALIVE 
        }
    }

    // merge:
    int v = x; 
    return v;  
}

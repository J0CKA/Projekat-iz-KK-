#include <stdbool.h>

int main_test(bool cond) {
    int x;
    x = 5;   //mrtav
    
    if (cond) {
        // then:
        x = 10;  //živ ako je uslov tačan
    } else {
        x = 20;  //živ ako je uslov netačan
    }

    int v = x; 
    return v;
}

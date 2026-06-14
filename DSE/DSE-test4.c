#include <stdio.h>

int primer(int n, int limit) {
    int sum = 0;
    int i = 0;

    while (i < n) {
        sum = sum + 5;      // ŽIV UPIS! 

        if (sum > limit) {
            sum = sum + 10; // ŽIV UPIS!
        } else {
            i = i + 1;      // ŽIV UPIS! 
        }

        i = i + 1;          // ŽIV UPIS! 
    }

    return sum;             // ŽIVO ČITANJE! 
}

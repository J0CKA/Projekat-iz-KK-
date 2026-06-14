//sve su žive
#include <stdio.h>

int primer(int n, int limit) {
    int sum = 0;
    int i = 0;

    while (i < n) {
        sum = sum + 5;

        if (sum > limit) {
            sum = sum + 10; 
        } else {
            i = i + 1;    
        }

        i = i + 1;        
    }

    return sum;         
}

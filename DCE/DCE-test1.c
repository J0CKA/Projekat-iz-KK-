#include <stdio.h>

int main() {
    goto live;

dead:
    return 0;   // DEAD BLOCK

live:
    return 1;
}

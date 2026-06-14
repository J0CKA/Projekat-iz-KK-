#include <stdio.h>

volatile int sink;

void napredni_test(int *p, int *q, int uslov)
{
    *p = 100;      //mrtva

    *q = 5;        //živa

    if (uslov)
        sink = *q;

    *p = 200;
}

int main(void)
{
    int a = 0;
    int b = 0;

    napredni_test(&a, &b, 1);

    return 0;
}

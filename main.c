#include "polynomial.c"
#include <stdio.h>

int main() {
    Poly /* P = polyIrrGlouton(3);
    printf("\n");
    polyPrint(P);
    polyFree(P);
    printf("\n\n"); */

    P = polyIrrRabin(3, p);
    printf("\n");
    polyPrint(P);
    polyFree(P);
    printf("\n");

    return 0;
}
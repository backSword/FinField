#include "polynomial.c"
#include <stdio.h>

int main() {
    num a[9] = {1};
   // num b[2] = {1};
    Poly P = polyFromArray(a, 8);
    //Poly D = polyFromArray(b, 1);
    //Poly R, Q;
   // R = polyCopy(P);
    polyPrint(P);
    int* j = malloc(9);
    printf("%d\n", *j);
    return 0;
    /* polyEuclid(P, D, &R, &Q);
    polyPrint(P);
    printf(" = (");
    polyPrint(D);
    printf(" ) x (");
    polyPrint(Q);
    printf(") + ");
    polyPrint(R);
    printf("\n"); */
    /* Poly P = polyIrrGlouton(3);
    printf("\n");
    polyPrint(P);
    polyFree(P);
    printf("\n\n");

    P = polyIrrRabin(3, p);
    printf("\n");
    polyPrint(P);
    polyFree(P);
    printf("\n"); */

    return 0;
}
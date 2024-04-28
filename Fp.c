// static
#include <stdint.h>

// generated
#define p 3
typedef uint8_t Fp;

// static
#include <stdbool.h>
#include <stdio.h>
void extendedEuclid(int A, int B, int* x, int* y) {                          
    if (B == 0) {
        *x = 1;
        *y = 0;
    } else {
        extendedEuclid(B, A%B, x, y);
        int temp = *x;
        *x = *y;
        *y = temp - (A/B) * *y;
    }
}
int inversemodp(int A, int P) {
    int x, y;
    extendedEuclid(A, P, &x, &y);
    return x % P;
}

void Fpprint(int x) {
    printf("%d", x);
}
Fp Fpzero = 0;
Fp Fpone = 1;
bool Fpequal(int x, int y) {
    return x == y;
}
Fp Fpsum(int x, int y) {
    return (x + y) % p;
}
Fp Fpprod(int x, int y) {
    return (x * y) % p;
}
Fp Fpdiv(int x, int y) {
    return Fpprod(x, inversemodp(y, p));
}
Fp Fpinv(int x) {
    return inversemodp(x, p);
}
Fp Fpminus(int x) {
    return p - x;
}
Fp Fpnext(int x) {
    return (x + 1) % p;
}
// static
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "mathext.c"

// generated
#define p 3
typedef uint8_t Fp;

// static

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
Fp Fpsucc(int x) {
    return (x + 1) % p;
}
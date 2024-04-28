#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t uint;

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
int inversemodp(int A, int p) {
    int x, y;
    extendedEuclid(A, p, &x, &y);
    while (x < 0)
        x += p;
    return x % p;
}

uint uint_pow(uint base, uint exp)
{
    uint result = 1;
    while (exp)
    {
        if (exp % 2)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

typedef struct s_uintLL {
    uint val;
    struct s_uintLL* next;
} uintLL;
bool isPrime(int n, uintLL* primes) {
    while (primes != NULL) {
        if (n % primes->val)
            return false;
        primes = primes->next;
    }

    return true;
}
void primeFactorisation(uint n, uint factors[]) {
    uintLL* primes = NULL,
        * endPrimes = NULL;
    int j = 0;

    for (int i = 2; i < sqrt(n); i ++)
        if (isPrime(i, primes)) {
            uintLL* l = malloc(sizeof(uintLL));
            l->val = i;
            l->next = NULL;
            if (endPrimes == NULL)
                primes = l;
            else
                endPrimes->next = l;
            endPrimes = l;

            if (n % i == 0) {
                factors[j] = i;
                j ++;
                n /= i;
                i --; // make sure we retry the same number, in case it divides n multiple times
            }
        }
    factors[j] = n;
    factors[j + 1] = 0;

    while (primes != NULL) {
        uintLL* p = primes;
        primes = p->next;
        free(p);
    }
}
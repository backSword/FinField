#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include <polynomial.c>

#define p 2
#define n 3
#define q 8

typedef struct s_num {
    Poly P;
} num;
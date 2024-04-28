#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
// #include "mathext.c"
#define max(a, b) (a < b ? b : a)

// !! deg (0) = 0

// generated area
#include "Fp.c"
typedef Fp num;
#define printnum Fpprint
#define zero Fpzero
#define one Fpone
#define equal Fpequal
#define sum Fpsum
#define prod Fpprod
#define inv Fpinv
#define minus Fpminus
#define succ Fpsucc

typedef struct s_Poly {
    uint deg;
    uint* coeff;
} Poly;

void polyFree(Poly P) {
    free(P.coeff);
}

bool polyEqual(const Poly P, const Poly Q) {
    if (P.deg != Q.deg)
        return false;
    for (uint i = 0; i < P.deg; i ++)
        if (!equal(P.coeff[i], Q.coeff[i]))
            return false;
    return true;
}

bool polyConst(Poly P, num x) {
    return P.deg == 0 && equal(P.coeff[0], x);
}

void polyPrint(Poly P) {
    for (uint i = P.deg; i > 0; i --) {
        if (!equal(P.coeff[i], zero)) {
            if (!equal(P.coeff[i], one))
                printnum(P.coeff[i]);
            if (i == 1)
                printf("X ");
            else
                printf("X^%d ", i);
        }
        if (P.coeff[i - 1] != 0)
            printf("+ ");
    }
    if (!equal(P.coeff[0], zero)) 
        printnum(P.coeff[0]);
    if (polyConst(P, zero))
        printnum(0);
}

Poly polyXPower(uint n) {
    Poly R;
    R.deg = n;
    R.coeff = malloc((n+1)*sizeof(num));
    for (uint i = 0; i < n; i ++)
        R.coeff[i] = zero;
    R.coeff[n] = one;
    return R;
}

num safeCoeff(const Poly P, uint i) {
    return i > P.deg? 0 : P.coeff[i];
}

Poly polyScale(const Poly P, num x) {
    Poly R;
    R.deg = P.deg;
    R.coeff = malloc((R.deg+1)*sizeof(num));
    for (uint i = 0; i <= P.deg; i ++)
        R.coeff[i] = prod(P.coeff[i], x);
    return R;
}

// NB : fait plus de trvail car retire les potentiels 0 devant le polynome
Poly polySum(const Poly P, const Poly Q) {
    Poly R;
    uint i = max(P.deg, Q.deg) + 1;
    num cr;
    do {
        i --;
        cr = sum(safeCoeff(P, i), safeCoeff(Q, i));
    } while (i != 0 && equal(cr, zero));
    R.deg = i;
    R.coeff = malloc((i+1)*sizeof(num));
    R.coeff[i] = cr;
    i --;
    while (i != -1) { 
        R.coeff[i] = sum(safeCoeff(P, i), safeCoeff(Q, i)); 
        i --;
    }
    return R;
}

Poly polySub(const Poly P, const Poly Q) {
    return polySum(P, polyScale(Q, minus(one)));
}

Poly polyProd(const Poly P, const Poly Q) {
    Poly R;
    R.deg = P.deg + Q.deg;
    R.coeff = malloc((R.deg+1)*sizeof(num));
    for (uint i = 0; i <= P.deg; i++)
        for (uint j = 0; j <= Q.deg; j++)
            R.coeff[i + j] = sum(R.coeff[i + j], prod(P.coeff[i], Q.coeff[j]));
    return R;
}

Poly polyUnit(const Poly P) {
    assert(!polyConst(P, zero));
    return polyScale(P, inv(P.coeff[P.deg]));
}

Poly polyCopy(const Poly P) {
    Poly R;
    R.deg = P.deg;
    printf("copy deg %d\n", P.deg);
    R.coeff = malloc((R.deg+1)*sizeof(num));
    for (uint i = 0; i <= P.deg; i ++)
        R.coeff[i] = P.coeff[i];
    return R;
}

void polyReplace(Poly* P, Poly Q) {
    free(P->coeff);
    *P = Q;
}

void polyShiftX(Poly P, uint n) {
    Poly R;
    R.deg = P.deg + n;
    R.coeff = malloc((R.deg+1)*sizeof(num));
    for (uint i = 0; i < n; i++)
        R.coeff[i] = 0;
    for (uint i = 0; i <= P.deg; i++)
        R.coeff[i + n] = P.coeff[i];
    polyFree(P);
    P = R;
}

void polyEuclid(const Poly P, const Poly D, Poly* R, Poly* Q) {
    printf("euclid\n");
    if (D.deg == 0) {
        assert(D.coeff[0] != 0);
        R->deg = 0;
        R->coeff = malloc(sizeof(num));
        R->coeff[0] = zero;
        *Q = polyScale(P, inv(D.coeff[0]));
        return;
    } 
    polyPrint(P);
    printf("euclid mid\n");
    *R = polyCopy(P);
    if (P.deg < D.deg) {
        Q->deg = 0;
        printf("a\n");
        Q->coeff = malloc(sizeof(num));
        Q->coeff[0] = 0;
        printf("b\n");
        return;
    }
    printf("euclid later\n");
    Q->deg = P.deg - D.deg;
    Q->coeff = malloc((Q->deg+1)*sizeof(num));
    Poly S;
    while (R->deg >= D.deg) {
        num l = prod(R->coeff[R->deg], inv(D.coeff[D.deg]));
        uint deg = R->deg - D.deg;
        polyReplace(&S, polyScale(D, l));
        polyShiftX(S, deg);
        polyReplace(R, polySub(*R, S));
        Q->coeff[deg] = l;
    }
    polyFree(S);
}

bool polyDiv(const Poly D, const Poly P) {
    if (D.deg == 0)
        return true;
    printf("start div\n");
    Poly R, Q;
    polyEuclid(P, D, &R, &Q);
    polyPrint(R);
    printf("\n");
    bool b = polyConst(R, zero);
    polyFree(R);
    polyFree(Q);
    printf("end div\n");
    return b;
}

Poly polyGcd(const Poly A, const Poly B) {
    printf("polyGcd : A = ");
    polyPrint(A);
    printf(", B = ");
    polyPrint(B);
    printf("\n");
    if (A.deg < B.deg)
        return polyGcd(B, A);
    if (polyConst(B, zero))
        return polyUnit(A);

    Poly Q, R;
    polyEuclid(A, B, &R, &Q);
    polyFree(Q);
    Poly G = polyGcd(B, R);
    polyFree(R);
    printf("polyGcdEnd : G = ");
    polyPrint(G);
    printf("\n");
    return G;
}

Poly polyFromArray(const num c[], uint deg) {
    Poly R;
    R.deg = deg;
    printf("%ld\n", (deg+1)*sizeof(num));
    R.coeff = malloc((deg+1)*sizeof(num));
    for (uint i = 0; i <= deg; i++)
        R.coeff[i] = c[i];
    return R;
}

num polyEval(const Poly P, num x) {
    num y = P.coeff[P.deg];
    for (uint i = P.deg - 1; i != -1; i --)
        y = sum(P.coeff[i], prod(y, x));
    return y;
}

typedef struct s_PolyLL {
    Poly val;
    struct s_PolyLL* next;
} PolyLL;

bool polyIrrGloutonAux(const Poly P, PolyLL* irr) {
    if (equal(polyEval(P, zero), zero))
        return false;
    for (num x = one; !equal(x, zero); x = succ(x)) 
        if (equal(polyEval(P, x), zero))
            return false;

    if (P.deg <= 3)
        return true;

    while (irr != NULL) {
        if (irr->val.deg <= P.deg / 2) {
            printf("  ");polyPrint(irr->val); printf("\n");
            if (polyDiv(irr->val, P))
                return false;
        }
        irr = irr->next;
    }

    return true;
}
Poly polyIrrGlouton(uint n) {
    PolyLL* irr = NULL,
        * irrEnd = NULL;
    Poly P;
    P.coeff = malloc((n+1)*sizeof(num));

    printf("building irr list...\n");
    for (uint i = 2; i < n; i++) {
        P.deg = i;
        P.coeff[i] = one;
        while (equal(P.coeff[i], one)) { // fait tous les polynomes unitaires de deg i
            polyPrint(P);
            if (polyIrrGloutonAux(P, irr)) {
                PolyLL* l = malloc(sizeof(PolyLL));
                l->val = polyCopy(P);
                l->next = NULL;
                if (irrEnd == NULL)
                    irr = l;
                else
                    irrEnd->next = l;
                irrEnd = l;
                printf(" irr found");
            }
            printf("\n");

            uint j = -1;
            do { 
                j ++;
                P.coeff[j] = succ(P.coeff[j]);
            } while (j < i && equal(P.coeff[j], zero));
        }
        P.coeff[i] = zero;
    }

    printf("searching for irr of deg %d...\n", n);
    P.deg = n;
    P.coeff[n] = one;
    while (equal(P.coeff[n], one)) {
        polyPrint(P);
        printf("\n");
        if (polyIrrGloutonAux(P, irr)) 
            return P;

        uint j = -1;
        do {
            j ++;
            P.coeff[j] = succ(P.coeff[j]);
        } while (j < n && equal(P.coeff[j], zero));
    }

    while (irr != NULL) {
        PolyLL* l = irr->next;
        free(irr);
        irr = l;
    }
    printf("done\n");
    return P;
}

bool polyIrrRabinAux(Poly P, uint D, uint q, uint s[]) {
    Poly Q = polyXPower(uint_pow(q, D));
    Q.coeff[1] = minus(one);

    printf("> ");
    polyPrint(Q);
    printf("\n");

    if (!polyDiv(P, Q))
        return false;

    for (int i = 0; s[i] != 0; i ++) {
        Q.deg = uint_pow(q, D / s[i]);
        Q.coeff[Q.deg] = 1;
        
        printf("< ");
        polyPrint(Q);
        printf("\n");

        if (!polyConst(polyGcd(P, Q), one))
            return false;
    }

    polyFree(Q);
    return true;
}
Poly polyIrrRabin(uint d, uint q) {
    uint s[(int) (2*log(log(d)) + .5)];
    primeFactorisation(d, s);
    for (int i = 0; s[i] != 0; i ++) 
        printf("%d, ", s[i]);
    printf("\n");

    Poly P;
    P.deg = d;
    P.coeff = malloc((d+1)*sizeof(num));
    P.coeff[d] = one;

    while (equal(P.coeff[d], one)) {
        polyPrint(P);
        printf("\n");
        if (polyIrrRabinAux(P, d, q, s)) 
            return P;

        uint j = -1;
        do {
            j ++;
            P.coeff[j] = succ(P.coeff[j]);
        } while (j < d && equal(P.coeff[j], zero));
    }

    printf("done\n");
    return P;
}
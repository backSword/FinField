#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
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
#define div Fpdiv
#define inv Fpinv
#define minus Fpminus
#define next Fpnext

typedef struct s_Poly {
    int deg;
    int* coeff;
} Poly;

void polyFree(Poly P) {
    free(P.coeff);
}

void polyPrint(Poly P) {
    for (int i = P.deg; i > 0; i --) {
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
}

bool polyEqual(const Poly P, const Poly q) {
    if (P.deg != q.deg)
        return false;
    for (int i = 0; i < P.deg; i ++)
        if (!equal(P.coeff[i], q.coeff[i]))
            return false;
    return true;
}

num safeCoeff(const Poly P, int i) {
    assert(i >= 0);
    return i > P.deg? 0 : P.coeff[i];
}

Poly polyScale(const Poly P, num x) {
    Poly r;
    r.deg = P.deg;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i <= P.deg; i ++)
        r.coeff[i] = prod(P.coeff[i], x);
    return r;
}

// NB : fait plus de trvail car retire les potentiels 0 devant le polynome
Poly polySum(const Poly P, const Poly q) {
    Poly r;
    int i = max(P.deg, q.deg) + 1;
    num cr;
    do {
        i --;
        cr = sum(safeCoeff(P, i), safeCoeff(q, i));
    } while (i != 0 && equal(cr, zero));
    r.deg = i;
    r.coeff = malloc((i+1)*sizeof(num));
    while (i >= 0) { 
        r.coeff[i] = sum(safeCoeff(P, i), safeCoeff(q, i)); 
        i --;
    }
    return r;
}

Poly polySub(const Poly P, const Poly q) {
    return polySum(P, polyScale(q, minus(one)));
}

Poly polyProd(const Poly P, const Poly q) {
    Poly r;
    r.deg = P.deg + q.deg;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i <= P.deg; i++)
        for (int j = 0; j <= q.deg; j++)
            r.coeff[i + j] = sum(r.coeff[i + j], prod(P.coeff[i], q.coeff[j]));
    return r;
}

Poly polyCopy(const Poly P) {
    Poly r;
    r.deg = P.deg;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i <= P.deg; i ++)
        r.coeff[i] = P.coeff[i];
    return r;
}

// this algorithm modifies the polynomial for maximal speed
void polyDivX(Poly P) {
    assert(P.deg > 0);
    P.deg --;
    P.coeff ++;
}

Poly polyShiftX(const Poly P, int n) {
    Poly r;
    r.deg = P.deg + n;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i < n; i++)
        r.coeff[i] = 0;
    for (int i = 0; i <= P.deg; i++)
        r.coeff[i + n] = P.coeff[i];
    return r;
}

void polyEuclid(const Poly P, const Poly d, Poly* r, Poly* q) {
    assert(d.deg != 0);
    q->deg = P.deg / d.deg;
    q->coeff = malloc((q->deg+1) * sizeof(num));
    *r = polyCopy(P);
    Poly s;
    while (r->deg >= d.deg) {
        num l = div(r->coeff[r->deg], d.coeff[d.deg]);
        int deg = r->deg - d.deg;
        s = polyShiftX(polyScale(d, l), deg);
        *r = polySub(*r, s);
        q->coeff[deg] = l;
    }
    polyFree(s);
}

bool polyDiv(const Poly d, const Poly P) {
    if (d.deg == 0)
        return true;
    Poly r, q;
    polyEuclid(P, d, &r, &q);
    bool b = true;
    for (int i = 0; i < r.deg; i ++)
        if (!equal(r.coeff[i], zero))
            b = false;
    polyFree(r);
    polyFree(q);
    return b;
}

Poly polyFromArray(const num c[], int deg) {
    Poly r;
    r.deg = deg;
    r.coeff = malloc((deg+1)*sizeof(num));
    for (int i = 0; i <= deg; i++)
        r.coeff[i] = c[i];
    return r;
}

num polyEval(const Poly P, num x) {
    num y = P.coeff[P.deg];
    for (int i = P.deg - 1; i >= 0; i --)
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
    for (num x = one; !equal(x, zero); x = next(x)) 
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
Poly polyIrrGlouton(int n) {
    PolyLL* irr = NULL;
    PolyLL* irrEnd = NULL;
    Poly P;
    P.coeff = malloc((n+1)*sizeof(num));

    printf("building irr list...\n");
    for (int i = 2; i < n; i++) {
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

            int j = -1;
            do { 
                j ++;
                P.coeff[j] = next(P.coeff[j]);
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

        int j = -1;
        do {
            j ++;
            P.coeff[j] = next(P.coeff[j]);
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
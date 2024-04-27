#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

// !! deg (0) = 0

void extendedEuclid(int A, int B, int* x, int* y)
{                          
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
    return x % p;
}


// generated area
#define P 3
typedef int num;
#define printnum(x) (printf("%d", x))
#define zero 0
#define one 1
#define equal(x, y) (x == y)
#define sum(x, y) ((x + y) % P)
#define prod(x, y) ((x * y) % P)
#define div(x, y) (prod(x, inversemodp(y, P)))
#define inv(x) (inversemodp(x, P))
#define minus(x) (P - x)
#define next(x) ((x + 1) % P)
#define max(a, b) (a < b ? b : a)

typedef struct s_Poly {
    int deg;
    int* coeff;
} Poly;

void polyFree(Poly p) {
    free(p.coeff);
}

void polyPrint(Poly p) {
    for (int i = p.deg; i > 0; i --) {
        if (!equal(p.coeff[i], zero)) {
            if (!equal(p.coeff[i], one))
                printnum(p.coeff[i]);
            if (i == 1)
                printf("X ");
            else
                printf("X^%d ", i);
        }
        if (p.coeff[i - 1] != 0)
            printf("+ ");
    }
    if (!equal(p.coeff[0], zero)) 
        printnum(p.coeff[0]);
}

bool polyEqual(const Poly p, const Poly q) {
    if (p.deg != q.deg)
        return false;
    for (int i = 0; i < p.deg; i ++)
        if (!equal(p.coeff[i], q.coeff[i]))
            return false;
    return true;
}

num safeCoeff(const Poly p, int i) {
    assert(i >= 0);
    return i > p.deg? 0 : p.coeff[i];
}

Poly polyScale(const Poly p, num x) {
    Poly r;
    r.deg = p.deg;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i <= p.deg; i ++)
        r.coeff[i] = prod(p.coeff[i], x);
    return r;
}

// NB : fait plus de trvail car retire les potentiels 0 devant le polynome
Poly polySum(const Poly p, const Poly q) {
    Poly r;
    int i = max(p.deg, q.deg) + 1;
    num cr;
    do {
        i --;
        cr = sum(safeCoeff(p, i), safeCoeff(q, i));
    } while (i != 0 && equal(cr, zero));
    r.deg = i;
    r.coeff = malloc((i+1)*sizeof(num));
    while (i >= 0) { 
        r.coeff[i] = sum(safeCoeff(p, i), safeCoeff(q, i)); 
        i --;
    }
    return r;
}

Poly polySub(const Poly p, const Poly q) {
    return polySum(p, polyScale(q, minus(one)));
}

Poly polyProd(const Poly p, const Poly q) {
    Poly r;
    r.deg = p.deg + q.deg;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i <= p.deg; i++)
        for (int j = 0; j <= q.deg; j++)
            r.coeff[i + j] = sum(r.coeff[i + j], prod(p.coeff[i], q.coeff[j]));
    return r;
}

Poly polyCopy(const Poly p) {
    Poly r;
    r.deg = p.deg;
    r.coeff = malloc((p.deg+1)*sizeof(num));
    for (int i = 0; i < p.deg; i ++)
        r.coeff[i] = p.coeff[i];
    return r;
}

// this algorithm modifies the polynomial for maximal speed
void polyDivX(Poly p) {
    assert(p.deg > 0);
    p.deg --;
    p.coeff ++;
}

Poly polyShiftX(const Poly p, int n) {
    Poly r;
    r.deg = p.deg + n;
    r.coeff = malloc((r.deg+1)*sizeof(num));
    for (int i = 0; i < n; i++)
        r.coeff[i] = 0;
    for (int i = 0; i <= p.deg; i++)
        r.coeff[i + n] = p.coeff[i];
    return r;
}

void polyEuclid(const Poly p, const Poly d, Poly* r, Poly* q) {
    q->deg = p.deg / d.deg;
    q->coeff = malloc((q->deg+1) * sizeof(num));
    *r = polyCopy(p);
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

bool polyDiv(const Poly d, const Poly p) {
    Poly r, q;
    polyEuclid(p, d, &r, &q);
    bool b = true;
    for (int i = 0; i < r.deg; i ++)
        if (!equal(r.coeff[i], zero))
            b = false;
    polyFree(r);
    polyFree(q);
    return b;
}

num eval(const Poly p, num x) {
    num y = p.coeff[p.deg];
    for (int i = p.deg - 1; i >= 0; i --)
        y = sum(p.coeff[i], prod(y, x));
    return y;
}


typedef struct s_PolyLL {
    Poly val;
    struct s_PolyLL* next;
} PolyLL;


bool polyIrrGloutonAux(const Poly p, PolyLL* irr) {
    if (equal(eval(p, zero), zero))
        return false;
    for (num x = one; !equal(x, zero); x = next(x)) 
        if (equal(eval(p, x), zero))
            return false;

    if (p.deg <= 3)
        return true;
    
    polyPrint(p);printf("\n");

    while (irr != NULL) {
        if (irr->val.deg < p.deg && polyDiv(irr->val, p))
            return false;
        irr = irr->next;
    }

    return true;
}
Poly polyIrrGlouton(int n) {
    PolyLL* irr = NULL;
    Poly p;
    p.coeff = malloc((n+1)*sizeof(num));

    printf("building irr list...\n");
    for (int i = 2; i < n; i++) {
        p.deg = i;
        p.coeff[i] = one;
        while (equal(p.coeff[i], one)) { // fait tous les polynomes unitaires de deg i
            polyPrint(p);
            if (polyIrrGloutonAux(p, irr)) {
                PolyLL l = {.val = p, .next = irr};
                irr = &l;
                printf(" irr found");
            }
            printf("\n");

            int j = -1;
            do { 
                j ++;
                p.coeff[j] = next(p.coeff[j]);
            } while (j < i && equal(p.coeff[j], zero));
        }
        p.coeff[i] = zero;
    }

    printf("searching for irr of deg %d...\n", n);
    p.deg = n;
    p.coeff[n] = one;
    while (equal(p.coeff[n], one)) {
        polyPrint(p);
        printf("\n");
        if (polyIrrGloutonAux(p, irr)) 
            return p;

        int j = -1;
        do { 
            printf("yay\n");
            j ++;
            p.coeff[j] = next(p.coeff[j]);
        } while (j < n && equal(p.coeff[j], zero));
    }
    p.coeff[n] = zero;

    printf("done\n");
    return p;
}

int main() {
    Poly p = polyIrrGlouton(4);
    polyPrint(p);
    polyFree(p);

    return 0;
}
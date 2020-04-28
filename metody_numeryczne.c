#include <stdio.h>
#include <math.h>
#define g 9.81
#define PATH "."
#define k 2.0

typedef struct{
    float x, y, z;
} cords;

void choice(int*, cords*, cords*, cords*, const int);
void initialize(const int, cords*, cords*, cords*, const int);
void euler(cords*, cords*, cords*, float, float, float, const int);
void verlet(cords*, cords*, cords*, float, float, float, const int);
void vVerlet(cords*, cords*, cords*, float, float, float, const int);
void Fcalc(cords*, cords*);
void energy(cords*, cords*, float*, float*, float*, float);
void oscEnergy(cords*, cords*, float*, float*, float*, float);
void flushf(FILE**, char*);
void stepo(cords *, cords *, cords *, float*, float*, float*, float, int);

int main() {
    int c = 0, method = 0;
    cords v = {0, 0, 0}, F = {0, 0, 0}, r = {0, 0, 0};
    printf("\nMetody numeryczna:");
    printf("\n1. Metoda Eulera");
    printf("\n2. Metoda Verleta");
    printf("\n3. Metoda predkosciowa Verleta");
    printf("\nWybierz metode:");
    scanf("%d", &method);
    while (c == 0) choice(&c, &v, &F, &r, method);
    return 0;
}

void choice(int* c, cords* v, cords* F, cords* r, const int method) {
    printf("\nRuch:");
    printf("\n1. Bez sil zewnetrznych");
    printf("\n2. W polu grawitacyjnym ziemskim");
    printf("\n3. Z dzialajaca podana sila");
    printf("\n4. Zakoncz dzialanie programu");
    printf("\nWybierz rodzaj ruchu:");
    scanf("%d", c);
    switch(*c) {
        case 1:
            (*F).x = 0;
            (*F).y = 0;
            (*F).z = 0;
            initialize(*c, v, F, r, method);
            break;
        case 2:
            (*F).x = 0;
            (*F).z = 0;
            initialize(*c, v, F, r, method);
            break;
        case 3:
            initialize(*c, v, F, r, method);
            break;
        case 4:
            break;
        default:
            printf("Podano bledny numer, sprobuj ponownie");
            *c = 0;
            break;
    }
}



void initialize(int c, cords* v, cords* F, cords* r, const int method) {
    int osc = 0;
    float t = 0, dt = 0, m = 1;

    printf("\nPodaj warunki poczatkowe polozenia (xo, yo):");
    scanf("%f", &(*r).x);
    scanf("%f", &(*r).y);
    printf("Podaj warunki poczatkowe predkosci (vxo, vyo):");
    scanf("%f", &(*v).x);
    scanf("%f", &(*v).y);
    printf("Podaj mase:");
    scanf("%f", &m);
    if (c == 3) {
        printf("Podaj warunki poczatkowe sily (Fxo, Fyo):");
        scanf("%f", &(*F).x);
        scanf("%f", &(*F).y);
    }
    if(c == 2)  {
        (*F).y = m * -g;
    }

    printf("Podaj czas calkowity:");
    scanf("%f", &t);
    printf("Podaj krok czasowy:");
    scanf("%f", &dt);

    if (c >= 4 &&  c <= 4) osc = 1;

    if (method == 1) {
        euler(v, F, r, m, t, dt, 1);
    } else if (method == 2 ){
        verlet(v, F, r, m, t, dt, 1);
    } else if (method == 3){
        vVerlet(v, F, r, m, t, dt, 1);
    }
}



void euler(cords* v, cords* F, cords* r, float m, float t, float dt, const int osc) {
    float to = 0, Ep = 0, Ek = 0, Ec = 0, w = 0;
    FILE *data = NULL;
    flushf(&data, "./euler_data.txt");

    stepo(F, r, v, &Ep, &Ek, &Ec, m, osc);
    fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);

    while (to < t) {
        to += dt;

        (*r).x += (*v).x * dt;
        (*r).y += (*v).y * dt;

        if (osc) Fcalc(F, r);
        (*v).x += (1/m) * (*F).x * dt;
        (*v).y += (1/m) * (*F).y * dt;

        if (osc) {
            oscEnergy(r, v, &Ep, &Ek, &Ec, m);
        } else {
            energy(r, v, &Ep, &Ek, &Ec, m);
        }

        fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);
    }
    fclose(data);

}



void verlet(cords* v, cords* F, cords* r, float m, float t, float dt, const int osc) {
    cords rm = {0, 0, 0}, rp = {0, 0, 0}; /*rm - krok poprzedni, rp - krok kolejny */
    float to = 0, Ep = 0, Ek = 0, Ec = 0, sqdt = 0;;
    FILE *data = NULL;
    flushf(&data, "./verlet_data.txt");

    stepo(F, r, v, &Ep, &Ek, &Ec, m, osc);
    fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);

    sqdt = dt*dt;
    rp.x = (*r).x + ((*v).x * dt) + (0.5 * (1/m) * (*F).x * sqdt);
    rp.y = (*r).y + ((*v).y * dt) + (0.5 * (1/m) * (*F).y * sqdt);

    while(to < t) {
        to += dt;

        rm.x = (*r).x;
        rm.y = (*r).y;
        (*r).x = rp.x;
        (*r).y = rp.y;

        if (osc) Fcalc(F, r);
        rp.x = -rm.x + 2 * (*r).x + ((1/m) * (*F).x * sqdt);
        rp.y = -rm.y + 2 * (*r).y + ((1/m) * (*F).y * sqdt);

        (*v).x = (rp.x - rm.x)/(2*dt);
        (*v).y = (rp.y - rm.y)/(2*dt);

        if (osc) {
            oscEnergy(r, v, &Ep, &Ek, &Ec, m);
        } else {
            energy(r, v, &Ep, &Ek, &Ec, m);
        }

        fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);
    }
    fclose(data);
}



void vVerlet(cords* v, cords* F, cords* r, float m, float t, float dt, const int osc) {
    float to = 0, Ep = 0, Ek = 0, Ec = 0, sqdt = 0;;
    cords Fo = {0, 0, 0};
    FILE *data = NULL;
    flushf(&data, "./vVerlet_data.txt");

    stepo(F, r, v, &Ep, &Ek, &Ec, m, osc);
    fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);

    sqdt = dt*dt;

    while(to < t) {
        to += dt;

        (*r).x = (*r).x + ((*v).x * dt ) + (0.5 * (1/m) * (*F).x * sqdt);
        (*r).y = (*r).y + ((*v).y * dt ) + (0.5 * (1/m) * (*F).y * sqdt);
        /* do poprawy */
        Fo.x = (*F).x;
        Fo.y = (*F).y;
        if (osc) Fcalc(F, r);
        (*v).x = (*v).x + ((0.5/m) * (Fo.x + (*F).x) * dt);
        (*v).y = (*v).y + ((0.5/m) * (Fo.y + (*F).y) * dt);

        if (osc) {
            oscEnergy(r, v, &Ep, &Ek, &Ec, m);
        } else {
            energy(r, v, &Ep, &Ek, &Ec, m);
        }

        fprintf(data, "%f %f %f %f %f %f %f %f\n", (*r).x, (*r).y, (*v).x, (*v).y, Ep, Ek, Ec, to);
    }
    fclose(data);
}


void Fcalc(cords *F, cords *r) {
    (*F).x = -k * (*r).x;
    (*F).y = -k * (*r).y;
}

void energy(cords* r, cords* v, float* Ep, float* Ek, float* Ec, float m) {
    (*Ep) = m * g * (*r).y;
    (*Ek) = 0.5 * m * ((((*v).x * (*v).x)) + (((*v).y * (*v).y)));
    (*Ec) = (*Ep) + (*Ek);
}

void oscEnergy(cords* r, cords *v, float* Ep, float* Ek, float* Ec, float m) {
    (*Ep) = 0.5 * k * ((((*r).x * (*r).x)) + (((*r).y * (*r).y)));
    (*Ek) = 0.5 * m * ((((*v).x * (*v).x)) + (((*v).y * (*v).y)));
    (*Ec) = (*Ep) + (*Ek);
}

void flushf(FILE** data, char* name) {
    (*data) = fopen(name, "w");
    fprintf((*data), "%s", ""); /* czyszcze plik z poprzednich danych */
    fclose((*data));
    (*data) = fopen(name, "a"); /* tym razem otwieram plik tylko z opcją append */
    fprintf((*data), "#   r.x      r.y      v.x      v.y       Ep        Ek        Ec        t\n");
}

void stepo(cords *F, cords *r, cords *v, float* Ep, float* Ek, float* Ec, float m, int osc) {
    if (osc) {
        oscEnergy(r, v, Ep, Ek, Ec, m);
        Fcalc(F, r);
    } else {
        energy(r, v, Ep, Ek, Ec, m);
    }
}
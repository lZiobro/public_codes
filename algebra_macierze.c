#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 255
#define SEPARATOR ','

double wyznacznik(double**, const int, const int);
double odwracanie(double**, const int, const int);
double** wczytaj(FILE*, int*, int*);
double** na_liczby(char**, const int, int*, int*);
void free_buffer_2d(void** , const int);

double** jednostkowa(const int n) {
    double **matrix;
    int i = 0;
    matrix = calloc(n, sizeof(double *));
    for (i = 0 ; i < n ; i++ ) {
        matrix[i] = calloc(n, sizeof(double));
    }
    for (i = 0 ; i < n ; i++ ) {
            matrix[i][i] = 1;
    }
        return matrix;
}



double gauss(double** matrix, double** matrixex, const int n, const int m) {
    double array[n][m]; /* aby nie modyfikowac glownej */
    int i = 0, j = 0, k = 0, l = 0;
    double tmp = 0;

    if(n != m) {
        printf("Nie mozna wyznaczyc macierzy odwrotnej - wprowadzona macierz nie jest kwadratowa.");
        return 0;
    }

    for ( i = 0 ; i < n ; i++ ) {
        for (j = 0; j < m; j++) {
            array[i][j] = matrix[i][j];
        }
    }

    /* algorytm gaussa */

    /* sprowadzam do postaci trojkatnej */
    for ( i = 0 ; i < n ; i++ ) {
        for (j = i + 1; j < m; j++) {
            tmp = array[j][i] / array[i][i];

/*
poprawic przed wyslaniem - wyznacznik != 0 = nieosobliwa
*/

            for (k = 0 ; k < m ; k++) {
                array[j][k] -= tmp * array[i][k];
                matrixex[j][k] -= tmp * matrixex[i][k];
            }
        }
    }

    /* teraz, zeby zostaly wartosci tylko na przekatnej */
    for ( i = n-1 ; i > 0 ; i-- ) {
        for (j = i - 1; j >= 0; j--) {
            tmp = array[j][i] / array[i][i];
            for (k = 0 ; k < n ; k++) {
                array[j][k] -= tmp * array[i][k];
                matrixex[j][k] -= tmp * matrixex[i][k];
            }
        }
    }

    /* na przekatnej leza wartosci, ale beda najprawdopodobniej rozne od 1, czyli sprowadzam do jednostkowej */
    for ( i = 0 ; i < n ; i++ ) {
        tmp = 1/array[i][i];
        array[i][i] *= tmp;
        for (j = 0 ; j < m ; j++) {
            matrixex[i][j] *= tmp;
        }
    }



    /* printing result */
    for ( i = 0 ; i < n ; i++ ) {
        for (j = 0; j < m; j++) {
            /* moze i troche niepotrzebny test logiczny, ale wyswietla dzieki niemu w dobrych kolumnach w konsoli (: */
            if(matrixex[i][j]>0) {
                printf(" %f ", matrixex[i][j]);
            } else {
                printf("%f ", matrixex[i][j]);
            }
        }
        printf("\n");
    }
    return 0;
}




int main() {
    FILE* plik = NULL;
    double **dane = NULL;
    int i = 0, j = 0, n = 0, m = 0;
    plik = fopen("dane.txt", "r");
    dane = wczytaj(plik, &n, &m);
    if(dane == NULL) {
        printf("Blad wczytywania macierzy");
        return 0;
    } else {
        for (i = 0; i < n ; i++) {
            for (j = 0; j < m ; j++) {
                printf("%f     ", dane[i][j]);
            }
            printf("\n");
        }
    }
    gauss(dane, jednostkowa(n), n, m);
    return 0;
}



double** wczytaj(FILE *fp/* poprawic na nazwe pliku i fp w funkcji */, int* n, int* m) {
    char **buffer = NULL;
    int c = 0, i = 0, j = 1;
    /* allocate first line */
    buffer = malloc(j * sizeof(char*));
    buffer[0] = calloc(MAX_LINE, sizeof(char));

    while (( c = fgetc(fp) ) != EOF ) {
        /* read till new line or EOF*/
        if (c != '\n' && c != '\r') {
            buffer[j-1][i++] = (char)c;
        } else {
            /* if line ended with \n or \r, add a new line */
                j += 1;
                buffer = realloc(buffer, j * sizeof(char*));
                buffer[j-1] = calloc(MAX_LINE, sizeof(char));
                i = 0;
        }
    }
    return na_liczby(buffer, j, n, m);
}

double** na_liczby(char** buffer, const int height, int* n, int* m) {
    double** array = NULL;
    char line[MAX_LINE], *endptr = NULL;
    int h = 0, w = 0; /* aktualne pozycje w buffer*/
    int i = 0, k = 0, mnew = 0;

    /* okreslenie wymiaru m */
    while( buffer[h][w] != '\0' ) {
        if (buffer[h][w++] == SEPARATOR) (*m)++;
    }
    (*m)++;
    (*n) = height;
    w = 0;

    array = malloc((*n) * sizeof(double *));
    for ( i = 0 ; i < (*n) ; i++ ) {
        array[i] = calloc((*m), sizeof(double));
    }

        while (1) {
            /* get one value */
            while (buffer[h][w] != SEPARATOR && buffer[h][w] != '\0') {
                line[k++] = buffer[h][w];
                w++;
            }
            mnew++;
            /* do we have space for that? */
            if (mnew > (*m)) return NULL;
            array[h][mnew-1] = strtod(line, &endptr);

            if (buffer[h][w] == '\0') {
                /* is the new line shorter? */
                if((mnew-(*m)) < 0) return NULL;
                /* was the the last line? */
                if (h == height - 1) {
                    for ( i = 0 ; i < height ; i++ ) {
                        free(buffer[i]);
                    }
                    free(buffer);
                    return array;
                }
                /* start from next row, zero'ing line width */
                h++;
                mnew = 0;
                w = -1;
            }
            /* move on from the separator and clear line */
            w++;
            memset(line, 0, k);
            k = 0;
        }
}
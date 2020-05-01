#include <stdio.h>

#define WIDTH 65
#define HEIGHT 31

/* wypelniania cale pole pustymi polami */
void init_field(char(*)[65]);

/* serce programu */
void sierpinski(char(*)[65]);

/* wyrzucanie wyniku do konsoli */
void print_field(char(*)[65]);

int main()
{
    char field[HEIGHT][WIDTH];

    init_field(field);
    /* inicjacja wedlug zalecen z zadania */
    field[0][31] = '#';
    sierpinski(field);
    print_field(field);




    return 0;
}

void init_field(char (*field)[WIDTH]) {
    int i = 0, j = 0;

    for ( i = 0 ; i < HEIGHT ; i++ ) {
        for ( j = 0 ; j < WIDTH ; j++ ) {
            field[i][j] = ' ';
        }
    }
}

void sierpinski(char (*field)[WIDTH]) {
    int i = 0, j = 0;

    for ( i = 0 ; i < HEIGHT-1 ; i++) {
        /* sprawdzam pola graniczne, gdzie normalnie algorytm wyszedlby poza zakres indexow tablicy */
        if(field[i][1] == '#') field[i+1][0] = '#';
        if(field[i][WIDTH-2] == '#') field[i+1][WIDTH-1] = '#';
        for ( j = 1 ; j < WIDTH-1 ; j++ ) {
            if (( field[i][j-1] != field[i][j+1] ) && ( field[i][j-1] || field[i][j+1] == '#')) {
                field[i+1][j] = '#';
            }
        }
    }
}

void print_field(char (*field)[WIDTH]) {
    int i = 0, j = 0;
    for ( i = 0 ; i < HEIGHT ; i++ ) {
        for ( j = 0 ; j < WIDTH ; j++ ) {
            printf("%c", field[i][j]);
        }
        printf("\n");
    }
}
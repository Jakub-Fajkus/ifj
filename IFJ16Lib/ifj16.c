//
// Created by rockuo on 9.10.16.
// This is library for implementation of IFJ16 class
//

#include "ifj16.h"

#define CHAR_SIZE    sizeof(char)

//local
void Error7() {
    fprintf(stderr, "Input Error \n");
    exit(7);
}
// end local

int readInt() {
    char c;
    char *string;
    int newInt, i = 1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if ((((int) c) >= ((int) '0')) && (((int) c) <= ((int) '9'))) {
            string[i - 1] = c;
            i++;
            string = realloc(string, CHAR_SIZE * i);
        } else {
            free(string);
            Error7();
        }
    }
    string[i - 1] = '\0';
    sscanf(string, "%d", &newInt);
    free(string);
    return newInt;
}

double readDouble() {
    char c;
    char *string;
    double newDouble;
    int i = 1, exponentIndex = -1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if (
                (((int) c >= (int) '0') && ((int) c <= (int) '9')) ||
                ((int) c == (int) '.' && exponentIndex < 0)
                ) {
            string[i - 1] = c;
            i++;
            string = realloc(string, CHAR_SIZE * i);
        } else {
            if (((int) c == 'e' || (int) c == 'E') && exponentIndex == -1 && i>1) {
                exponentIndex = i-1;

                string[i - 1] = c;
                i++;
                string = realloc(string, CHAR_SIZE * i);
            } else if (((int) c == '+' || (int) c == '-') && exponentIndex == i-2){
                string[i - 1] = c;
                i++;
                string = realloc(string, CHAR_SIZE * i);
            }else {
                free(string);
                Error7();
            }
        }
    }
    string[i - 1] = '\0';
    sscanf(string, "%lf", &newDouble);
    free(string);
    return newDouble;
}

char *readString() { // nepoužívám scanf abych měl vždycky přesně naalokovanou velikost stringu
    char c;
    char *string;
    int i = 1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        string[i - 1] = c;
        i++;
        string = realloc(string, CHAR_SIZE * i);
    }
    string[i - 1] = '\0';
    return string;
}

void print( /*some multi argument magic using va_list*/ ) {

}

int length(char *s) {
    return strlen(s);
}

char *substr(char *s, int i, int n) {
    return "test";
}

int compare(char *s1, char *s2) {
    return 0;
}

int find(char *s, char *search) {
    return 0;
}

char *sort(char *s) {
    return "aa";
}
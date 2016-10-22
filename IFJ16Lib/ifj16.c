//
// Created by rockuo on 9.10.16.
// This is library for implementation of IFJ16 class
//

#include "ifj16.h"
#include "../IAL/ial.h"
#include <string.h>

#define CHAR_SIZE    sizeof(char)

//local
void Error7() {
    fprintf(stderr, "Input Error \n");
    exit(7);
}

void Error10() {
    fprintf(stderr, "Other Error \n");
    exit(10);
}
// end local

int ifj16_readInt() {
    int c;
    char *string;
    int newInt, i = 1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if (((c) >= ((int) '0')) && ((c) <= ((int) '9'))) {
            string[i - 1] = (char) c;
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

double ifj16_readDouble() {
    int c;
    char *string;
    double newDouble;
    int i = 1, exponentIndex = -1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if (
                ((c >= (int) '0') && (c <= (int) '9')) ||
                (c == (int) '.' && exponentIndex < 0)
           ) {
            string[i - 1] = (char) c;
            i++;
            string = realloc(string, CHAR_SIZE * i);
        } else {
            if ((c == 'e' || c == 'E') && exponentIndex == -1 && i > 1) {
                exponentIndex = i - 1;

                string[i - 1] = (char) c;
                i++;
                string = realloc(string, CHAR_SIZE * i);
            } else if ((c == '+' || c == '-') && exponentIndex == i - 2) {
                string[i - 1] = (char) c;
                i++;
                string = realloc(string, CHAR_SIZE * i);
            } else {
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

char *ifj16_readString() { // nepoužívám scanf abych měl vždycky přesně naalokovanou velikost stringu
    int c;
    char *string;
    int i = 1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        string[i - 1] = (char) c;
        i++;
        string = realloc(string, CHAR_SIZE * i);
    }
    string[i - 1] = '\0';
    return string;
}

void ifj16_print(char *s) {
    printf(s);
}

int ifj16_length(char *s) {
    return (int) strlen(s);
}

char *ifj16_substr(char *s, int i, int n) {

    if ( s == NULL || n <= 0 || i < 0 )
        Error10();

    int length = (int)strlen(s);
    if ( i + n > length )
        Error10();

    char *newSubstr = malloc(sizeof(char *) +1);
    if (newSubstr == NULL)
        Error10();

    strncpy(newSubstr, s+i, n);
    newSubstr[n] = '\0';
    return newSubstr;
}

int ifj16_compare(char *s1, char *s2) {
    return 0;
}

int ifj16_find(char *s, char *search) {
    return boyer_moore((unsigned char*)s, (unsigned int)strlen(s), (unsigned char*)search, (unsigned int)strlen(search));
}

char *ifj16_sort(char *s) {
    return quickSortWrapper(s);
}
//
// Created by rockuo on 9.10.16.
// This is library for implementation of IFJ16 class
//

#include "ifj16.h"

//local
void Error7() {
    fprintf(stderr, "Input Error \n");
    exit(7);
}
// end local

int readInt() {
    return 0;
}

double readDouble() {
    return 0.0;
}

char *readString() {
    return "test";
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
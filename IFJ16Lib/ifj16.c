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

void Error10() {
    fprintf(stderr, "Other Error \n");
    exit(10);
}
// end local

int readInt() {
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

double readDouble() {
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

char *readString() { // nepoužívám scanf abych měl vždycky přesně naalokovanou velikost stringu
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

void print( /*some multi argument magic using va_list*/ ) {

}

int length(char *s) {
    return (int) strlen(s);
}

char *substr(char *s, int i, int n) {

    if ( s == NULL || n <= 0 || i < 0 )
        Error10();

    int length = strlen(s);
    if ( i + n > length )
        Error10();

    char *newSubstr = malloc(sizeof(char *) +1);
    if (newSubstr == NULL)
        Error10();

    strncpy(newSubstr, s+i, n);
    newSubstr[n] = '\0';
    return newSubstr;
}

int compare(char *s1, char *s2) {
    return 0;
}

int find(char *s, char *search) {
    return 0;
}

char *sort(char *s) {

    int right = length(s);
    char *arr; /* our array for return as sorted string */
    arr = (char*) malloc(sizeof(char)*right);


    int k = 0;  /* copy elements into that */
    while(k <= right-1){
        arr[k] = s[k];
        k++;
    }

    quickSort(arr,0,right); /* call our quick sort */

    return arr;
}

void quickSort(char *arr, int left, int right){

    if(left < right){
        int border = left;
        for(int i = left+1; i < right; i++){
            if(arr[i] < arr[left]){
                swap(arr, i, ++border);
            }
        }
        swap(arr, left, border);
        quickSort(arr, left, border);
        quickSort(arr, border + 1, right);
    }
}

char medianIndex(char *s){
    char pole[3] = {s[0],s[length(s)/2],s[length(s)-1]};
    for (int i = 0; i < 3; ++i) {
        if(pole[i] > pole[i+1]){
            char tmp = pole[i];
            pole[i] = pole[i+1];
            pole[i+1] = tmp;
        }
    }

    return pole[1];
}

void swap(char *arr, int left, int right){
    char tmp = arr[right];
    arr[right] = arr[left];
    arr[left] = tmp;
}
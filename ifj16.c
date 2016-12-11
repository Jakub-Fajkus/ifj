/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 * Andrej Hučko
 */

#include "ifj16.h"
#include "debug.h"

#define ALPHABET_LEN 256

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
    bool readSomething = false;

    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if (((c) >= ((int) '0')) && ((c) <= ((int) '9'))) {
            string[i - 1] = (char) c;
            i++;
            string = realloc(string, CHAR_SIZE * i);
            readSomething = true;
        } else {
            free(string);
            Error7();
        }
    }
    if(!readSomething) {
        free(string);
        Error7();
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
    bool readSomething = false;

    int i = 1, exponentIndex = -1; // only '\0'
    string = (char *) malloc(CHAR_SIZE * i);
    while (((c = getchar()) != EOF) && (c != '\n')) {
        if (
                ((c >= (int) '0') && (c <= (int) '9')) ||
                (c == (int) '.' && exponentIndex < 0)
           ) {
            string[i - 1] = (char) c;
            i++;
            readSomething = true;
            string = realloc(string, CHAR_SIZE * i);
        } else {
            if ((c == 'e' || c == 'E') && exponentIndex == -1 && i > 1) {
                exponentIndex = i - 1;

                string[i - 1] = (char) c;
                i++;
                readSomething = true;
                string = realloc(string, CHAR_SIZE * i);
            } else if ((c == '+' || c == '-') && exponentIndex == i - 2) {
                string[i - 1] = (char) c;
                i++;
                readSomething = true;
                string = realloc(string, CHAR_SIZE * i);
            } else {
                free(string);
                Error7();
            }
        }
    }
    if(!readSomething) {
        free(string);
        Error7();
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
    printf("%s", s);
}

int ifj16_length(char *s) {
    return (int) strlen(s);
}

char *ifj16_substr(char *s, int i, int n) {

    if ( s == NULL || n <= 0 || i < 0 ) {
        Error10();
    }

    int length = (int)strlen(s);

    if ( i + n > length ) {
        Error10();
    }

    char *newSubstr = malloc( CHAR_SIZE * (n+1) );
    if (newSubstr == NULL){
        Error10();
    }
    newSubstr[0] = '\0';

    strncpy(newSubstr, s+i, n);
    newSubstr[n] = '\0';
    return newSubstr;
}

int ifj16_compare(char *s1, char *s2) {
    if (s1 == NULL || s2 == NULL) {
        exit(99);
    }
    int ret = strcmp(s1, s2);

    if(ret > 0) {
        return 1;
    } else if (ret < 0) {
        return -1;
    } else {
        return 0;
    }
}

int ifj16_find(char *s, char *search) {
    return boyer_moore((unsigned char*)s, (unsigned int)strlen(s), (unsigned char*)search, (unsigned int)strlen(search));
}

char *ifj16_sort(char *s) {
    return quickSortWrapper(s);
}

char *stringConcat(char *str1, char *str2) {

    char *buffer = malloc(sizeof(char)*(strlen(str1) + strlen(str2)+1));
    buffer[0] = '\0';

    strcat(buffer, str1);
    strcat(buffer, str2);

    return buffer;
}

void make_delta(int *delta, unsigned char *pat, int patlen) {
    int i;
    for (i=0; i < ALPHABET_LEN; i++) {
        delta[i] = patlen; //not found
    }
    for (i=0; i < patlen-1; i++) {
        delta[pat[i]] = patlen-1 - i;
    }
}

int boyer_moore (unsigned char *string, unsigned int stringlen,unsigned char *pat, unsigned int patlen) {
    int i;
    int delta[ALPHABET_LEN];
    make_delta(delta, pat, patlen);

    // The empty pattern must be considered specially
    if (patlen == 0) {
        return 0;
    }

    i = patlen-1;
    while (i < stringlen) {
        int j = patlen-1;
        while (j >= 0 && (string[i] == pat[j])) {
            --i;
            --j;
        }
        if (j < 0) {
            return (string + i+1) - string;
        }

        i += delta[string[i]];
    }

    return -1;
}


//************** QUICK SORT **************

char* quickSortWrapper(char *s) {
    int right = (int)strlen(s);
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

void swap(char *arr, int left, int right){
    char tmp = arr[right];
    arr[right] = arr[left];
    arr[left] = tmp;
}
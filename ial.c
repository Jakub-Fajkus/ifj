#include <stdint.h>
#include <stdlib.h>

#include "ial.h"
#include "ifj16.h"

//************** BOYER-MOORE **************

#define ALPHABET_LEN 256

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

//todo: @DavidCzernin unused?
char medianIndex(char *s){
    char pole[3] = {s[0],s[ifj16_length(s)/2],s[ifj16_length(s)-1]};
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

char *stringConcat(char *str1, char *str2) {
    char *buffer = malloc(sizeof(char)*(strlen(str1) + strlen(str2)+1));
    buffer[0] = '\0';

    strcat(buffer, str1);
    strcat(buffer, str2);

    return buffer;
}
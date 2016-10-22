//
// Created by rockuo on 9.10.16.
//

#include "ifj16.h"
#include "../IAL/ial.h"

void testQuickSort();
void testFind();

int main(int argc, char *argv[]) {

    printf("readInt %d \n",ifj16_readInt());
    printf("readDouble %lf \n",ifj16_readDouble());
    printf("readString %s \n",ifj16_readString());
//
    ifj16_print("toto je string!\n");

    printf("length %d \n",ifj16_length("aaa"));
    printf("substr %s \n", ifj16_substr("aaa", 0, 1));
    printf("compare %d \n", ifj16_compare("abc", "abc"));

    testQuickSort();
    testFind();

    return 0;
}

void testQuickSort() {
    char *str;

    printf("************** STARTING testQuickSort **************\n");

    str = "abcdefgh";
    printf("sorting already sorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "hgfedcba";
    printf("sorting already inversely sorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "1234567";
    printf("sorting already sorted numbers '%s': '%s'\n", str, ifj16_sort(str));

    str = "7654321";
    printf("sorting already inversely sorted numbers '%s': '%s'\n", str, ifj16_sort(str));


    str = "hkagjfbdjkf";
    printf("sorting unsorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "hjkasdj4213";
    printf("sorting unsorted string with numbers '%s': '%s'\n", str, ifj16_sort(str));

    str = "OICAEQBC";
    printf("sorting unsorted string with uppercase '%s': '%s'\n", str, ifj16_sort(str));

    str = "CoOBaDA";
    printf("sorting unsorted string with uppercase and lowercase '%s': '%s'\n", str, ifj16_sort(str));

    printf("************** END OF testQuickSort **************\n\n\n");
}

void testFind() {
    char* haystack;
    char* needle;

    printf("************** STARTING testFind **************\n");

    haystack = "abcdef";
    needle = "bc";
    printf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "mama mele maso";
    needle = "mele";
    printf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "uz mi to funguje!";
    needle = "fun";
    printf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "abc";
    printf("located at the begining of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "ef";
    printf("located at the end of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "fg";
    printf("located after the end of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "1a";
    printf("located before the beginning of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "XX";
    printf("not located at all: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    printf("************** END OF testFind **************\n\n\n");
}

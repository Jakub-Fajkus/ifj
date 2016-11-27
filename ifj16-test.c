//
// Created by rockuo on 9.10.16.
//

#include "ifj16.h"
#include "ial.h"
#include "Debug.h"

void testQuickSort();
void testFind();

//int main(int argc, char *argv[]) {
//
//    printf("readInt %d \n",ifj16_readInt());
//    printf("readDouble %lf \n",ifj16_readDouble());
//    debugPrintf("readString %s \n",ifj16_readString());
////
//    ifj16_print("toto je string!\n");
//
//    debugPrintf("length %d \n",ifj16_length("aaa"));
//    debugPrintf("substr %s \n", ifj16_substr("aaa", 0, 1));
//    debugPrintf("compare %d \n", ifj16_compare("abc", "abc"));
//
//    testQuickSort();
//    testFind();
//
//    return 0;
//}

void testQuickSort() {
    char *str;

    debugPrintf("************** STARTING testQuickSort **************\n");

    str = "abcdefgh";
    debugPrintf("sorting already sorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "hgfedcba";
    debugPrintf("sorting already inversely sorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "1234567";
    debugPrintf("sorting already sorted numbers '%s': '%s'\n", str, ifj16_sort(str));

    str = "7654321";
    debugPrintf("sorting already inversely sorted numbers '%s': '%s'\n", str, ifj16_sort(str));


    str = "hkagjfbdjkf";
    debugPrintf("sorting unsorted string '%s': '%s'\n", str, ifj16_sort(str));

    str = "hjkasdj4213";
    debugPrintf("sorting unsorted string with numbers '%s': '%s'\n", str, ifj16_sort(str));

    str = "OICAEQBC";
    debugPrintf("sorting unsorted string with uppercase '%s': '%s'\n", str, ifj16_sort(str));

    str = "CoOBaDA";
    debugPrintf("sorting unsorted string with uppercase and lowercase '%s': '%s'\n", str, ifj16_sort(str));

    debugPrintf("************** END OF testQuickSort **************\n\n\n");
}

void testFind() {
    char* haystack;
    char* needle;

    debugPrintf("************** STARTING testFind **************\n");

    haystack = "abcdef";
    needle = "bc";
    debugPrintf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "mama mele maso";
    needle = "mele";
    debugPrintf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "uz mi to funguje!";
    needle = "fun";
    debugPrintf("located inside the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "abc";
    debugPrintf("located at the begining of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "ef";
    debugPrintf("located at the end of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "fg";
    debugPrintf("located after the end of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "1a";
    debugPrintf("located before the beginning of the string: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    haystack = "abcdef";
    needle = "XX";
    debugPrintf("not located at all: find '%s' in '%s': found on index: %d \n", needle, haystack, ifj16_find(haystack, needle));

    debugPrintf("************** END OF testFind **************\n\n\n");
}

//
// Created by rockuo on 9.10.16.
//

#include "ifj16.h"


int main(int argc, char *argv[]) {
    printf("readInt %d \n",readInt());
    printf("readDouble %lf \n",readDouble());
    printf("readString %s \n",readString());

//    print( /*some multi argument magic using va_list*/ );


    printf("length %d \n",length("aaa"));
    printf("substr %s \n", substr("aaa", 0, 1));
    printf("compare %d \n", compare("abc", "abc"));
    printf("find %d \n", find("abc", "a"));
    printf("sort %s \n", sort("bbabafrvabb"));
    //printf("median from %s is %c","adc",medianIndex("adc"));
    return 0;
}	

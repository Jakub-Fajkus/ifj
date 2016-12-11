//
// Created by Jakub Fajkus on 13.11.16.
//

#include "syntacticalanalyzer.h"
#include "debug.h"
int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("no input file found\n");
        return 99;
    }

    runSyntacticalAnalysis(argv[1]);

    debugPrintf("\n-----KONEC-----");
    return 0;
}

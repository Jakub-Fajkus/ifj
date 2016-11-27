//
// Created by Jakub Fajkus on 13.11.16.
//

#include "ifj16.h"
#include "symboltable.h"
#include "lexicalanalyzer.h"
#include "syntacticalanalyzer.h"
#include "stack.h"
#include "lexicalanalyzerstructures.h"
#include "interpret.h"
#include "instruction.h"
#include "debug.h"
//#include "BasicStructures.h"
//#include "Debug.h"

void simulateSymbolTable();

int main(int argc, char *argv[])
{
    int x = 0;
    // uncomment what you need


//    simulateSymbolTable();
/*
    if (argc != 2) {
        printf("no input file found\n");
        return 99;
    }
    */
    runSyntacticalAnalysis(argv[1]);


//    x = callInterpret();    // #justInterpretThings

    debugPrintf("\n-----KONEC-----");
    return x;
}

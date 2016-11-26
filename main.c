//
// Created by Jakub Fajkus on 13.11.16.
//

#include "ifj16.h"
#include "SymbolTable.h"
#include "LexicalAnalyzer.h"
#include "SyntacticalAnalyzer.h"
#include "Stack.h"
#include "LexicalAnalyzerStructures.h"
#include "Interpret.h"
#include "instruction.h"
//#include "BasicStructures.h"
//#include "Debug.h"

void simulateSymbolTable();

int main(int argc, char *argv[])
{
    // uncomment what you need


//    simulateSymbolTable();
/*
    if (argc != 2) {
        printf("no input file found\n");
        return 99;
    }
    */
//    runSyntacticalAnalysis(argv[1]);


    callInterpret();    // #justInterpretThings

    printf("\n-----KONEC-----");
    return 0;
}

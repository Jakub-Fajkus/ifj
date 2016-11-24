//
// Created by Jakub Fajkus on 23.11.16.
//

#include "SemanticalAnalyzer.h"
#include "SymbolTable.h"

extern SYMBOL_TABLE_NODEPtr *globalSymbolTable;
extern SYMBOL_TABLE_NODEPtr *actualSymbolTable;
extern SYMBOL_TABLE_FUNCTION *actualFunction;

void checkIfFunctionRunExists() {
    // get function Main.run

    SYMBOL_TABLE_FUNCTION *function = getFunctionFromTable(globalSymbolTable, "Main.run");

    if (function == NULL) {
        exit(3);
    }
}
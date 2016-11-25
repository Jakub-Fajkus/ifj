//
// Created by Jakub Fajkus on 23.11.16.
//

#include "SemanticalAnalyzer.h"
#include "ifj16.h"

extern SYMBOL_TABLE_NODEPtr *globalSymbolTable;
extern SYMBOL_TABLE_FUNCTION *actualFunction;

void checkIfFunctionRunExists() {
    // get function Main.run

    SYMBOL_TABLE_FUNCTION *function = getFunctionFromTable(globalSymbolTable, "Main.run");

    if (function == NULL) {
        exit(3);
    }
}

SYMBOL_TABLE_FUNCTION *semantic_getFunction(char * functionName) {
    SYMBOL_TABLE_FUNCTION *function = NULL;
    if (-1 == ifj16_find(functionName, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(actualFunction->name, 0, ifj16_find(actualFunction->name, "."));
        char *nameWithDot = stringConcat(className, ".");
        char *fullyQualifiedName = stringConcat(nameWithDot, functionName);
        free(nameWithDot);

        function = getFunctionFromTable(globalSymbolTable, fullyQualifiedName);
        free(className);
        free(fullyQualifiedName);
    } else {
        //the name is already fully qualified
        function = getFunctionFromTable(globalSymbolTable, functionName);
    }
    if(function == NULL){
        exit(3);
    }
    return function;
}

SYMBOL_TABLE_VARIABLE *semantic_getVariable(char * name) {
    SYMBOL_TABLE_VARIABLE *variable = NULL;
    if (-1 == ifj16_find(name, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(actualFunction->name, 0, ifj16_find(actualFunction->name, "."));
        char *nameWithDot = stringConcat(className, ".");
        char *fullyQualifiedName = stringConcat(nameWithDot, name);
        free(nameWithDot);

        variable = getVariableFromTable(globalSymbolTable, fullyQualifiedName);
        free(className);
        free(fullyQualifiedName);
    } else {
        //the name is already fully qualified
        variable = getVariableFromTable(globalSymbolTable, name);
    }
    if(variable == NULL){
        exit(3);
    }
    //the variable was either found(is global) or not found at all
    return variable;
}
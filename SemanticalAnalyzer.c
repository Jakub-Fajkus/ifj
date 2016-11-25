//
// Created by Jakub Fajkus on 23.11.16.
//

#include "SemanticalAnalyzer.h"
#include "ifj16.h"
#include "BasicStructures.h"

struct SYMBOL_TABLE_NODE *globalSymbolTable;
SYMBOL_TABLE_FUNCTION *actualFunction;

void semanticCheckForFunctionRun() {
    // get function Main.run
    SYMBOL_TABLE_FUNCTION *function = getFunctionFromTable(&globalSymbolTable, "Main.run");
    if (function == NULL) {
        exit(3);
    }
    if(function->type != TYPE_VOID){
        exit(8);
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

        function = getFunctionFromTable(&globalSymbolTable, fullyQualifiedName);
        free(className);
        free(fullyQualifiedName);
    } else {
        //the name is already fully qualified
        function = getFunctionFromTable(&globalSymbolTable, functionName);
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

        variable = getVariableFromTable(&globalSymbolTable, fullyQualifiedName);
        free(className);
        free(fullyQualifiedName);
    } else {
        //the name is already fully qualified
        variable = getVariableFromTable(&globalSymbolTable, name);
    }
    if(variable == NULL){
        exit(3);
    }
    //the variable was either found(is global) or not found at all
    return variable;
}

SYMBOL_TABLE_VARIABLE *semantic_getInitializedVariable(char * name) {
    SYMBOL_TABLE_VARIABLE *variable = semantic_getVariable(name);
    if (variable->initialized == false) {
        printf("variable %s is not initialized", name);
        exit(8);
    }
    return variable;
}

void checkFunctionParametersType(char *functionName, tDLList *parameters){
    struct SYMBOL_TABLE_FUNCTION_STR *function = semantic_getFunction(functionName);
    ListFirst(parameters);
    ListFirst(function->parameters);
    while(true){
        if(function->parameters->Act->element.data.parameter->type != parameters->Act->element.data.parameter->type){
            exit(4);
        }
        if(function->parameters->Act == function->parameters->Last && parameters->Act == parameters->Last){
            break;
        }else if((function->parameters->Act == function->parameters->Last && parameters->Act != parameters->Last) ||
                (function->parameters->Act != function->parameters->Last && parameters->Act == parameters->Last)){
            printf("Wrong number of Parameters");
            exit(4);
        }
    }
}
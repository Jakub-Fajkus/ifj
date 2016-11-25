//
// Created by Jakub Fajkus on 23.11.16.
//

#ifndef IFJ_SEMANTICALANALYZER_H
#define IFJ_SEMANTICALANALYZER_H
#include "SymbolTable.h"
#include "DoubleLinkedList.h"
void checkIfFunctionRunExists();
SYMBOL_TABLE_FUNCTION *semantic_getFunction(char * functionName);
SYMBOL_TABLE_VARIABLE *semantic_getVariable(char * name);
SYMBOL_TABLE_VARIABLE *semantic_getInitializedVariable(char * name);

/**
 *
 * @param functionName
 * @param parameters list of Strings
 */
void checkFunctionParametersType(char *functionName, tDLList *parameters);
#endif //IFJ_SEMANTICALANALYZER_H

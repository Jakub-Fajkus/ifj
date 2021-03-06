/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 * David Czernin
 */

#ifndef IFJ_SEMANTICALANALYZER_H
#define IFJ_SEMANTICALANALYZER_H
#include "symboltable.h"
#include "doublelinkedlist.h"
void semanticCheckForFunctionRun();
SYMBOL_TABLE_FUNCTION *semantic_getFunction(char * functionName);
SYMBOL_TABLE_VARIABLE *semantic_getVariable(char * name);
SYMBOL_TABLE_VARIABLE *semantic_getInitializedVariable(char * name);
bool semantical_checkTypes(DATA_TYPE destination, DATA_TYPE source);
bool semantical_checkFunctionCall(tDLList *declaredParameters, tDLList *usedParameters);
void createInstructionsToCallIfj16Function(char *functionName, tDLList *instructions, tDLList *givenParameters, char *returnValueToVariable);
void semantical_ExistAndCorrectTypeOfReturns(SYMBOL_TABLE_NODEPtr *symbolTable);
bool isFunctionFromIfj16(char *name);
void initializeVariable(char *name);
bool canConvertTypes(DATA_TYPE destination, DATA_TYPE source);
void semantic_firstPass_testStaticVariable(char * name);
void semanticCheckForMissingStaticVar();
SYMBOL_TABLE_FUNCTION* semanticCreateAndInsertFunction(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters, tDLList *instructions, bool hasReturn);
SYMBOL_TABLE_VARIABLE* semanticCreateAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, bool initialized);
/**
 *
 * @param functionName
 * @param parameters list of Strings
 */
void checkFunctionParametersType(char *functionName, tDLList *parameters);
#endif //IFJ_SEMANTICALANALYZER_H

//
// Created by Jakub Fajkus on 12.11.16.
//

#ifndef IFJ_SYNTACTICANALYZER_H
#define IFJ_SYNTACTICANALYZER_H

#include "SymbolTable.h"


#define stringEquals(x,y) (strcmp(x, y) == 0)
/**
 * Get the next token
 *
 * @return
 */
TOKEN *getCachedToken();

/**
 * Run the whole syntactical analysis
 *
 * @param fileName
 */
void runSyntacticalAnalysis(char *fileName);
TOKEN *getCachedToken();
void returnCachedTokens(unsigned int count);
/**
 * Get all tokend from the lexical analyser
 *
 * @param fileName
 * @return
 */
tDLList* getAllTokens(char *fileName);
LIST_ELEMENT wrapInstructionIntoListElement(struct sINSTRUCTION *instr);
LIST_ELEMENT createListElementWithFunctionParamameter(char *name, DATA_TYPE type);
int analyzeExpression(tDLList *instructionList, char **resultVariableName, DATA_TYPE *resultVariableType);

bool ruleProg();
bool ruleClassDefinition(char *className);
bool ruleDefinition(char *className, DATA_TYPE type, char *name, bool *variableInitialized, bool *isFunction);
bool rulePropDef(bool *variableInitialized, DATA_TYPE variableType, char *variableName, char *className);
bool ruleFuncDef();
bool ruleStListDecl();
bool ruleStList();
bool ruleDecl(DATA_TYPE type, char *variableName);
bool ruleStat();
bool ruleStatBeginningId(char *functionOrPropertyName);
bool ruleFuncCall(char *calledFunctionName, char *assignReturnValueToVariable);
bool ruleFuncParams(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters);
bool ruleParam(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters);
bool ruleFunctionCallEnd();
bool ruleAfterFunctionCallExp(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters);
bool ruleFuncDefParams();
bool ruleDefParam();
bool ruleDefParamBeginTi();
bool ruleFunctionDefEnd();
bool ruleTypeInt(DATA_TYPE *type);
bool ruleTypeDouble(DATA_TYPE *type);
bool ruleTypeString(DATA_TYPE *type);
bool ruleDefinitionStart(char *className);
bool ruleExpSemicolon();

int analyzeExpression(tDLList *instructionList, char **resultVariableName, DATA_TYPE *resultVariableType);
#endif //IFJ_SYNTACTICANALYZER_H

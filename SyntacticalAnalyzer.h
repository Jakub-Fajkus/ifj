//
// Created by Jakub Fajkus on 12.11.16.
//

#ifndef IFJ_SYNTACTICANALYZER_H
#define IFJ_SYNTACTICANALYZER_H

#include "SymbolTable.h"


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


bool ruleProg();
bool ruleClassDefinition(char *className);
bool ruleDefinition(char *className, DATA_TYPE type, char *name, bool *variableInitialized, bool *isFunction);
bool rulePropDef(bool *variableInitialized);
bool ruleFuncDef(SYMBOL_TABLE_FUNCTION *function);
bool ruleStListDecl(SYMBOL_TABLE_FUNCTION *function);
bool ruleStList();
bool ruleDecl(SYMBOL_TABLE_FUNCTION *function, DATA_TYPE type, char *variableName);
bool ruleStat();
bool ruleStatBeginningId();
bool ruleFuncCall();
bool ruleFuncParams();
bool ruleParam();
bool ruleFunctionCallEnd();
bool ruleAfterFunctionCallExp();
bool ruleFuncDefParams(SYMBOL_TABLE_FUNCTION *function);
bool ruleDefParam(SYMBOL_TABLE_FUNCTION *function);
bool ruleDefParamBeginTi(SYMBOL_TABLE_FUNCTION *function, DATA_TYPE type, char *name);
bool ruleFunctionDefEnd();
bool ruleTypeInt(DATA_TYPE *type);
bool ruleTypeDouble(DATA_TYPE *type);
bool ruleTypeString(DATA_TYPE *type);
bool ruleTypeVoid(DATA_TYPE *type);
bool ruleDefinitionStart(char *className);
bool ruleExpSemicolon();
bool ruleIdFunctionCall(char *name);

int analyzeExpression(tDLList *instructionList, char *resultVariableName);
#endif //IFJ_SYNTACTICANALYZER_H

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
bool ruleClassDefinition();
bool ruleDefinition();
bool rulePropDef();
bool ruleFuncDef();
bool ruleStListDecl();
bool ruleStList();
bool ruleDecl();
bool ruleStat();
bool ruleStatBeginningId();
bool ruleFuncCall();
bool ruleFuncParams();
bool ruleParam();
bool ruleFunctionCallEnd();
bool ruleAfterFunctionCallExp();
bool ruleFuncDefParams();
bool ruleDefParam();
bool ruleDefParamBeginTi();
bool ruleFunctionDefEnd();
bool ruleTypeInt(char **name);
bool ruleTypeDouble(char **type);
bool ruleTypeString(char **type);
bool ruleTypeVoid(char **type);
bool ruleDefinitionStart();
bool ruleExpSemicolon();

#endif //IFJ_SYNTACTICANALYZER_H

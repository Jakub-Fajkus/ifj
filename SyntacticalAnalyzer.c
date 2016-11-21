//
// Created by Jakub Fajkus on 12.11.16.
//

#include "SyntacticalAnalyzer.h"
#include "Debug.h"
#include "LexicalAnalyzerStructures.h"

#define stringEquals(x,y) (strcmp(x, y) == 0)

tDLList *globalTokens;

/**
 * Return 'count' number of tokens.
 * If the count equals to one, the actual token is returned and the next token returned by funtion getCachedToken() is the same token as it was returned the last time.
 *
 * Example: tokens are:
 * tok1 -> tok2 -> tok3
 * last returned token was "tok2". you call the function returnCachedTokens() with count= 1 and the next token retunred by function getCachedToken() will be again "tok2"

 * @param count
 */
void returnCachedTokens(unsigned int count);

/**
 * Get all tokend from the lexical analyser
 *
 * @param fileName
 * @return
 */
tDLList* getAllTokens(char *fileName);

void testTokens();

void firstPass();

void runSyntacticalAnalysis(char *fileName) {
    globalTokens = getAllTokens(fileName);
    tDLList *tokens = globalTokens;
    firstPass();

//    testTokens(); //todo: this is only for "testing"
}


TOKEN *getCachedToken() {
    LIST_ELEMENT *element = malloc(sizeof(LIST_ELEMENT));
    TOKEN *token;

    //copy the active element
    ListElementCopy(globalTokens, element);
    token = element->data.token;
    //move the activity to the next element
    ListSuccessor(globalTokens);
    //free the container
    free(element);

    return token;
}

void returnCachedTokens(unsigned int count) {
    for (int i = 0; i < count; ++i) {
        ListPredcessor(globalTokens);
    }
}

tDLList* getAllTokens(char *fileName) {
    tDLList *listOfTokens = malloc(sizeof(tDLList));
    ListInit(listOfTokens);

    initializeStream(fileName);
    TOKEN *token;

    do {
        token = getToken();
        LIST_ELEMENT_DATA *data = malloc(sizeof(LIST_ELEMENT_DATA));
        data->token = token;

        struct LIST_ELEMENT *listElement = malloc(sizeof(LIST_ELEMENT));
        listElement->data = *data;
        listElement->type = LIST_ELEMENT_TYPE_TOKEN;

        ListInsertLast (listOfTokens, *listElement);
    } while(token->type != END_OF_FILE);

    ListFirst(listOfTokens);

    return listOfTokens;
}

bool ruleTypeString() {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "string")) {
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleTypeDouble() {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleTypeInt() {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "int")) {
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleProg(){
    //todo
    return false;
}

bool ruleClassDefinition(){
    //todo
    return false;
}

bool ruleDefinition(){
    //todo
    return false;
}

bool rulePropDef(){
    //todo
    return false;
}

bool ruleFuncDef(){
    //todo
    return false;
}

bool ruleStListDecl(){
    //todo
    return false;
}

bool ruleStList(){
    //todo
    return false;
}

bool ruleDecl(){
    //todo
    return false;
}

bool ruleStat(){
    //todo
    return false;
}

bool ruleFuncCall(){
    //todo
    return false;
}

bool ruleFuncParams(){
    //todo
    return false;
}

bool ruleParam(){
    //todo
    return false;
}

bool ruleFunctionCallEnd(){
    //todo
    return false;
}

bool ruleAfterFunctionCallExp(){
    //todo
    return false;
}

bool ruleFuncDefParams(){
    //todo
    return false;
}

bool ruleDefParam(){
    //todo
    return false;
}

bool ruleDefParamBeginTi(){
    //todo
    return false;
}

bool ruleFunctionDefEnd(){
    //todo
    return false;
}









void testTokens() {
    tDLList *tokens = getAllTokens("test1.txt");
    TOKEN *actualToken;

    actualToken = getCachedToken(); //1.
    printToken(actualToken);
    actualToken = getCachedToken(); //2.
    printToken(actualToken);
    actualToken = getCachedToken(); //3.
    printToken(actualToken);
    actualToken = getCachedToken(); //4.
    printToken(actualToken);
    returnCachedTokens(1); //return to 3. and 2.
    actualToken = getCachedToken(); //2.
    printToken(actualToken);
}

void firstPass() {
    ListFirst(globalTokens);


    //call function for class
}



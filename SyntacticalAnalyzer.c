//
// Created by Jakub Fajkus on 12.11.16.
//

#include "SyntacticalAnalyzer.h"
#include "Debug.h"

/**
 * Get the next token
 *
 * @param tokens
 * @return
 */
TOKEN *getCachedToken(tDLList *tokens);
/**
 * Return 'count' number of tokens.
 * If the count equals to one, the actual token is returned and the next token returned by funtion getCachedToken() is the same token as it was returned the last time.
 *
 * Example: tokens are:
 * tok1 -> tok2 -> tok3
 * last returned token was "tok2". you call the function returnCachedTokens() with count= 1 and the next token retunred by function getCachedToken() will be again "tok2"
 *
 * @param tokens
 * @param count
 */
void returnCachedTokens(tDLList *tokens, unsigned int count);

/**
 * Get all tokend from the lexical analyser
 *
 * @param fileName
 * @return
 */
tDLList* getAllTokens(char *fileName);

void testTokens();

void firstPass(tDLList *tokens);

void runSyntacticalAnalysis(char *fileName) {
    tDLList *tokens = getAllTokens(fileName);
    firstPass(tokens);

    testTokens(); //todo: this is only for "testing"
}


TOKEN *getCachedToken(tDLList *tokens) {
    LIST_ELEMENT *element = malloc(sizeof(LIST_ELEMENT));
    TOKEN *token;

    //copy the active element
    DLCopy(tokens, element);
    token = element->data.token;
    //move the activity to the next element
    DLSucc(tokens);
    //free the container
    free(element);

    return token;
}

void returnCachedTokens(tDLList *tokens, unsigned int count) {
    for (int i = 0; i < count; ++i) {
        DLPred(tokens);
    }
}

tDLList* getAllTokens(char *fileName) {
    tDLList *listOfTokens = malloc(sizeof(tDLList));
    DLInitList(listOfTokens);

    initializeStream(fileName);
    TOKEN *token;

    do {
        token = getToken();
        LIST_ELEMENT_DATA *data = malloc(sizeof(LIST_ELEMENT_DATA));
        data->token = token;

        struct LIST_ELEMENT *listElement = malloc(sizeof(LIST_ELEMENT));
        listElement->data = *data;
        listElement->type = LIST_ELEMENT_TYPE_TOKEN;

        DLInsertLast(listOfTokens, *listElement);
    } while(token->type != END_OF_FILE);

    DLFirst(listOfTokens);

    return listOfTokens;
}

void testTokens() {
    tDLList *tokens = getAllTokens("test1.txt");
    TOKEN *actualToken;

    actualToken = getCachedToken(tokens); //1.
    printToken(actualToken);
    actualToken = getCachedToken(tokens); //2.
    printToken(actualToken);
    actualToken = getCachedToken(tokens); //3.
    printToken(actualToken);
    actualToken = getCachedToken(tokens); //4.
    printToken(actualToken);
    returnCachedTokens(tokens, 1); //return to 3. and 2.
    actualToken = getCachedToken(tokens); //2.
    printToken(actualToken);
}

void firstPass(tDLList *tokens) {
    TOKEN *token;
    DLFirst(tokens);

    do {
        token = getCachedToken(tokens);

        //do stuff :D

    } while(token->type != END_OF_FILE);

}

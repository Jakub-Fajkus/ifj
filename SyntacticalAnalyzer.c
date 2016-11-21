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

bool ruleId() {
    TOKEN *token = getCachedToken();

    return token->type == IDENTIFIER || token->type == IDENTIFIER_FULL;
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
    TOKEN *token = getCachedToken();
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "class")) {
        if (ruleId()) {
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '{') {
                if (ruleClassDefinition()) {
                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == '}') {
                        if (ruleProg()) {
                            return true;
                        } else {
                            //rule prog was not applied, but it has an epsilon rule... so check if the token is EOF(in this particular case...)
                            //if so, the source file ended and the syntax analysis is successful
                            token = getCachedToken();
                            if (token->type == END_OF_FILE) {
                                //no need to return any token, the source file is over
                                return true;
                            } else {
                                //the token was not an EOF so there were some other tokens but they did not match the rule prog
                                //so the syntax analysis is over because of syntax error
                                globalTokens->Act = activeElementRuleApplication;
                                return false;
                            }
                        }
                    }
                }
            }
        }
    } else if (token->type == END_OF_FILE) {
        return true;
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleClassDefinition(){
    TOKEN *token = getCachedToken();
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "static")) {
        if (ruleDefinitionStart()) {
            if (ruleClassDefinition()) {
                return true;
            } else {
                globalTokens->Act = activeElementRuleApplication;
                return false;
            }
        } else {
            globalTokens->Act = activeElementRuleApplication;

            return false;
        }
    } else {
        //rule prog was not applied, but it has an epsilon rule... so check if the token is '}'(in this particular case...)
        //if so, the class definition ended and rule is applied
        token = getCachedToken();
        if (token->type == BRACKET && token->data.bracket.name == '}') {
            returnCachedTokens(1);
            return true;
        } else {
            //the token was not an '}' so there were some other tokens but they did not match the rule
            //so the syntax analysis is over because of syntax error
            globalTokens->Act = activeElementRuleApplication;
            return false;
        }
    }
}

bool ruleDefinition(){
    return rulePropDef() || ruleFuncDef();
}

bool rulePropDef(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        return true;
    } else if (token->type == OPERATOR_ASSIGN) {
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: replace dummy test with actual list of instruction
        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
        ListInit(dummyInstructionLIst);

        if (parseExpression(globalTokens, dummyInstructionLIst)) {
            if (token->type == SEMICOLON) {
                return true;
            } else {
                //the rule application was unsuccessful, so return the token list to the state in which it was before this function call
                globalTokens->Act = activeElementRuleApplication;
                return false;
            }
        } else {
            globalTokens->Act = activeElementRuleApplication;
            return false;
        }
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleFuncDef(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == BRACKET && token->data.bracket.name == '(') {
        if (ruleFuncDefParams()) {
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '{') {
                if (ruleStListDecl()) {
                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == '}') {
                        return true;
                    }
                } else {
                    //rule stListDeclaration was not applied, but it has an epsilon rule... so check if the token is '}'(in this particular case...)
                    //if so, the source the rule is applied
                    //note tha the condition with the bracket is not duplicated as it seems... the condition below works with different tokens
                    //the bracket below is the only token after the stListDecl which is allowed and represents the rule
                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == '}') {
                        //return the bracket
                        returnCachedTokens(1);

                        return true;
                    } else {
                        //the token was not an '}' so there were some other tokens
                        //so the syntax analysis is over because of syntax error
                        globalTokens->Act = activeElementRuleApplication;
                        return false;
                    }
                }
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleStListDecl(){
    //todo:!! <ST_LIST_DECL> -> EPSILON

    //<ST_LIST_DECL> -> <TYPE><ID><DECL><ST_LIST_DECL>
    if (ruleTypeDouble() || ruleTypeInt() || ruleTypeString()) {
        if (ruleId()) {
            if (ruleDecl()) {
                if (ruleStListDecl()) {
                    return true;
                }
            }
        }
    //<ST_LIST_DECL> -> <STAT> <ST_LIST_DECL>
    } else if (ruleStat()) {
        if (ruleStListDecl()) {
            return true;
        }
    //<ST_LIST_DECL> -> { <ST_LIST> }
    } else {
        TOKEN *token = getCachedToken();
        if (token->type == BRACKET && token->data.bracket.name == '{') {
            if (ruleStList()) {
                if (token->type == BRACKET && token->data.bracket.name == '}') {
                    return true;
                }
            }
        } else {
            returnCachedTokens(1);
            return false;
        }
    }

    return false;
}

bool ruleStList(){
    //<ST_LIST> -> <STAT> <ST_LIST>
    if (ruleStat()) {
        if (ruleStList()) {
            return true;
        }
    //<ST_LIST>-> { <ST_LIST> }
    } else {
        TOKEN *token = getCachedToken();
        if (token->type == BRACKET && token->data.bracket.name == '{') {
            if (ruleStList()) {
                if (token->type == BRACKET && token->data.bracket.name == '}') {
                    return true;
                }
            }
        } else {
            returnCachedTokens(1);
            return false;
        }
    }
    //todo:!!!<ST_LIST> -> EPSILON

}

//copied from rulePropDef
bool ruleDecl(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        return true;
    } else if (token->type == OPERATOR_ASSIGN) {
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: rep;ace dummy test with actual list of instruction
        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
        ListInit(dummyInstructionLIst);

        if (parseExpression(globalTokens, dummyInstructionLIst)) {
            if (token->type == SEMICOLON) {
                return true;
            } else {
                //the rule application was unsuccessful, so return the token list to the state in which it was before this function call
                globalTokens->Act = activeElementRuleApplication;
                return false;
            }
        } else {
            globalTokens->Act = activeElementRuleApplication;
            return false;
        }
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleStat(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
    ListInit(dummyInstructionLIst);

    //<STAT> -> <ID><STAT_BEGINNING_ID>;
    if (ruleId() && ruleStatBeginningId()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        //<STAT> -> while ( <EXP> ) { <ST_LIST> }
        if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "while")) {
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                if (parseExpression(globalTokens, dummyInstructionLIst)) {
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            if (ruleStList()) {
                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }

        //<STAT> -> if ( <EXP> ) { <ST_LIST> } else { <ST_LIST> }
        } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "if")) {
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                if (parseExpression(globalTokens, dummyInstructionLIst)) {
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            if (ruleStList()) {
                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "else")) {
                                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                                            if (ruleStList()) {
                                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        //<STAT> -> return <EXP>;
        } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "return")) {
            if (parseExpression(globalTokens, dummyInstructionLIst)) {
                if (token->type == SEMICOLON) {
                    return true;
                }
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFuncCall(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    //<FUNC_CALL> -> <ID> ( <FUNC_PARAMS>
    if (ruleId()) {
        TOKEN *token = getCachedToken();
        if (token->type == BRACKET && token->data.bracket.name == '('){
            if (ruleFuncParams()) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFuncParams(){
    //<FUNC_PARAMS> -> <PARAM>
    //<FUNC_PARAMS> -> <FUNCTION_CALL_END>
    return ruleParam() || ruleFunctionCallEnd();
}

bool ruleParam(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
    ListInit(dummyInstructionLIst);

    //<PARAM> -> <EXP> <AFTER_FUNCTION_CALL_EXP>
    //<PARAM> -> <ID> <AFTER_FUNCTION_CALL_EXP>



    if (parseExpression(globalTokens, dummyInstructionLIst)) {
        if (ruleAfterFunctionCallExp()) {
            return true;
        }
    } else if (ruleId()) {
        if (ruleAfterFunctionCallExp()) {
            return true;
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFunctionCallEnd(){
//    <FUNCTION_CALL_END> -> )
    TOKEN *token = getCachedToken();

    if (token->type == BRACKET && token->data.bracket.name == ')') {
        return true;
    }

    returnCachedTokens(1);
    return false;
}

bool ruleAfterFunctionCallExp(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    //<AFTER_FUNCTION_CALL_EXP> -> <FUNCTION_CALL_END>
    //<AFTER_FUNCTION_CALL_EXP> -> , <PARAM>

    if (ruleFunctionCallEnd()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        if (token->type == SEPARATOR) {
            if (ruleParam()) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFuncDefParams(){
    //<FUNC_DEF_PARAMS> -> <DEF_PARAM>
    //<FUNC_DEF_PARAMS> -> <FUNCTION_DEF_END>

    return ruleDefParam() || ruleFunctionDefEnd();
}

bool ruleDefParam(){
    //<DEF_PARAM> -> <TYPE> <ID> <DEF_PARAM_BEGIN_TI>
    if (ruleTypeInt() || ruleTypeDouble() || ruleTypeString()) {
        if (ruleId()) {
            if (ruleDefParamBeginTi()) {
                return true;
            }
        }
    }

    //no need to return tokens
    return false;
}

bool ruleDefParamBeginTi(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    //<DEF_PARAM_BEGIN_TI> ->  <FUNCTION_DEF_END>
    //<DEF_PARAM_BEGIN_TI> -> , <DEF_PARAM>

    if (ruleFunctionDefEnd()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        if(token->type == SEPARATOR) {
            if (ruleDefParam()) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFunctionDefEnd(){
    //<FUNCTION_DEF_END> -> ) <ST_LIST_DECL>
    TOKEN *token = getCachedToken();

    if (token->type == BRACKET && token->data.bracket.name == ')') {
        if (ruleStListDecl()) {
            return true;
        }
    }

    returnCachedTokens(1);
    return false;
}

bool ruleTypeVoid() {
    if (ruleTypeInt() || ruleTypeDouble() || ruleTypeString()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();
        if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "void")) {
            return true;
        } else {
            returnCachedTokens(1);
            return false;
        }
    }
}

bool ruleDefinitionStart() {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    //<DEFINITION_START> -> void <ID><FUNC_DEF>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "void")) {
        if (ruleId()) {
            if (ruleFuncDef()) {
                return true;
            }
        }
    //<DEFINITION_START> -> string <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "string")) {
        if (ruleId()) {
            if (ruleDefinition()) {
                return true;
            }
        }
    //<DEFINITION_START> -> int <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "id")) {
        if (ruleId()) {
            if (ruleDefinition()) {
                return true;
            }
        }
    //<DEFINITION_START> -> double <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        if (ruleId()) {
            if (ruleDefinition()) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleStatBeginningId() {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
    ListInit(dummyInstructionLIst);
    //<STAT_BEGINNING_ID> -> <FUNC_CALL>
    //<STAT_BEGINNING_ID> -> =<EXP>

    if (ruleFuncCall()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        if(token->type == OPERATOR_ASSIGN) {
            if (parseExpression(globalTokens, dummyInstructionLIst)) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
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

//maybe not the best idea...
//    TOKEN *token = getCachedToken();
//
//    //check if the file is not empty
//    if (token->type = EOF) {
//        printf("missing class Main\n");
//        exit(3);
//    }
//    returnCachedTokens(1);


    //call function for class
}



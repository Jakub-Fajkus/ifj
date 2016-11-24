//
// Created by Jakub Fajkus on 12.11.16.
//
#include "SyntacticalAnalyzer.h"
#include "ifj16.h"
#include "Debug.h"
#include "SemanticalAnalyzer.h"
#include "LexicalAnalyzerStructures.h"
#include "BasicStructures.h"
#include "SymbolTable.h"
#include <stdbool.h>

#define stringEquals(x,y) (strcmp(x, y) == 0)

tDLList *globalTokens;

SYMBOL_TABLE_NODEPtr *globalSymbolTable;
SYMBOL_TABLE_NODEPtr *actualSymbolTable;
SYMBOL_TABLE_FUNCTION *actualFunction;

bool firstPass = true;

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

void makeFirstPass();

void makeSecondPass();

void runSyntacticalAnalysis(char *fileName) {
    globalTokens = getAllTokens(fileName);
    tDLList *tokens = globalTokens; //for debugger

    initializeSymbolTable(&globalSymbolTable);
    initializeSymbolTable(&actualSymbolTable);
//    SYMBOL_TABLE_FUNCTION *function = createAndInsertFunction(globalSymbolTable, "Main.run", TYPE_VOID, 0, NULL);
//    actualFunction = function;
//    actualFunction = function;

    makeFirstPass();
//    makeSecondPass();

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

bool ruleId(char **name) {
    TOKEN *token = getCachedToken();


    if (token->type == IDENTIFIER) {
        *name = token->data.identifier.name;
        printf("id: %s\n", *name);
        return true;
    }

    if(token->type == IDENTIFIER_FULL) {
        char *classWithDot = stringConcat(token->data.identifierFull.class, ".");
        char *fullName = stringConcat(classWithDot, token->data.identifierFull.name);
        free(classWithDot);

        *name = fullName;
        printf("id: %s\n", *name);
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleTypeString(DATA_TYPE *type) {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "String")) {
        *type = TYPE_STRING;
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleTypeDouble(DATA_TYPE *type) {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        *type = TYPE_DOUBLE;
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleTypeInt(DATA_TYPE *type) {
    TOKEN *token = getCachedToken();

    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "int")) {
        *type = TYPE_INT;
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

bool ruleProg(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    char *className;
    //<PROG> -> class <ID> {<CLASS_DEFINITION> } <PROG>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "class")) {
        if (ruleId(&className)) {
            createAndInsertIntVariable(globalSymbolTable, stringConcat(className, ".*"), false);
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '{') {
                if (ruleClassDefinition(className)) {
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

bool ruleClassDefinition(char *className){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    //<CLASS_DEFINITION> -> static <DEFINITION_START> <CLASS_DEFINITION>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "static")) {
        if (ruleDefinitionStart(className)) {
            if (ruleClassDefinition(className)) {
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
//        token = getCachedToken();
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

bool ruleDefinition(char *className, DATA_TYPE type, char *name, bool *variableInitialized, bool *isFunction){
    //<DEFINITION> -> <PROP_DEF>
    //<DEFINITION> -> <FUNC_DEF>
    if (rulePropDef(variableInitialized)) {
        *isFunction = false;
        return true;
    } else {
        char *classNameWithDot = stringConcat(className, ".");
        SYMBOL_TABLE_FUNCTION *function = createAndInsertFunction(globalSymbolTable, stringConcat(classNameWithDot, name), type, 0, NULL);
        if(ruleFuncDef(function)) {
            *isFunction = true;
            return true;
        } else {
            *isFunction = false;
            return false;
        }

    }
}

bool rulePropDef(bool *variableInitialized){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();
    *variableInitialized = false;

    if (token->type == SEMICOLON) {
        return true;
    } else if (token->type == OPERATOR_ASSIGN) {

        //<PROP_DEF> ->  = <EXP>;
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: replace dummy test with actual list of instruction
        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
        ListInit(dummyInstructionLIst);

        char* resultVariableName;

        printf("calling analyzeExpression from rulePropDef\n");
        if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
            token = getCachedToken();
            if (token->type == SEMICOLON) {
                *variableInitialized = true;
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

bool ruleFuncDef(SYMBOL_TABLE_FUNCTION *function){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();
    actualFunction = function;
    actualSymbolTable = &function->localSymbolTable;

    //<FUNC_DEF> -> ( <FUNC_DEF_PARAMS> { <ST_LIST_DECL> }
    if (token->type == BRACKET && token->data.bracket.name == '(') {
        if (ruleFuncDefParams(function)) {
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '{') {
                if (ruleStListDecl(function)) {
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

bool ruleStListDecl(SYMBOL_TABLE_FUNCTION *function){
    //<ST_LIST_DECL> -> <TYPE><ID><DECL><ST_LIST_DECL>
    DATA_TYPE type;
    char *name;

    if (ruleTypeDouble(&type) || ruleTypeInt(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            if (ruleDecl(function, type, name)) {
                if (ruleStListDecl(function)) {
                    return true;
                }
            }
        }
    //<ST_LIST_DECL> -> <STAT> <ST_LIST_DECL>
    } else if (ruleStat()) {
        if (ruleStListDecl(function)) {
            return true;
        }
    } else {
        TOKEN *token = getCachedToken();
        //<ST_LIST_DECL> -> { <ST_LIST> }
        if (token->type == BRACKET && token->data.bracket.name == '{') {
            if (ruleStList()) {
                if (token->type == BRACKET && token->data.bracket.name == '}') {
                    return true;
                }
            }
        //<ST_LIST> -> EPSILON
        } else if(token->type == BRACKET && token->data.bracket.name == '}') {
            returnCachedTokens(1);
            return true;

        // FAIL
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

    } else {
        TOKEN *token = getCachedToken();
        //<ST_LIST>-> { <ST_LIST> }
        if (token->type == BRACKET && token->data.bracket.name == '{') {
            if (ruleStList()) {
                if (token->type == BRACKET && token->data.bracket.name == '}') {
                    return true;
                }
            }
        //<ST_LIST> -> EPSILON
        } else if(token->type == BRACKET && token->data.bracket.name == '}') {
            returnCachedTokens(1);
            return true;
        // FAIL
         }
    }

    returnCachedTokens(1);
    return false;
}

bool ruleDecl(SYMBOL_TABLE_FUNCTION *function, DATA_TYPE type, char *variableName){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        createAndInsertVariable(&function->localSymbolTable, variableName, type, false);
        return true;
    } else if (token->type == OPERATOR_ASSIGN) {
        //<DECL> -> = <EXP>;
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: rep;ace dummy test with actual list of instruction
        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
        ListInit(dummyInstructionLIst);
        char* resultVariableName;

        printf("calling analyzeExpression from ruleDecl\n");
        if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
            token = getCachedToken();

            //<DECL> -> ;
            if (token->type == SEMICOLON) {
                createAndInsertVariable(&function->localSymbolTable, variableName, type, true);
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

    char *name;

    //<STAT> -> <ID><STAT_BEGINNING_ID>;
    if (ruleId(&name) && ruleStatBeginningId()) {
        TOKEN *token = getCachedToken();
        if(token->type == SEMICOLON) {
            return true;
        }
    } else {
        TOKEN *token = getCachedToken();

        //<STAT> -> while ( <EXP> ) { <ST_LIST> }
        if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "while")) {
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                char* resultVariableName;
                printf("calling analyzeExpression from ruleStat <STAT> -> while ( <EXP> ) { <ST_LIST> }\n");
                if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();
                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            if (ruleStList()) {
                                token = getCachedToken();
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
            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                char* resultVariableName;
                printf("calling analyzeExpression from ruleStat <STAT> -> if ( <EXP> ) { <ST_LIST> } else { <ST_LIST> }\n");
                if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();
                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            if (ruleStList()) {
                                token = getCachedToken();
                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                    token = getCachedToken();
                                    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "else")) {
                                        token = getCachedToken();
                                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                                            if (ruleStList()) {
                                                token = getCachedToken();
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
        //<STAT> -> return <EXP_SEMICOLON>
        } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "return")) {
//            char* resultVariableName;
            if (ruleExpSemicolon()) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleExpSemicolon() {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        return true;
    } else {
        returnCachedTokens(1);

        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
        ListInit(dummyInstructionLIst);
        char *resultVariableName;

        printf("calling analyzeExpression from ruleExpSemicolon\n");
        if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
            token = getCachedToken();
            if (token->type == SEMICOLON){
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFuncCall(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    //<FUNC_CALL> -> ( <FUNC_PARAMS>
    TOKEN *token = getCachedToken();
    if (token->type == BRACKET && token->data.bracket.name == '('){
        if (ruleFuncParams()) {
            return true;
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleFuncParams(){
    //<FUNC_PARAMS> -> <PARAM>
    //<FUNC_PARAMS> -> <FUNCTION_CALL_END>
    return ruleFunctionCallEnd() || ruleParam();
}

bool ruleParam(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
    ListInit(dummyInstructionLIst);

    char *name;

    //<PARAM> -> <EXP> <AFTER_FUNCTION_CALL_EXP>
    //<PARAM> -> <ID> <AFTER_FUNCTION_CALL_EXP>
    char* resultVariableName;
    printf("calling analyzeExpression from ruleParam\n");
    if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
        if (ruleAfterFunctionCallExp()) {
            return true;
        }
    } else if (ruleId(&name)) {
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

bool ruleFuncDefParams(SYMBOL_TABLE_FUNCTION *function){
    //<FUNC_DEF_PARAMS> -> <DEF_PARAM>
    //<FUNC_DEF_PARAMS> -> <FUNCTION_DEF_END>
    return ruleDefParam(function) || ruleFunctionDefEnd();
}

bool ruleDefParam(SYMBOL_TABLE_FUNCTION *function){

    DATA_TYPE type;
    char *name;
    //<DEF_PARAM> -> <TYPE> <ID> <DEF_PARAM_BEGIN_TI>
    if (ruleTypeInt(&type) || ruleTypeDouble(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            DLPostInsert(function->parameters, *createFunctionParamListElement(type, name));
            if (ruleDefParamBeginTi(function, type, name)) {
                return true;
            }
        }
    }

    //no need to return tokens
    return false;
}

bool ruleDefParamBeginTi(SYMBOL_TABLE_FUNCTION *function, DATA_TYPE type, char *name){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    //<DEF_PARAM_BEGIN_TI> ->  <FUNCTION_DEF_END>
    //<DEF_PARAM_BEGIN_TI> -> , <DEF_PARAM>

    if (ruleFunctionDefEnd()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        if(token->type == SEPARATOR) {
            if (ruleDefParam(function)) {
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
        return true;
    }

    returnCachedTokens(1);
    return false;
}

bool ruleTypeVoid(DATA_TYPE *type) {

    if (ruleTypeInt(type) || ruleTypeDouble(type) || ruleTypeString(type)) {
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

bool ruleDefinitionStart(char *className) {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    char *functionOrPropertyName;
    DATA_TYPE type;
    //<DEFINITION_START> -> void <ID><FUNC_DEF>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "void")) {
        if (ruleId(&functionOrPropertyName)) {
            char *classNameWithDot = stringConcat(className, ".");
            tDLList *params = malloc(sizeof(tDLList));
            ListInit(params);
            SYMBOL_TABLE_FUNCTION *function = createAndInsertFunction(globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), TYPE_VOID, 0, NULL);
            if (ruleFuncDef(function)) {
                return true;
            }
        }
    //<DEFINITION_START> -> string <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "String")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
                type = TYPE_STRING;
                char *classNameWithDot = stringConcat(className, ".");
                if(!isFunction) {
                    createAndInsertStringVariable(globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
                }
                return true;
            }
        }
    //<DEFINITION_START> -> int <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "int")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
                type = TYPE_STRING;
                char *classNameWithDot = stringConcat(className, ".");
                if(!isFunction) {
                    createAndInsertIntVariable(globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
                }
                return true;
            }
        }
    //<DEFINITION_START> -> double <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
                type = TYPE_STRING;
                char *classNameWithDot = stringConcat(className, ".");
                if(!isFunction) {
                    createAndInsertDoubleVariable(globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
                }
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
        char* resultVariableName;

        if(token->type == OPERATOR_ASSIGN) {
            char *name;
            printf("calling analyzeExpression from ruleStatBeginningId\n");
            int code = analyzeExpression(dummyInstructionLIst, resultVariableName);
            if(-1 == code) {
                if(ruleId(&name)) {
                    if (ruleFuncCall()) {
                        return true;
                    }
                }
            } else if (code == 1) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

void testTokens() {
    tDLList *tokens = getAllTokens("test1-0.txt");
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

void makeFirstPass() {
    ListFirst(globalTokens);

    bool result = ruleProg();

    if(result == false) {
        exit(2);
    }

    checkIfFunctionRunExists();
}

void makeSecondPass() {
    ListFirst(globalTokens);

    bool result = ruleProg();

    if(result == 0) {
        exit(2);
    }
}

int analyzeExpression(tDLList *instructionList, char *resultVariableName) {
    int code = parseExpression(instructionList, resultVariableName, globalSymbolTable, actualSymbolTable, actualFunction, firstPass);

    //todo: if code -1
    if (code == 0) {
        return 1;

    //it is a function call
    } else if (code == -1) {
        return -1;
    }
    else {
        exit(code);
    }
}


void addGlobalVariablesToFrame() {

}
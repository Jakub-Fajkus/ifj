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

struct SYMBOL_TABLE_NODE *globalSymbolTable;
SYMBOL_TABLE_FUNCTION *actualFunction;
struct tDLListStruct *mainInstructionList;

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
//void createInstructionsForGlobalVariables(struct tDLListStruct *mainInstructionList);
LIST_ELEMENT wrapInstructionIntoListElement(INSTRUCTION *instr);

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
            createAndInsertIntVariable(&globalSymbolTable, stringConcat(className, ".*"), false);
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

bool ruleDefinition(char *className, DATA_TYPE type, char *functionOrPropertyName, bool *variableInitialized, bool *isFunction){
    //<DEFINITION> -> <PROP_DEF>
    //<DEFINITION> -> <FUNC_DEF>
    if (rulePropDef(variableInitialized, type, functionOrPropertyName, className)) {
        *isFunction = false;
        return true;
    } else {
        char *classNameWithDot = stringConcat(className, ".");
        SYMBOL_TABLE_FUNCTION *function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), type, 0, NULL, NULL);
        actualFunction = function;
        if(ruleFuncDef()) {
            *isFunction = true;
            return true;
        } else {
            *isFunction = false;
            return false;
        }

    }
}

//only for static variables
bool rulePropDef(bool *variableInitialized, DATA_TYPE variableType, char *variableName, char *className){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();
    *variableInitialized = false;
    char *fullyQualifiedVariableName = stringConcat(stringConcat(className, "."), variableName);

    if (token->type == SEMICOLON) {
        if (firstPass) {

            createAndInsertVariable(&globalSymbolTable, fullyQualifiedVariableName, variableType, false);
            ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createGlobalVariable(fullyQualifiedVariableName, variableType)));
            // noo need to check type
        }
        return true;
    } else if (token->type == OPERATOR_ASSIGN) {

        //<PROP_DEF> ->  = <EXP>;
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: replace dummy test with actual list of instruction
//        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
//        ListInit(dummyInstructionLIst);

        char* resultVariableName;

        printf("calling analyzeExpression from rulePropDef\n");
        if (analyzeExpression(mainInstructionList, resultVariableName)) {
            token = getCachedToken();
            if (token->type == SEMICOLON) {
                *variableInitialized = true;
                createAndInsertVariable(&globalSymbolTable, fullyQualifiedVariableName, variableType, true);
                ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createInstructionAssign(fullyQualifiedVariableName, resultVariableName)));
                //todo: check type of the result variable with variable declaration



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

    //<FUNC_DEF> -> ( <FUNC_DEF_PARAMS> { <ST_LIST_DECL> }
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

//todo was function redundant? global variable should do well?
bool ruleStListDecl(){
    //<ST_LIST_DECL> -> <TYPE><ID><DECL><ST_LIST_DECL>
    DATA_TYPE type;
    char *name;

    if (ruleTypeDouble(&type) || ruleTypeInt(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            if (ruleDecl(type, name)) {
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

bool ruleDecl(DATA_TYPE type, char *variableName){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        if (!firstPass) {
            createAndInsertVariable(&actualFunction->localSymbolTable, variableName, type, false);
            //no need to check types
        }

        return true;
    } else if (token->type == OPERATOR_ASSIGN) {
        //<DECL> -> = <EXP>;
        //todo: check if the next tokens are expression
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        //todo: rep;ace dummy test with actual list of instruction
//        tDLList *dummyInstructionLIst = malloc(sizeof(tDLList));
//        ListInit(dummyInstructionLIst);
        char* resultVariableName;

        printf("calling analyzeExpression from ruleDecl\n");
        if (analyzeExpression(actualFunction->instructions, resultVariableName)) { //todo
            token = getCachedToken();

            //<DECL> -> ;
            if (token->type == SEMICOLON) {
                if (!firstPass) {
                    createAndInsertVariable(&actualFunction->localSymbolTable, variableName, type, true);
                    //todo: check type of the result variable with variable declaration
                }

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
    char* resultVariableName;
    printf("calling analyzeExpression from ruleParam\n");
    if (analyzeExpression(dummyInstructionLIst, resultVariableName)) {
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

    DATA_TYPE type;
    char *name;
    //<DEF_PARAM> -> <TYPE> <ID> <DEF_PARAM_BEGIN_TI>
    if (ruleTypeInt(&type) || ruleTypeDouble(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            DLPostInsert(actualFunction->parameters, *createFunctionParamListElement(type, name));
            if (ruleDefParamBeginTi(type, name)) {
                return true;
            }
        }
    }

    //no need to return tokens
    return false;
}

bool ruleDefParamBeginTi(DATA_TYPE type, char *name){
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
            SYMBOL_TABLE_FUNCTION *function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), TYPE_VOID, 0, NULL, NULL);
            actualFunction = function;
            if (ruleFuncDef()) {
                return true;
            }
        }
    //<DEFINITION_START> -> string <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "String")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            type = TYPE_STRING;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
//                char *classNameWithDot = stringConcat(className, ".");
//                if(!isFunction) {
//                    createAndInsertStringVariable(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
//                }
                return true;
            }
        }
    //<DEFINITION_START> -> int <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "int")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            type = TYPE_STRING;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
//                char *classNameWithDot = stringConcat(className, ".");
//                if(!isFunction) {
//                    createAndInsertIntVariable(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
//                }
                return true;
            }
        }
    //<DEFINITION_START> -> double <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            type = TYPE_STRING;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
//                char *classNameWithDot = stringConcat(className, ".");
//                if(!isFunction) {
//                    createAndInsertDoubleVariable(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), propertyInitialized);
//                }
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

void runSyntacticalAnalysis(char *fileName) {
    globalTokens = getAllTokens(fileName);

    initializeSymbolTable(globalSymbolTable);

    makeFirstPass();
    //makeSecondPass();
}


void makeFirstPass() {
    //set active to the first element
    ListFirst(globalTokens);
    //create main instruction list
    mainInstructionList = malloc(sizeof(struct tDLListStruct));
    ListInit(mainInstructionList);
    //create instruction Instruction_Create_GlobalFrame_And_LocalStack
    INSTRUCTION *createGlobalFrameInstruction = createFirstInstruction();
    ListFirst(mainInstructionList);
    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createGlobalFrameInstruction));

    bool result = ruleProg();

    if(result == false) {
        exit(2);
    }

    semanticCheckForFunctionRun();

    //just "testing"
    printListOfInstructions(mainInstructionList);


}

void makeSecondPass() {
    ListFirst(globalTokens);
    firstPass = false;

    bool result = ruleProg();

    if(result == 0) {
        exit(2);
    }

    //call interpret
    //todo:
}

int analyzeExpression(tDLList *instructionList, char *resultVariableName/*,DATA_TYPE *resultVariableType*/) {
    DATA_TYPE resultDatatype;
    int code = parseExpression(instructionList, resultVariableName, &resultDatatype, firstPass);

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


//void createInstructionsForGlobalVariables(struct tDLListStruct *mainInstructionList) {
//    struct STACK_STR *symbolTableStacked = BTInorder(*globalSymbolTable);
//
////    Create_GlobalFrame_And_LocalStack
//    INSTRUCTION *createGlobalFrameInstruction = createFirstInstruction();
//    ListFirst(mainInstructionList); //just in case
//
//    //insert the first instruction
//    DLPostInsert(mainInstructionList, wrapInstructionIntoListElement(createGlobalFrameInstruction));
//
//    //insert global variables
//    while (!stackEmpty(symbolTableStacked)) {
//        struct STACK_ELEMENT actualElement;
//        stackTop(symbolTableStacked, &actualElement);
//
//        //filter all records for classe itself
//        if (actualElement.data.symbolTableNode->data->type == TREE_NODE_VARIABLE) {
//            printf("variable name: %s\n", actualElement.data.symbolTableNode->data->item->variable->name);
//            if (ifj16_find(actualElement.data.symbolTableNode->data->item->variable->name, ".*")) {
//
//                stackPop(symbolTableStacked);
//                continue;
//            }
//        } else {
//            printf("function name: %s\n", actualElement.data.symbolTableNode->data->item->function->name);
//        }
//
////        DLPostInsert(mainInstructionList, wrapInstructionIntoListElement(createGlobalVariable(actualElement.data.symbolTableNode->data->item->variable->name, actualElement.data.symbolTableNode->data->item->variable->type)));
//
//
//        stackPop(symbolTableStacked);
//    }
//}


LIST_ELEMENT wrapInstructionIntoListElement(INSTRUCTION *instr) {
    LIST_ELEMENT newElement;

    newElement.type = LIST_ELEMENT_TYPE_INSTRUCTION;
    newElement.data.instr = instr;

    return newElement;
}
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
#include "Debug.h"

tDLList *globalTokens;

struct SYMBOL_TABLE_NODE *globalSymbolTable;
struct SYMBOL_TABLE_FUNCTION_STR *actualFunction;
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
void addIfj16Functions();

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

/**
 * no instruction to generate
 */
bool ruleId(char **name) {
    TOKEN *token = getCachedToken();

    if (token->type == IDENTIFIER) {
        *name = token->data.identifier.name;
        debugPrintf("id: %s\n", *name);
        return true;
    }

    if(token->type == IDENTIFIER_FULL) {
        char *classWithDot = stringConcat(token->data.identifierFull.class, ".");
        char *fullName = stringConcat(classWithDot, token->data.identifierFull.name);
        free(classWithDot);

        *name = fullName;
        debugPrintf("id: %s\n", *name);
        return true;
    } else {
        returnCachedTokens(1);
        return false;
    }
}

/**
 * no instruction to generate
 */
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

/**
 * no instruction to generate
 */
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

/**
 * no instruction to generate
 */
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

/**
 * Instruction_Create_Global_Variable for CLASS.*
 */
bool ruleProg(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    char *className;
    //<PROG> -> class <ID> {<CLASS_DEFINITION> } <PROG>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "class")) {
        if (ruleId(&className)) {
            if(firstPass) {
                //insert a record for the class itself
                createAndInsertIntVariable(&globalSymbolTable, stringConcat(className, ".*"), false);
                //create instruction to insert global variable
                ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createGlobalVariable(stringConcat(className, ".*"), TYPE_INT)));
            }

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

/**
 * no instruction to generate
 */
bool ruleClassDefinition(char *className){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    //<CLASS_DEFINITION> -> static <DEFINITION_START> <CLASS_DEFINITION>
    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "static")) {
        if (ruleDefinitionStart(className)) {
            //recursive call
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
        struct SYMBOL_TABLE_FUNCTION_STR *function = NULL;

        if(firstPass) {
            function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), type, 0, NULL, NULL);
        } else {
            function = semantic_getFunction(stringConcat(classNameWithDot, functionOrPropertyName));
        }

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
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        debugPrintf("calling analyzeExpression from rulePropDef\n");
        char* resultVariableName = NULL;
        DATA_TYPE resultVariableType;
        debugPrintf("calling analyzeExpression from rulePropDef\n");
        if (analyzeExpression(mainInstructionList, &resultVariableName, &resultVariableType)) {
            token = getCachedToken();
            if (token->type == SEMICOLON) {
                *variableInitialized = true;
                if(firstPass) {
                    createAndInsertVariable(&globalSymbolTable, fullyQualifiedVariableName, variableType, true);

                    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createInstrAssign(fullyQualifiedVariableName, resultVariableName)));
                }
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

/**
 * here the whole function definition takes place
 * the actualFunction pointer is already set to the function which is beeing processed
 *
 */
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


/**
 * the body of each function
 * actualFunction is set already
 *
 * there will be plenty of instructions!
 */
bool ruleStListDecl(){
    //<ST_LIST_DECL> -> <TYPE><ID><DECL><ST_LIST_DECL>
    DATA_TYPE type;
    char *name;

    if (ruleTypeDouble(&type) || ruleTypeInt(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            if (ruleDecl(type, name)) {
                //recursive call
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
            if (ruleStList(NULL)) {
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

bool ruleStList(tDLList *listOfInstuctions){

    //insert actual instruction into listOfInstructions?
    //fail there cant..
    //zarazku?


    //<ST_LIST> -> <STAT> <ST_LIST>
    if (ruleStat()) {
        if (ruleStList(listOfInstuctions)) {
            return true;
        }

    } else {
        TOKEN *token = getCachedToken();
        //<ST_LIST>-> { <ST_LIST> }
        if (token->type == BRACKET && token->data.bracket.name == '{') {
            if (ruleStList(listOfInstuctions)) {
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

/**
 * function for declaration of local variable
 * actualFunction is already set
 *
 * Instruction_Create_Local_Variable
 * Instruction_Assign
 */
bool ruleDecl(DATA_TYPE declaredType, char *variableName){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        if (firstPass) {
            createAndInsertVariable(&actualFunction->localSymbolTable, variableName, declaredType, false);
            //no need to check types
        } else {
            ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createLocalVariable(variableName, declaredType)));
        }

        return true;
    } else if (token->type == OPERATOR_ASSIGN) {
        //<DECL> -> = <EXP>;
        //the check should return true if the expression was successfully parsed, false otherwise
        //the check should set the active token to the token which is right after the expression(example: tokens:"3+4)", the active token should be ')'
        char* resultVariableName = NULL;
        DATA_TYPE resultVariableType;
        debugPrintf("calling analyzeExpression from ruleDecl\n");
        //no function call allowed! thank god...
        if (analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType)) {
            //the resultVariableName contains the variable name which contains the result of the expression
            token = getCachedToken();

            //<DECL> -> ;
            if (token->type == SEMICOLON) {
                if (firstPass) {
                    //insert the local variable to the symbol table
                    createAndInsertVariable(&actualFunction->localSymbolTable, variableName, declaredType, true);
                } else {
                    //create instruction to assign the temporal variable created by expAnalyzer to the local variable which was defined right now
                    ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrAssign(variableName, resultVariableName)));
                    //todo: check type of the result variable with variable declaration resultVariableType == declaredType or is convertible to type
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

/**
 * one statement in a function body
 */
bool ruleStat(){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    char *functionOrVariableName = NULL;

    tDLList *listOfInsTrue = malloc(sizeof(tDLList));
    tDLList *listOfInsFalse = malloc(sizeof(tDLList));
    ListInit(listOfInsTrue);
    ListInit(listOfInsFalse);


    //<STAT> -> <ID><STAT_BEGINNING_ID>;
    if (ruleId(&functionOrVariableName) && ruleStatBeginningId(functionOrVariableName)) {
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
                DATA_TYPE resultVariableType;
                debugPrintf("calling analyzeExpression from ruleStat <STAT> -> while ( <EXP> ) { <ST_LIST> }\n");
                if (analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType)) {
                    token = getCachedToken();

                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();
                        if (token->type == BRACKET && token->data.bracket.name == '{'){

                            if (ruleStList(listOfInsTrue)) {
                                token = getCachedToken();
                                if (token->type == BRACKET && token->data.bracket.name == '}') {

                                   /* if(!firstPass){
                                        ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrWhile(resultVariableName,)));
                                    }*/

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
                DATA_TYPE resultVariableType;
                debugPrintf("calling analyzeExpression from ruleStat <STAT> -> if ( <EXP> ) { <ST_LIST> } else { <ST_LIST> }\n");
                if (analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType)) {

                    //result of expression, temp which would be used in createInsIf
                   /* if(firstPass){
                        ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createLocalVariable(resultVariableName, resultVariableType)));
                    }*/

                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();

                        if (token->type == BRACKET && token->data.bracket.name == '{'){

                            //list of instructions for true
                            if (ruleStList(listOfInsTrue)) {

                                token = getCachedToken();
                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                    token = getCachedToken();
                                    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "else")) {
                                        token = getCachedToken();
                                        if (token->type == BRACKET && token->data.bracket.name == '{'){

                                            //list of instructions for false
                                            if (ruleStList(listOfInsFalse)) {
                                                token = getCachedToken();
                                                if (token->type == BRACKET && token->data.bracket.name == '}') {

                                                    if(!firstPass){
                                                        ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrIf(resultVariableName, listOfInsTrue, listOfInsFalse)));
                                                    }

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

        char *resultVariableName = NULL;
        DATA_TYPE resultVariableType;

        printf("calling analyzeExpression from ruleExpSemicolon\n");
        if (analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType)) {
            token = getCachedToken();
            if (token->type == SEMICOLON){
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

/**
 *
 */
bool ruleFuncCall(char *calledFunctionName, char *assignReturnValueToVariable){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    struct SYMBOL_TABLE_FUNCTION_STR *functionToCall = NULL;

    //<FUNC_CALL> -> ( <FUNC_PARAMS>
    TOKEN *token = getCachedToken();
    if (token->type == BRACKET && token->data.bracket.name == '('){
        if (!firstPass) {
            //set activity to the first parameter
            functionToCall = semantic_getFunction(calledFunctionName);
            ListFirst(functionToCall->parameters);
        }

        tDLList *parameters = malloc(sizeof(tDLList));
        ListInit(parameters);

        if (ruleFuncParams(functionToCall, parameters)) {
            if(!firstPass) {
                semantical_checkFunctionCall(functionToCall->parameters, parameters);

                //you have a list of PARAM *
                if(isFunctionFromIfj16(calledFunctionName)) {
                    //todo: generate instruction to call conrete function
                    createInstructionsToCallIfj16Function(calledFunctionName, actualFunction->instructions, parameters, assignReturnValueToVariable);
                } else {
                    //todo: generate instructions to push local variables
                    ListFirst(parameters);
                    ListFirst(functionToCall->parameters);

                    while(DLActive(parameters) ) {
                        FUNCTION_PARAMETER *param = functionToCall->parameters->Act->element.data.parameter;
                        ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createLocalVariable(param->name, param->type)));

                        //move to the next parameter
                        ListSuccessor(parameters);
                        ListSuccessor(functionToCall->parameters);
                    }

                    ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrCallFunction(functionToCall->instructions, assignReturnValueToVariable)));
                }
            }

            return true;
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

/**
 * generates
 */
bool ruleFuncParams(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters){
    //<FUNC_PARAMS> -> <PARAM>
    //<FUNC_PARAMS> -> <FUNCTION_CALL_END>
    //create local frame of the function

    if (ruleFunctionCallEnd()) {
        return true;
    } else if (ruleParam(functionToCall, parameters)) {
        return true;
    }

    return false;
}

bool ruleParam(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    //<PARAM> -> <EXP> <AFTER_FUNCTION_CALL_EXP>
    char* resultVariableName = NULL;
    DATA_TYPE resultVariableType;
    printf("calling analyzeExpression from ruleParam\n");
    if (analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType)) {
        if (ruleAfterFunctionCallExp(functionToCall, parameters)) {
            if(!firstPass) {
                ListInsertLast(parameters, createListElementWithFunctionParamameter(resultVariableName, resultVariableType));
            }
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

bool ruleAfterFunctionCallExp(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    //<AFTER_FUNCTION_CALL_EXP> -> <FUNCTION_CALL_END>
    //<AFTER_FUNCTION_CALL_EXP> -> , <PARAM>

    if (ruleFunctionCallEnd()) {
        return true;
    } else {
        TOKEN *token = getCachedToken();

        if (token->type == SEPARATOR) {
            if (ruleParam(functionToCall, parameters)) {
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

/**
 * no instruction is generated
 */
bool ruleFuncDefParams(){
    //<FUNC_DEF_PARAMS> -> <DEF_PARAM>
    //<FUNC_DEF_PARAMS> -> <FUNCTION_DEF_END>
    return ruleDefParam() || ruleFunctionDefEnd();
}

/**
 * no instruction is generated
 */
bool ruleDefParam(){

    DATA_TYPE type;
    char *name;
    //<DEF_PARAM> -> <TYPE> <ID> <DEF_PARAM_BEGIN_TI>
    if (ruleTypeInt(&type) || ruleTypeDouble(&type) || ruleTypeString(&type)) {
        if (ruleId(&name)) {
            //insert function parameter
            DLPostInsert(actualFunction->parameters, *createFunctionParamListElement(type, name));
            if (ruleDefParamBeginTi()) {
                return true;
            }
        }
    }

    //no need to return tokens
    return false;
}

/**
 * no instruction is generated
 */
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

/**
 * no instruction is generated
 */
bool ruleFunctionDefEnd(){
    //<FUNCTION_DEF_END> -> ) <ST_LIST_DECL>
    TOKEN *token = getCachedToken();

    if (token->type == BRACKET && token->data.bracket.name == ')') {
        return true;
    }

    returnCachedTokens(1);
    return false;
}

/**
 * ???? first if branch does not generate instructions(nested rules do!)
 */
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
            struct SYMBOL_TABLE_FUNCTION_STR *function = NULL;
            if(firstPass) {
                function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), TYPE_VOID, 0, NULL, NULL);
            } else {
                function = semantic_getFunction(stringConcat(classNameWithDot, functionOrPropertyName));
            }
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

/**
 * situations:
 * var = EXP; (the variable already exists in the symbol table)
 * fun(...);
 *
 * Instruction_Assign
 * arithmetical and logical instructions created in expressionAnalyzer
 */
bool ruleStatBeginningId(char *functionOrPropertyName) {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    //<STAT_BEGINNING_ID> -> <FUNC_CALL>
    //<STAT_BEGINNING_ID> -> =<EXP>

    if (ruleFuncCall(functionOrPropertyName, NULL)) {
        return true;
    } else {
        TOKEN *token = getCachedToken();
        char* resultVariableName = NULL;

        if(token->type == OPERATOR_ASSIGN) {
            char *functionName = NULL;
            DATA_TYPE resultVariableType;
            printf("calling analyzeExpression from ruleStatBeginningId\n");
            int code = analyzeExpression(actualFunction->instructions, &resultVariableName, &resultVariableType);
            //function call
            if (-1 == code) {
                if(ruleId(&functionName)) {
                    if (ruleFuncCall(functionName, functionOrPropertyName)) {
                        if(!firstPass) {
                            initializeVariable(functionOrPropertyName);
                            //add functionOrPropertyName
                            ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrAssign(functionOrPropertyName, resultVariableName)));
                        }

                        return true;
                    }
                }
                //
            } else if (code == 1) {
                //check if the variable "functionOrPropertyName" exists in a symbol table
                if(!firstPass) {
                    semantic_getVariable(functionOrPropertyName);
                    //generate instructions to assign resultVariableName to functionOrPropertyName
                    initializeVariable(functionOrPropertyName);
                    ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrAssign(functionOrPropertyName, resultVariableName)));
                }
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
    addIfj16Functions();

    makeFirstPass();
}


void makeFirstPass() {
    //set active to the first element
    ListFirst(globalTokens);

    actualFunction = NULL;

    //create main instruction list
    mainInstructionList = malloc(sizeof(struct tDLListStruct));
    ListInit(mainInstructionList);
    //create instruction Instruction_Create_GlobalFrame_And_LocalStack
    ListFirst(mainInstructionList);
    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createFirstInstruction()));

    bool result = ruleProg();

    //todo: create local stack and call function Main.run

    if(result == false) {
        exit(2);
    }

    semanticCheckForFunctionRun();
    makeSecondPass();

    //test
    struct STACK_STR *symbolTableStacked = BTInorder(globalSymbolTable);
    while (!stackEmpty(symbolTableStacked)) {
        struct STACK_ELEMENT actualElement;
        stackTop(symbolTableStacked, &actualElement);

        //filter all records for classe itself
        if (actualElement.data.symbolTableNode->data->type == TREE_NODE_FUNCTION) {
            SYMBOL_TABLE_FUNCTION *fun = actualElement.data.symbolTableNode->data->item->function;
            printf("\n\nprinitng instrucitons of function: %s\n", fun->name);
            ListFirst(fun->instructions);
            while(DLActive(fun->instructions)) {
                printInstruction(fun->instructions->Act->element.data.instr);
                ListSuccessor(fun->instructions);
            }
        }

        stackPop(symbolTableStacked);
    }

    printf("end of printing\n");
    //test



    //just "testing"
    printf("print list of main instructions\n");
    printListOfInstructions(mainInstructionList);


}

void makeSecondPass() {
    printf("\n\n********************************\n********************************\n\nSTARTING WITH THE SECOND PASS!\n\n********************************\n********************************\n");
    ListFirst(globalTokens);
    firstPass = false;

    bool result = ruleProg();

    if(result == 0) {
        exit(2);
    }

    //call interpret
    //todo:
}

/**
 *
 * @param instructionList
 * @param resultVariableName
 * @return 1 = expression ok(1 represents true)
 *         -1 = function call
 */
int analyzeExpression(tDLList *instructionList, char **resultVariableName, DATA_TYPE *resultVariableType) {
    int code = parseExpression(instructionList, resultVariableName, resultVariableType, firstPass);

    if (code == 0) {
        return 1;

    //it is a function call
    } else if (code == -1) {
        return -1;
    }
    else {
        printf("exiting with code %d returned from expressionAnalyzer", code);
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


LIST_ELEMENT createListElementWithFunctionParamameter(char *name, DATA_TYPE type) {
    LIST_ELEMENT newElement;
    FUNCTION_PARAMETER *param = malloc(sizeof(FUNCTION_PARAMETER));
    param->name = name;
    param->type = type;

    newElement.type = LIST_ELEMENT_TYPE_FUNCTION_PARAMETER;
    newElement.data.parameter = param;

    return newElement;
}

void addIfj16Functions() {

    createAndInsertFunction(&globalSymbolTable, "ifj16.readInt", TYPE_INT, 0, NULL, NULL);

    createAndInsertFunction(&globalSymbolTable, "ifj16.readDouble", TYPE_DOUBLE, 0, NULL, NULL);

    createAndInsertFunction(&globalSymbolTable, "ifj16.readString", TYPE_STRING, 0, NULL, NULL);

    tDLList *parametersPrint = malloc(sizeof(tDLList));
    ListInit(parametersPrint);
    ListInsertLast(parametersPrint, createListElementWithFunctionParamameter("string", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.print", TYPE_VOID, 0, parametersPrint, NULL);

    tDLList *parametersLength = malloc(sizeof(tDLList));
    ListInit(parametersLength);
    ListInsertLast(parametersLength, createListElementWithFunctionParamameter("s", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.length", TYPE_INT, 0, parametersLength, NULL);

    tDLList *parametersSubstr = malloc(sizeof(tDLList));
    ListInit(parametersSubstr);
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("s", TYPE_STRING));
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("i", TYPE_INT));
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("n", TYPE_INT));
    createAndInsertFunction(&globalSymbolTable, "ifj16.substr", TYPE_STRING, 0, parametersSubstr, NULL);

    tDLList *parametersCompare = malloc(sizeof(tDLList));
    ListInit(parametersCompare);
    ListInsertLast(parametersCompare, createListElementWithFunctionParamameter("s1", TYPE_STRING));
    ListInsertLast(parametersCompare, createListElementWithFunctionParamameter("s2", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.compare", TYPE_INT, 0, parametersCompare, NULL);

    tDLList *parametersFind = malloc(sizeof(tDLList));
    ListInit(parametersFind);
    ListInsertLast(parametersFind, createListElementWithFunctionParamameter("s", TYPE_STRING));
    ListInsertLast(parametersFind, createListElementWithFunctionParamameter("search", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.find", TYPE_INT, 0, parametersFind, NULL);

    tDLList *parametersSort = malloc(sizeof(tDLList));
    ListInit(parametersSort);
    ListInsertLast(parametersSort, createListElementWithFunctionParamameter("s", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.sort", TYPE_STRING, 0, parametersSort, NULL);

}

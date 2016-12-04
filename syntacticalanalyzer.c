//
// Created by Jakub Fajkus on 12.11.16.
//
#include "syntacticalanalyzer.h"
#include "ifj16.h"
#include "debug.h"
#include "semanticalanalyzer.h"
#include "lexicalanalyzerstructures.h"
#include "basicstructures.h"
#include "symboltable.h"
#include <stdbool.h>
#include "debug.h"
#include "interpret.h"

tDLList *globalTokens;

struct SYMBOL_TABLE_NODE *globalSymbolTable;
struct SYMBOL_TABLE_FUNCTION_STR *actualFunction;
struct tDLListStruct *mainInstructionList;
struct tDLListStruct *actualInstructionList;
struct STACK_STR *returnToVariables;
char* actualClass;

//todo: osetrit datovy typ vyrazu v prikazu return

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
void printAll();

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
            actualClass = className;
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

    actualFunction = NULL;

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
            function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), type, 0, NULL, NULL, 0);

        } else {
            function = semantic_getFunction(stringConcat(classNameWithDot, functionOrPropertyName));
        }

        actualFunction = function;
        actualInstructionList = function->instructions;
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
                    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createGlobalVariable(fullyQualifiedVariableName, variableType)));
                    createAndInsertVariable(&globalSymbolTable, fullyQualifiedVariableName, variableType, true);
                } else{
                    if(!canConvertTypes(variableType, resultVariableType)) {
                        debugPrintf("Incompatible types for assigned");
                        exit(4);
                    }

                    //todo: check type of the result variable with variable declaration
                    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createInstrAssign(fullyQualifiedVariableName, resultVariableName)));
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
            ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createActualLocalVariable(variableName, declaredType)));
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
        if (analyzeExpression(actualInstructionList, &resultVariableName, &resultVariableType)) {
            //the resultVariableName contains the variable name which contains the result of the expression
            token = getCachedToken();

            //<DECL> -> ;
            if (token->type == SEMICOLON) {
                if (firstPass) {
                    //insert the local variable to the symbol table
                    createAndInsertVariable(&actualFunction->localSymbolTable, variableName, declaredType, true);
                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createActualLocalVariable(variableName, declaredType)));
                } else {
                    //create instruction to assign the temporal variable created by expAnalyzer to the local variable which was defined right now
                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrAssign(variableName, resultVariableName)));
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

//    tDLList *listOfInsTrue = malloc(sizeof(tDLList));
//    tDLList *listOfInsFalse = malloc(sizeof(tDLList));
//    ListInit(listOfInsTrue);
//    ListInit(listOfInsFalse);


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
            tDLList *actualInstructionListBackup = actualInstructionList;
            tDLList *whileBodyList = malloc(sizeof(tDLList));
            ListInit(whileBodyList);
            tDLList *whileConditionList = malloc(sizeof(tDLList));
            ListInit(whileConditionList);

            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                char* resultVariableName = NULL;
                DATA_TYPE resultVariableType = TYPE_INT; //just some default value
                actualInstructionList = whileConditionList;
                debugPrintf("calling analyzeExpression from ruleStat <STAT> -> while ( <EXP> ) { <ST_LIST> }\n");
                if (analyzeExpression(actualInstructionList, &resultVariableName, &resultVariableType)) {
                    if(!firstPass && resultVariableType != TYPE_BOOL) {
                        debugPrintf("variable used in the confition of an while statement must be of type bool, type: %d given", resultVariableType);
                        exit(4);
                    }

                    token = getCachedToken();
                    actualInstructionList = actualInstructionListBackup;
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();
                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            actualInstructionList = whileBodyList;
                            if (ruleStList()) {
                                token = getCachedToken();
                                if (token->type == BRACKET && token->data.bracket.name == '}') {

                                    if(!firstPass){
                                        ListInsertLast(actualInstructionListBackup, wrapInstructionIntoListElement(createInstrWhile(resultVariableName, whileConditionList, whileBodyList)));
                                    }

                                    actualInstructionList = actualInstructionListBackup;
                                    return true;
                                }
                            }
                            actualInstructionList = actualInstructionListBackup;
                        }
                    }
                }
                actualInstructionList = actualInstructionListBackup;
            }

        //<STAT> -> if ( <EXP> ) { <ST_LIST> } else { <ST_LIST> }
        } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "if")) {
            tDLList *actualInstructionListBackup = actualInstructionList;
            tDLList *trueList = malloc(sizeof(tDLList));
            ListInit(trueList);
            tDLList *falseList = malloc(sizeof(tDLList));
            ListInit(falseList);
//            INSTRUCTION *whileInstruction = createInstrWhile()

            token = getCachedToken();
            if (token->type == BRACKET && token->data.bracket.name == '(') {
                char* resultVariableName = NULL;
                DATA_TYPE resultVariableType = TYPE_STRING; //just default value
                debugPrintf("calling analyzeExpression from ruleStat <STAT> -> if ( <EXP> ) { <ST_LIST> } else { <ST_LIST> }\n");
                if (analyzeExpression(actualInstructionList, &resultVariableName, &resultVariableType)) {
                    //result of expression, temp which would be used in createInsIf
                    if(!firstPass){
                        if(resultVariableType != TYPE_BOOL) {
                            debugPrintf("variable used in the confition of an if statement must be of type bool, type: %d given", resultVariableType);
                            exit(4);
                        }

                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createActualLocalVariable(resultVariableName, resultVariableType)));
                        //todo: test resultVariableType with bool
                    }

                    token = getCachedToken();
                    if (token->type == BRACKET && token->data.bracket.name == ')') {
                        token = getCachedToken();

                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                            actualInstructionList = trueList;
                            //list of instructions for true
                            if (ruleStList()) {
                                token = getCachedToken();
                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                    actualInstructionList = actualInstructionListBackup;

                                    token = getCachedToken();
                                    if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "else")) {
                                        token = getCachedToken();
                                        if (token->type == BRACKET && token->data.bracket.name == '{'){
                                            actualInstructionList = falseList;
                                            //list of instructions for false
                                            if (ruleStList()) {
                                                token = getCachedToken();
                                                if (token->type == BRACKET && token->data.bracket.name == '}') {
                                                    if(!firstPass){
                                                        ListInsertLast(actualInstructionListBackup, wrapInstructionIntoListElement(createInstrIf(resultVariableName, trueList, falseList)));
                                                    }

                                                    actualInstructionList = actualInstructionListBackup;
                                                    return true;
                                                }
                                            }
                                            actualInstructionList = actualInstructionListBackup;
                                        }
                                    }
                                }
                            }
                            actualInstructionList = actualInstructionListBackup;
                        }
                    }
                }
            }
        //<STAT> -> return <EXP_SEMICOLON>
        } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "return")) {
            char* tempVariableName = NULL;
            DATA_TYPE tempVariableType;

            if (ruleExpSemicolon(&tempVariableName, &tempVariableType)) {
                STACK_ELEMENT *stackElement = (STACK_ELEMENT*)malloc(sizeof(STACK_ELEMENT));
                stackTop(returnToVariables,stackElement);
                stackPop(returnToVariables);
                if(!firstPass) {
                    if(tempVariableName != NULL) {
                        //todo: check for datatypes(typeo f func and type of the result, possible implicit conversions)
                        if (actualFunction->type != tempVariableType) {
                            debugPrintf("incompatible return type of function: %s", actualFunction->name);
                            exit(4);
                        }

                        //the ruleExpSemicolon returned a variable name. in this case the statement was: return <EXP>; and we want to generate the assign instruction
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createActualLocalVariable(stringConcat("#", actualFunction->name), actualFunction->type)));
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrAssign(stringConcat("#", actualFunction->name), tempVariableName)));
                    }

                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrReturnFunction()));
                }
                return true;
            }
        }
    }

    globalTokens->Act = activeElementRuleApplication;
    return false;
}

bool ruleExpSemicolon(char **tempVariableName, DATA_TYPE *tempVariableType) {
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;
    TOKEN *token = getCachedToken();

    if (token->type == SEMICOLON) {
        return true;
    } else {
        returnCachedTokens(1);

        debugPrintf("calling analyzeExpression from ruleExpSemicolon\n");
        if (analyzeExpression(actualInstructionList, tempVariableName, tempVariableType)) {
            token = getCachedToken();
            if (token->type == SEMICOLON){

                //check return type
                if(actualFunction->type == *tempVariableType){
                    actualFunction->hasReturn = true;
                }

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
        } else {
            functionToCall = getFunctionFromTable(&globalSymbolTable, calledFunctionName);
        }

        tDLList *parameters = malloc(sizeof(tDLList));
        ListInit(parameters);

        if (ruleFuncParams(functionToCall, parameters)) {
            if(!firstPass) {
                semantical_checkFunctionCall(functionToCall->parameters, parameters);

                //you have a list of PARAM *
                if(isFunctionFromIfj16(calledFunctionName)) {
                    //todo: check return types of the funciton - need to add parameter to this function and to the createInstructionsToCallIfj16Function
                    createInstructionsToCallIfj16Function(calledFunctionName, actualInstructionList, parameters, assignReturnValueToVariable);
                } else {
                    ListFirst(parameters);
                    ListFirst(functionToCall->parameters);

                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrFillLocalFrame()));

                    //create local variables for arguments
                    while(DLActive(parameters) ) {
                        FUNCTION_PARAMETER *param = functionToCall->parameters->Act->element.data.parameter;
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createLocalVariable(param->name, param->type)));
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrCopyFromActualToUpcomingFrame(functionToCall->parameters->Act->element.data.parameter->name, parameters->Act->element.data.parameter->name)));

                        //move to the next parameter
                        ListSuccessor(parameters);
                        ListSuccessor(functionToCall->parameters);
                    }

                    //do not create local variable for void function
                    if(functionToCall->type != TYPE_VOID) {
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createActualLocalVariable(stringConcat("#"/*"#"*/, functionToCall->name), functionToCall->type)));
                    }

                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrCallFunction(functionToCall->instructions, functionToCall)));
                    //do not assign to local variable for void function
                    if(functionToCall->type != TYPE_VOID) {
                        ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrAssign(assignReturnValueToVariable, stringConcat("#", functionToCall->name))));
                    }
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
//1,4,14,15
bool ruleParam(struct SYMBOL_TABLE_FUNCTION_STR *functionToCall, tDLList *parameters){
    tDLElemPtr activeElementRuleApplication = globalTokens->Act;

    //<PARAM> -> <EXP> <AFTER_FUNCTION_CALL_EXP>
    char* resultVariableName = NULL;
    DATA_TYPE resultVariableType;
    debugPrintf("calling analyzeExpression from ruleParam\n");
    if (analyzeExpression(actualInstructionList, &resultVariableName, &resultVariableType)) {
        if (ruleAfterFunctionCallExp(functionToCall, parameters)) {
            if(!firstPass) {
                char *newResultVariableName = malloc(strlen(resultVariableName) + 1* sizeof(char));
                strcpy(newResultVariableName, resultVariableName);
                ListInsertLast(parameters, createListElementWithFunctionParamameter(newResultVariableName, resultVariableType));
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
            if (firstPass) {
                ListInsertLast(actualFunction->parameters, *createFunctionParamListElement(type, name));
                createAndInsertVariable(&actualFunction->localSymbolTable, name, type, true);
            }
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
                function = createAndInsertFunction(&globalSymbolTable, stringConcat(classNameWithDot, functionOrPropertyName), TYPE_VOID, 0, NULL, NULL, 0);
            } else {
                function = semantic_getFunction(stringConcat(classNameWithDot, functionOrPropertyName));
            }
            actualFunction = function;
            actualInstructionList = function->instructions;
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
                return true;
            }
        }
    //<DEFINITION_START> -> int <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "int")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            type = TYPE_INT;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
                return true;
            }
        }
    //<DEFINITION_START> -> double <ID><DEFINITION>
    } else if (token->type == KEYWORD && stringEquals(token->data.keyword.name, "double")) {
        if (ruleId(&functionOrPropertyName)) {
            bool propertyInitialized = false;
            bool isFunction = false;
            type = TYPE_DOUBLE;
            if (ruleDefinition(className, type, functionOrPropertyName, &propertyInitialized, &isFunction)) {
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
            debugPrintf("calling analyzeExpression from ruleStatBeginningId\n");
            int code = analyzeExpression(actualInstructionList, &resultVariableName, &resultVariableType);
            //function call
            if (-1 == code) {
                if(ruleId(&functionName)) {
//                  push return variable name to stack
                    STACK_ELEMENT stackElement;
                    stackElement.type =FUNCTION_RETURN_NAME;
                    char *functionReturnName = (char*)malloc(sizeof(char)*(strlen(functionOrPropertyName)+1));
                    strcpy(functionReturnName,functionOrPropertyName);
                    stackElement.data.functionReturnName = functionReturnName;
                    stackPush(returnToVariables,stackElement);

                    if (ruleFuncCall(functionName, functionOrPropertyName)) {
                        if(!firstPass) {
                            initializeVariable(functionOrPropertyName);

                            //add functionOrPropertyName
//                            ListInsertLast(actualFunction->instructions, wrapInstructionIntoListElement(createInstrAssign(functionOrPropertyName, resultVariableName)));
                        }

                        return true;
                    }else{
                        //pop if function Not called
                        stackPop(returnToVariables);
                    }

                }
                //
            } else if (code == 1) {
                //check if the variable "functionOrPropertyName" exists in a symbol table
                if(!firstPass) {
                    semantic_getVariable(functionOrPropertyName);
                    //generate instructions to assign resultVariableName to functionOrPropertyName
                    initializeVariable(functionOrPropertyName);
                    ListInsertLast(actualInstructionList, wrapInstructionIntoListElement(createInstrAssign(functionOrPropertyName, resultVariableName)));
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

//    printAllTokens(globalTokens);
    initializeSymbolTable(globalSymbolTable);
    addIfj16Functions();

    makeFirstPass();
    makeSecondPass();
    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createInstrFillLocalFrame()));
    SYMBOL_TABLE_FUNCTION *fun = semantic_getFunction("Main.run");
    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createInstrCallFunction(fun->instructions, fun)));
    ListInsertLast(mainInstructionList, wrapInstructionIntoListElement(createLastInstruction()));

    printAll();

    exit(Interpret(mainInstructionList, NULL, NULL, NULL, false));


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

    returnToVariables = malloc(sizeof(struct STACK_STR));
    stackInit(returnToVariables);
    bool result = ruleProg();

    if(result == false) {
        exit(2);
    }

    semanticCheckForFunctionRun();
}

void makeSecondPass() {
    debugPrintf("\n\n********************************\n********************************\n\nSTARTING WITH THE SECOND PASS!\n\n********************************\n********************************\n");
    ListFirst(globalTokens);
    firstPass = false;
    actualFunction = NULL;

    bool result = ruleProg();

    if(result == 0) {
        exit(2);
    }
    semantical_ExistAndCorrectTypeOfReturns(&globalSymbolTable);
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
        debugPrintf("exiting with code %d returned from expressionAnalyzer", code);
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

    createAndInsertIntVariable(&globalSymbolTable, "ifj16.*", true);
    createAndInsertFunction(&globalSymbolTable, "ifj16.readInt", TYPE_INT, 0, NULL, NULL, 1);

    createAndInsertFunction(&globalSymbolTable, "ifj16.readDouble", TYPE_DOUBLE, 0, NULL, NULL, 1);

    createAndInsertFunction(&globalSymbolTable, "ifj16.readString", TYPE_STRING, 0, NULL, NULL, 1);

    tDLList *parametersPrint = malloc(sizeof(tDLList));
    ListInit(parametersPrint);
    ListInsertLast(parametersPrint, createListElementWithFunctionParamameter("string", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.print", TYPE_VOID, 0, parametersPrint, NULL, 0);

    tDLList *parametersLength = malloc(sizeof(tDLList));
    ListInit(parametersLength);
    ListInsertLast(parametersLength, createListElementWithFunctionParamameter("s", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.length", TYPE_INT, 0, parametersLength, NULL, 1);

    tDLList *parametersSubstr = malloc(sizeof(tDLList));
    ListInit(parametersSubstr);
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("s", TYPE_STRING));
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("i", TYPE_INT));
    ListInsertLast(parametersSubstr, createListElementWithFunctionParamameter("n", TYPE_INT));
    createAndInsertFunction(&globalSymbolTable, "ifj16.substr", TYPE_STRING, 0, parametersSubstr, NULL, 1);

    tDLList *parametersCompare = malloc(sizeof(tDLList));
    ListInit(parametersCompare);
    ListInsertLast(parametersCompare, createListElementWithFunctionParamameter("s1", TYPE_STRING));
    ListInsertLast(parametersCompare, createListElementWithFunctionParamameter("s2", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.compare", TYPE_INT, 0, parametersCompare, NULL, 1);

    tDLList *parametersFind = malloc(sizeof(tDLList));
    ListInit(parametersFind);
    ListInsertLast(parametersFind, createListElementWithFunctionParamameter("s", TYPE_STRING));
    ListInsertLast(parametersFind, createListElementWithFunctionParamameter("search", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.find", TYPE_INT, 0, parametersFind, NULL, 1);

    tDLList *parametersSort = malloc(sizeof(tDLList));
    ListInit(parametersSort);
    ListInsertLast(parametersSort, createListElementWithFunctionParamameter("s", TYPE_STRING));
    createAndInsertFunction(&globalSymbolTable, "ifj16.sort", TYPE_STRING, 0, parametersSort, NULL, 1);

}

void printAll() {
    debugPrintf("print list of main instructions\n");
    printInstructions(mainInstructionList);

    //test - print instructions for all functions
    struct STACK_STR *symbolTableStacked = BTInorder(globalSymbolTable);
    while (!stackEmpty(symbolTableStacked)) {
        struct STACK_ELEMENT actualElement;
        stackTop(symbolTableStacked, &actualElement);

        //filter all records for classe itself
        if (actualElement.data.symbolTableNode->data->type == TREE_NODE_FUNCTION) {
            SYMBOL_TABLE_FUNCTION *fun = actualElement.data.symbolTableNode->data->item->function;
            debugPrintf("\n\nprinitng instrucitons of function: %s\n", fun->name);
            ListFirst(fun->instructions);
            while(DLActive(fun->instructions)) {
                printInstruction(fun->instructions->Act->element.data.instr);
                ListSuccessor(fun->instructions);
            }
        }

        stackPop(symbolTableStacked);
    }

    debugPrintf("end of printing\n");
}
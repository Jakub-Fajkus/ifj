/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 */

#include "expressionanalyzer.h"
#include "lexicalanalyzerstructures.h"
#include "symboltable.h"
#include "stack.h"
#include "basicstructures.h"
#include "semanticalanalyzer.h"
#include "debug.h"
#include "ifj16.h"

extern struct SYMBOL_TABLE_FUNCTION_STR *actualFunction;
extern char* actualClass;
extern struct SYMBOL_TABLE_NODE *globalSymbolTable;

unsigned long iterator = 0;
int brackets = 0;
void concatenateString();
DATA_TYPE returnType;

char *varName = NULL;
int generate3AddressCode(tDLList *threeAddressCode,tStack *stack, tStack *backStack, bool firstPass);
char *convertShortNameToFullName(char *name);


INSTRUCTION *createInstrMathWithUpdatedNames(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2);
INSTRUCTION *createLocalOrGlobalVariable(char *name, DATA_TYPE type);
INSTRUCTION *pushLocalOrGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);

void setVarName(char *name){ // sets name for return
    if(varName != NULL)
        varName = (char*)realloc(varName,sizeof(char)*(strlen(name)+1));
    else
        varName = (char*)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(varName,name);
}

void setReturnType(DATA_TYPE type){ // sets type for return
    if(type == TYPE_BOOL){
        returnType = TYPE_BOOL;
        return;
    }else if(type == TYPE_VOID){
        exit(4);
    } else if(type == TYPE_STRING || returnType == TYPE_STRING){
        returnType = TYPE_STRING;
        return;
    }

    if(returnType == TYPE_INT){
        returnType = type;
        return;
    }else if(returnType == TYPE_DOUBLE &&  type!=TYPE_INT){
        returnType = type;
        return;
    }else{
        returnType = TYPE_DOUBLE;
        return;
    }
}

bool stopNow = false;

static char terminalTable[14][14] = { // precedent table to 2 dimensional table
           /*+*/ /*-*//***//*(*//*)*//*I*//*/*//*<*//*>*//*>=*//*>=*//*==*//*!=*//*$*/
        /*+*/{'>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>'},
        /*-*/
             {'>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>'},
        /***/
             {'>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>'},
        /*(*/
             {'<', '<', '<', '<', '=', '<', '<', '<', '<', '<', '<', '<', '<', 'X'},
        /*)*/
             {'>', '>', '>', 'X', '>', 'X', '>', '>', '>', '>', '>', '>', '>', '>'},
        /*I*/
             {'>', '>', '>', 'F', '>', 'X', '>', '>', '>', '>', '>', '>', '>', '>'},
        /*/*/
             {'>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>'},
        /*<*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*>*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*<=*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*>=*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*==*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*!=*/
             {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        /*$*/
             {'<', '<', '<', '<', 'X', '<', '<', '<', '<', '<', '<', '<', '<', 'S'}
};

DATA_TYPE getOutputType(DATA_TYPE type1, DATA_TYPE type2) { // get output type by given operands
    if (type1 == TYPE_STRING || type2 == TYPE_STRING) {
        return TYPE_STRING;
    } else if (type1 == TYPE_DOUBLE || type2 == TYPE_DOUBLE) {
        return TYPE_DOUBLE;
    } else {
        return TYPE_INT;
    }
}

EA_TERMINAL_TYPE getTerminalDataType(TOKEN token) { // get key for precedent
    switch (token.type) {
        case KEYWORD:
        case OPERATOR_ASSIGN:
        case END_OF_FILE:
            return EA_UNKNOWN;
        case SEMICOLON:
            return EA_START_END;
        case IDENTIFIER_FULL:
        case IDENTIFIER:
        case LITERAL_STRING:
        case LITERAL_INTEGER:
        case LITERAL_DOUBLE:
            return EA_I;
        case OPERATOR_ARITHMETIC:
            switch (token.data.operatorArithmetic.name) {
                case '+':
                    return EA_ADD;
                case '-':
                    return EA_SUB;
                case '*':
                    return EA_MUL;
                case '/':
                    return EA_DIV;
                default:
                    break;
            }
            break;
        case OPERATOR_LOGIC:
            if (strcmp(token.data.operatorLogic.name, "<") == 0) {
                return EA_IS_LESS;
            } else if (strcmp(token.data.operatorLogic.name, ">") == 0) {
                return EA_IS_MORE;
            } else if (strcmp(token.data.operatorLogic.name, ">=") == 0) {
                return EA_IS_MORE_EQUAL;
            } else if (strcmp(token.data.operatorLogic.name, "<=") == 0) {
                return EA_IS_LESS_EQUAL;
            } else if (strcmp(token.data.operatorLogic.name, "==") == 0) {
                return EA_IS_EQUAL;
            } else if (strcmp(token.data.operatorLogic.name, "!=") == 0) {
                return EA_IS_NOT_EQUAL;
            } else {
                break;
            }
        case BRACKET: {
            switch (token.data.bracket.name) {
                case '(':
                    brackets++;
                    return EA_LEFT_BR;
                case ')':
                    if (brackets > 0) {
                        brackets--;
                        return EA_RIGHT_BR;
                    } else {
                        return EA_START_END;
                    }
                default:
                    break;
            }
        }
        case SEPARATOR:
            return EA_START_END;
        default:
            break;
    }
    return EA_UNKNOWN;
}

int parseExpression(tDLList *threeAddressCode, char **returnValName, DATA_TYPE *returnValType,bool firstPass) { // public function/ interface of expression analyzer
    debugPrintf("\nDEBUG expression START\n");
    returnType = TYPE_INT;
    bool lookingForTerminal = true;
    STACK_ELEMENT stackElement;
    tStack *stack = malloc(sizeof(tStack));
    tStack *backStack = malloc(sizeof(tStack));
    EA_TERMINAL_DATA terminalData;
    stackInit(stack);
    stackInit(backStack);
    // push $
    stackElement.type = EA_TERMINAL;
    stackElement.data.terminalData.type = EA_START_END;

    stackPush(stack, stackElement);

    // return error when first token is part of expression
    TOKEN *token = getCachedToken();
    if ((token->type == BRACKET && token->data.bracket.name == ')') ||
        token->type == KEYWORD || token->type == OPERATOR_ASSIGN ||
        token->type == SEMICOLON || token->type == END_OF_FILE) {
        returnCachedTokens(1);
        return 2;
    }
    returnCachedTokens(1);

    terminalData.type = EA_EMPTY;
    while (true) {
        if (terminalData.type == EA_EMPTY) {
            // new from cache START
            terminalData.token = *getCachedToken(); // BECAUSE reasons
            terminalData.type = getTerminalDataType(terminalData.token);
            // new from cache END
        }

        if (terminalData.type == EA_UNKNOWN)return 2;


        stackTop(stack, &stackElement);
        switch (stackElement.type) {
            case EA_TERMINAL: {
                if (lookingForTerminal) {
                    char action = terminalTable[stackElement.data.terminalData.type][terminalData.type];
                    if (stopNow && action != 'S') { // return error when expression continues after logical expression
                        return 2;
                    }
                    switch (action) {
                        case '<':
                            stackElement.type = EA_TERMINAL_ACTION;
                            stackElement.data.actionType = terminalData.type;
                            stackPush(stack, stackElement);

                            while (!stackEmpty(backStack)) {
                                stackTop(backStack, &stackElement);
                                stackPop(backStack);
                                stackPush(stack, stackElement);
                            }

                            stackElement.type = EA_TERMINAL;

                            stackElement.data.terminalData = terminalData;
                            stackPush(stack, stackElement);
                            terminalData.type = EA_EMPTY;
                            break;
                        case '>':
                            lookingForTerminal = false;
                            break;
                        case '=':
                            while (!stackEmpty(backStack)) {
                                stackTop(backStack, &stackElement);
                                stackPop(backStack);
                                stackPush(stack, stackElement);
                            }
                            stackElement.type = EA_TERMINAL;
                            stackElement.data.terminalData = terminalData;
                            stackPush(stack, stackElement);

                            terminalData.type = EA_EMPTY;
                            break;
                        case 'X':
                            fprintf(stderr, "Invalid expression");
                            return 2;
                        case 'S': // $E$ expression
                            returnCachedTokens(1);
                            debugPrintf("\nDEBUG expression END\n");
                            //reset globals
                            stopNow = false;
                            if(!firstPass) {
                                (*returnValName) = stringConcat(varName, "");
                                (*returnValType) = returnType;
                            }
                            return 0;
                        case 'F': // function call founded
                            if (!stackEmpty(stack)) {
                                stackTop(stack, &stackElement);
                                stackPop(stack);
                                if(stackElement.type!=EA_TERMINAL && stackElement.data.terminalData.type==EA_START_END)return 2;
                            } else return 2;
                            if (!stackEmpty(stack)) {
                                stackTop(stack, &stackElement);
                                stackPop(stack);
                                if(stackElement.type!=EA_TERMINAL_ACTION)return 2;
                            } else return 2;
                            if (!stackEmpty(stack)) {
                                stackTop(stack, &stackElement);
                                stackPop(stack);
                                if(stackElement.type!=EA_TERMINAL && stackElement.data.terminalData.type==EA_I)return 2;
                            } else return 2;
                            if(!stackEmpty(stack))return 2;
                            brackets = 0;
                            returnCachedTokens(2);
                            return -1;
                        default:
                            exit(99);
                    }
                    break;
                }
            } //NO BREAK
            case EA_NOT_TERMINAL:
                stackPop(stack);
                stackPush(backStack, stackElement);
                break;
            case EA_TERMINAL_ACTION:
                if (lookingForTerminal) {
                    exit(99);
                }
                lookingForTerminal = true;
                int genRetVal = generate3AddressCode(threeAddressCode, stack, backStack, firstPass);
                if (genRetVal != 0) return genRetVal;
                break;
            default:
                exit(99);
        }
    }

}

INSTRUCTION_TYPE actionToLogicInstruction(EA_TERMINAL_TYPE actionType) { // translate Expression Analyzer action instruction for interpret
    switch (actionType) {
        case EA_IS_LESS:
            return Instruction_Bool_Less;
        case EA_IS_MORE:
            return Instruction_Bool_More;
        case EA_IS_MORE_EQUAL:
            return Instruction_Bool_MoreEqual;
        case EA_IS_LESS_EQUAL:
            return Instruction_Bool_LessEqual;
        case EA_IS_EQUAL:
            return Instruction_Bool_Equals;
        case EA_IS_NOT_EQUAL:
            return Instruction_Bool_EqualsNot;
        default:
            exit(99);
    }
}
void tokenTypeToVarTypeAndValue(TOKEN token, DATA_TYPE *varType, VARIABLE_VALUE *varVal) {
    switch (token.type) {
        case LITERAL_DOUBLE:
            if(varType != NULL)*varType = TYPE_DOUBLE;
            if(varVal != NULL)varVal->doubleValue = token.data.numberDouble.value;
            break;
        case LITERAL_STRING:
            if(varType != NULL)*varType = TYPE_STRING;
            if(varVal != NULL)varVal->stringValue = token.data.literalString.name;
            break;
        case LITERAL_INTEGER:
            if(varType != NULL)*varType = TYPE_INT;
            if(varVal != NULL)varVal->intValue = token.data.numberInteger.value;
            break;
        default:
            exit(99);
    }
}


int generate3AddressCode(tDLList *threeAddressCode, tStack *stack, tStack *backStack, bool firstPass) {
    STACK_ELEMENT stackElement1;
    STACK_ELEMENT stackElement2;
    STACK_ELEMENT stackElement3;
    stackTop(stack, &stackElement1);
    stackPop(stack);
    EA_TERMINAL_TYPE actionType = stackElement1.data.actionType;
    stackElement2.type = EA_TERMINAL_ACTION; // fake value

    switch (actionType) { // calidate input
        case EA_IS_LESS:
        case EA_IS_MORE:
        case EA_IS_MORE_EQUAL:
        case EA_IS_LESS_EQUAL:
        case EA_IS_EQUAL:
        case EA_IS_NOT_EQUAL:
            stopNow = true;
        case EA_SUB:
        case EA_MUL:
        case EA_DIV:
        case EA_LEFT_BR:
        case EA_ADD:

            if (!stackEmpty(backStack)) {
                stackTop(backStack, &stackElement1);
                stackPop(backStack);
            } else return 2;

            if (!stackEmpty(backStack)) {
                stackTop(backStack, &stackElement2);
                stackPop(backStack);
            } else return 2;

            if (!stackEmpty(backStack)) {
                stackTop(backStack, &stackElement3);
                stackPop(backStack);
            } else return 2;

            if (!stackEmpty(backStack)) {
                return 2;
            }

            break;
        case EA_I:
            if (!stackEmpty(backStack)) {
                stackTop(backStack, &stackElement1);
                stackPop(backStack);
            } else return 2;

            if (!stackEmpty(backStack)) {
                return 2;
            }
            break;
        default:
            exit(99);
    }

    switch (actionType) {
        case EA_ADD:
            if (stackElement1.type == EA_NOT_TERMINAL &&
                stackElement2.type == EA_TERMINAL &&
                stackElement2.data.terminalData.type == EA_ADD &&
                stackElement3.type == EA_NOT_TERMINAL) {
                DATA_TYPE outputType = getOutputType(stackElement1.data.notTerminalData.type,
                                                     stackElement3.data.notTerminalData.type);
                setReturnType(outputType);
                if (!firstPass) {
                    concatenateString();
                    char *tempName = (char *) malloc(sizeof(char) * 30);
                    strcpy(tempName, varName);
                    INSTRUCTION *instruction1 = createLocalOrGlobalVariable(tempName, outputType);
                    ListInsertLast(threeAddressCode,createInstruction(instruction1));

                    INSTRUCTION *instruction2 = createInstrMathWithUpdatedNames(Instruction_Addition, tempName,
                                                   stackElement1.data.notTerminalData.name,
                                                   stackElement3.data.notTerminalData.name);
                    ListInsertLast(threeAddressCode,createInstruction(instruction2));
                    stackElement1.data.notTerminalData.name = tempName;
                } else {
                    stackElement1.data.notTerminalData.name = "tempName";
                }
                debugPrintf("generate: E->E+E\n");
                stackElement1.data.notTerminalData.type = outputType;
                stackElement1.type = EA_NOT_TERMINAL;
            } else return 2;
            break;
        case EA_SUB:
            if (stackElement1.type == EA_NOT_TERMINAL &&
                stackElement2.type == EA_TERMINAL &&
                stackElement2.data.terminalData.type == EA_SUB &&
                stackElement3.type == EA_NOT_TERMINAL) {
                DATA_TYPE outputType = getOutputType(stackElement1.data.notTerminalData.type,
                                                     stackElement3.data.notTerminalData.type);
                setReturnType(outputType);
                if (!firstPass) {

                    concatenateString();

                    if (outputType == TYPE_STRING) return 4;

                    char *tempName = (char *) malloc(sizeof(char) * 30);
                    strcpy(tempName, varName);
                    INSTRUCTION *instruction1 = createLocalOrGlobalVariable(tempName, outputType);
                    ListInsertLast(threeAddressCode,createInstruction(instruction1));

                    INSTRUCTION *instruction2 = createInstrMathWithUpdatedNames(Instruction_Subtraction, tempName,
                                                   stackElement1.data.notTerminalData.name,
                                                   stackElement3.data.notTerminalData.name);
                    ListInsertLast(threeAddressCode,createInstruction(instruction2));
                    stackElement1.data.notTerminalData.name = tempName;
                } else {
                    stackElement1.data.notTerminalData.name = "tempName";
                }
                debugPrintf("generate: E->E-E\n");
                stackElement1.data.notTerminalData.type = outputType;
                stackElement1.type = EA_NOT_TERMINAL;
            } else return 2;
            break;
        case EA_MUL:
            if (stackElement1.type == EA_NOT_TERMINAL &&
                stackElement2.type == EA_TERMINAL &&
                stackElement2.data.terminalData.type == EA_MUL &&
                stackElement3.type == EA_NOT_TERMINAL) {
                DATA_TYPE outputType = getOutputType(stackElement1.data.notTerminalData.type,
                                                     stackElement3.data.notTerminalData.type);
                setReturnType(outputType);
                if (!firstPass) {
                    concatenateString();

                    if (outputType == TYPE_STRING) return 4;

                    char *tempName = (char *) malloc(sizeof(char) * 30);
                    strcpy(tempName, varName);
                    INSTRUCTION *instruction1 = createLocalOrGlobalVariable(tempName, outputType);
                    ListInsertLast(threeAddressCode,createInstruction(instruction1));

                    INSTRUCTION *instruction2 = createInstrMathWithUpdatedNames(Instruction_Multiply, tempName,
                                                   stackElement1.data.notTerminalData.name,
                                                   stackElement3.data.notTerminalData.name);
                    ListInsertLast(threeAddressCode,createInstruction(instruction2));

                    stackElement1.data.notTerminalData.name = tempName;
                } else {
                    stackElement1.data.notTerminalData.name = "tempName";
                }
                debugPrintf("generate: E->E*E\n");

                stackElement1.data.notTerminalData.type = outputType;
                stackElement1.type = EA_NOT_TERMINAL;
            } else return 2;

            break;
        case EA_DIV:
            if (stackElement1.type == EA_NOT_TERMINAL &&
                stackElement2.type == EA_TERMINAL &&
                stackElement2.data.terminalData.type == EA_DIV &&
                stackElement3.type == EA_NOT_TERMINAL) {

                DATA_TYPE outputType = getOutputType(stackElement1.data.notTerminalData.type,
                                                     stackElement3.data.notTerminalData.type);
                setReturnType(outputType);
                if (!firstPass) {
                    concatenateString();

                    if (outputType == TYPE_STRING) return 4;

                    char *tempName = (char *) malloc(sizeof(char) * 30);
                    strcpy(tempName, varName);

                    INSTRUCTION *instruction1 = createLocalOrGlobalVariable(tempName, outputType);
                    ListInsertLast(threeAddressCode,createInstruction(instruction1));

                    INSTRUCTION *instruction2 = createInstrMathWithUpdatedNames(Instruction_Divide, tempName,
                                    stackElement1.data.notTerminalData.name,
                                    stackElement3.data.notTerminalData.name);
                    ListInsertLast(threeAddressCode,createInstruction(instruction2));

                    stackElement1.data.notTerminalData.name = tempName;
                } else {
                    stackElement1.data.notTerminalData.name = "tempName";
                }
                debugPrintf("generate: E->E/E\n");
                stackElement1.data.notTerminalData.type = outputType;
                stackElement1.type = EA_NOT_TERMINAL;
            } else return 2;
            break;
        case EA_LEFT_BR:
            if (stackElement1.type == EA_TERMINAL &&
                stackElement1.data.terminalData.type == EA_LEFT_BR &&
                stackElement2.type == EA_NOT_TERMINAL &&
                stackElement3.data.terminalData.type == EA_RIGHT_BR &&
                stackElement3.type == EA_TERMINAL) {
                debugPrintf("generate: E->(E)\n");
                stackElement1 = stackElement2;
            } else return 2;

            break;
        case EA_I:
            if (stackElement1.type == EA_TERMINAL &&
                stackElement1.data.terminalData.type == EA_I) {

                if (stackElement1.data.terminalData.token.type == IDENTIFIER) {
                    stackElement2.data.notTerminalData.name = stackElement1.data.terminalData.token.data.identifier.name;
                    setVarName(stackElement2.data.notTerminalData.name);
                    if (!firstPass) {
                        SYMBOL_TABLE_VARIABLE *symbolTableVariable= semantic_getInitializedVariable(stackElement2.data.notTerminalData.name);
                        stackElement2.data.notTerminalData.type = symbolTableVariable->type;
                    } else {
                        semantic_firstPass_testStaticVariable(stackElement2.data.notTerminalData.name);
                        stackElement2.data.notTerminalData.type = TYPE_INT;
                    }

                    debugPrintf("generate: E->i where i = ID\n");

                    setReturnType(stackElement2.data.notTerminalData.type);
                } else if (stackElement1.data.terminalData.token.type == IDENTIFIER_FULL) {
                    char *tempName = (char *) malloc(
                            sizeof(char) * (1
                                            +
                                            strlen(stackElement1.data.terminalData.token.data.identifierFull.class)
                                            +
                                            strlen(stackElement1.data.terminalData.token.data.identifierFull.class)
                            )
                    );
                    sprintf(
                            tempName,
                            "%s.%s",
                            stackElement1.data.terminalData.token.data.identifierFull.class,
                            stackElement1.data.terminalData.token.data.identifierFull.name
                    );
                    stackElement2.data.notTerminalData.name = tempName;
                    if (!firstPass) {
                        setVarName(tempName);
                        SYMBOL_TABLE_VARIABLE *symbolTableVariable= semantic_getInitializedVariable(tempName);
                        stackElement2.data.notTerminalData.type = symbolTableVariable->type;
                    } else {
                        semantic_firstPass_testStaticVariable(stackElement2.data.notTerminalData.name);
                        stackElement2.data.notTerminalData.type = TYPE_INT;
                        stackElement2.data.notTerminalData.name = "tepName";
                    }
                    setReturnType(stackElement2.data.notTerminalData.type );
                    debugPrintf("generate: E->i where i = ID_FULL\n");
                } else {
                    if (!firstPass) {
                        concatenateString();
                        char *tempName = (char *) malloc(sizeof(char) * (actualClass != NULL)? strlen(actualClass)+30 : 30);
                        strcpy(tempName, varName);
                        VARIABLE_VALUE *varVal = (VARIABLE_VALUE*)malloc(sizeof(VARIABLE_VALUE));
                        DATA_TYPE *varType = (DATA_TYPE*)malloc(sizeof(DATA_TYPE));
                        tokenTypeToVarTypeAndValue(stackElement1.data.terminalData.token, varType,varVal);
                        INSTRUCTION *instruction1 = pushLocalOrGlobalVariable(tempName,*varType,*varVal);
                        ListInsertLast(threeAddressCode,createInstruction(instruction1));

                        stackElement2.data.notTerminalData.type = *varType;
                        free(varVal);
                        free(varType);
                        stackElement2.data.notTerminalData.name = tempName;
                    } else {
                        stackElement2.data.notTerminalData.type = TYPE_INT;
                        stackElement2.data.notTerminalData.name = "tepName";
                    }
                    setReturnType(stackElement2.data.notTerminalData.type );
                    debugPrintf("generate: E->i where i = LIT\n");
                }

                stackElement2.type = EA_NOT_TERMINAL;
                stackElement1 = stackElement2;
            } else return 2;
            break;
        case EA_IS_LESS:
        case EA_IS_MORE:
        case EA_IS_MORE_EQUAL:
        case EA_IS_LESS_EQUAL:
        case EA_IS_EQUAL:
        case EA_IS_NOT_EQUAL:
            if (stackElement1.type == EA_NOT_TERMINAL &&
                stackElement2.type == EA_TERMINAL &&
                stackElement3.type == EA_NOT_TERMINAL) {

                if(stackElement1.data.notTerminalData.type == TYPE_STRING || stackElement3.data.notTerminalData.type == TYPE_STRING){
                    return 4;
                }
                if (!firstPass) {
                    concatenateString();
                    char *tempName = (char *) malloc(sizeof(char) * 30);
                    strcpy(tempName, varName);
                    INSTRUCTION *instruction1 = createLocalOrGlobalVariable(tempName, TYPE_INT);
                    ListInsertLast(threeAddressCode,createInstruction(instruction1));
                    INSTRUCTION *instruction2 =  createInstrExprEval(actionToLogicInstruction(actionType), convertShortNameToFullName(tempName),
                                                     convertShortNameToFullName(stackElement1.data.notTerminalData.name),
                                                     convertShortNameToFullName(stackElement3.data.notTerminalData.name));
                    ListInsertLast(threeAddressCode,createInstruction(instruction2));
                    stackElement1.data.notTerminalData.name = tempName;
                } else {
                    stackElement1.data.notTerminalData.name = "tempName";
                }
                setReturnType(TYPE_BOOL);
                stackElement1.data.notTerminalData.type = TYPE_BOOL;
                stackElement1.type = EA_NOT_TERMINAL;
                debugPrintf("generate: E->E_LOGIC_E\n");
            } else return 2;
            break;
        default:
            exit(99);
    }

    stackPush(stack, stackElement1);
    return 0;
};

INSTRUCTION *createLocalOrGlobalVariable(char *name, DATA_TYPE type) {
    if(actualFunction == NULL) {
        debugPrintf("createLocalOrGlobalVariable %s - global\n", name);
        return createGlobalVariable(name, type);
    } else {
        debugPrintf("createLocalOrGlobalVariable %s - local to function %s\n", name, actualFunction->name);
        return createActualLocalVariable(name, type);
    }
}

INSTRUCTION *pushLocalOrGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value) {
    if(actualFunction == NULL) {
        debugPrintf("pushLocalOrGlobalVariable %s - global\n", name);
        return pushGlobalVariable(name, type, value);
    } else {
        debugPrintf("pushLocalOrGlobalVariable %s - local to function %s\n", name, actualFunction->name);
        return pushActualLocalVariable(name, type, value);
    }
}


void concatenateString() { // "generate" variable for return
    if(varName != NULL)
        varName = (char*)realloc(varName,sizeof(char)*100);
    else
        varName = (char*)malloc(sizeof(char)*100);
    iterator++;

    if(actualFunction == NULL) {
        sprintf(varName, "#%s.ExpressionAnalyzerVar%lu", actualClass, iterator);
    } else {
        sprintf(varName, "#ExpressionAnalyzerVar%lu", iterator);
    }
}

char *convertShortNameToFullName(char *name) {
    if (ifj16_find(name, "#") == -1 ) {
        //it is not temp variable
        char *result = getVariable((actualFunction == NULL)? NULL : &actualFunction->localSymbolTable, &globalSymbolTable, actualClass, name)->name;

        return result;
    } else {
        //nothing to do...
        return name;
    }
}

INSTRUCTION *createInstrMathWithUpdatedNames(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2) {

    return createInstrMath(instType, convertShortNameToFullName(nameDst), convertShortNameToFullName(nameSrc1), convertShortNameToFullName(nameSrc2));
}

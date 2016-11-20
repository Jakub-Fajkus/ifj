//
// Created by rockuo on 19.11.16.
//

#include "ExpressionAnalizer.h"
#include "LexicalAnalyzerStructures.h"
#include "Stack.h"

void Error() {
    exit(99);
}

//typedef enum { // pro jistotu
//    OPEN = ,
//    CLOSE = ,
//    EQUAL = 2,
//    ERROR = 3,
//    SUCCESS = 4
//} OPERATION_TYPE;


static char terminalTable[14][14] = {
        {'>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>'},
        {'>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>'},
        {'>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>'},
        {'<', '>', '<', '<', '=', '<', '>', '>', '>', '>', '>', '>', '>', 'X'},
        {'>', '>', '>', 'X', '>', 'X', '>', '>', '>', '>', '>', '>', '>', '>'},
        {'>', '>', '>', 'X', '>', 'X', '>', '>', '>', '>', '>', '>', '>', '>'},
        {'>', '>', '>', '<', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', '>', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>'},
        {'<', '<', '<', '<', 'X', '<', '<', '<', '<', '<', '<', '<', '<', 'S'}
};

EA_TERMINAL_TYPE getTerminalDataType(TOKEN *token) {
    switch (token->type) {
        case KEYWORD:
        case OPERATOR_ASSIGN:
        case SEMICOLON:
        case END_OF_FILE:
            return EA_UNKNOWN;
        case IDENTIFIER_FULL:
        case IDENTIFIER:
        case LITERAL_STRING:
        case LITERAL_INTEGER:
        case LITERAL_DOUBLE:
            return EA_I;
        case OPERATOR_ARITHMETIC:
            switch (token->data.operatorArithmetic.name) {
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
        case OPERATOR_LOGIC:
            if (strcmp(token->data.operatorLogic.name, "<")) {
                return EA_IS_LESS;
            } else if (strcmp(token->data.operatorLogic.name, ">")) {
                return EA_IS_MORE;
            } else if (strcmp(token->data.operatorLogic.name, ">=")) {
                return EA_IS_MORE_EQUAL;
            } else if (strcmp(token->data.operatorLogic.name, "<=")) {
                return EA_IS_LESS_EQUAL;
            } else if (strcmp(token->data.operatorLogic.name, "==")) {
                return EA_IS_EQUAL;
            }else if (strcmp(token->data.operatorLogic.name, "!=")) {
                return EA_IS_NOT_EQUAL;
            } else {
                break;
            }
        case BRACKET: {
            switch (token->data.bracket.name) {
                case '(':
                    return EA_LEFT_BR;
                case ')':
                    return EA_RIGHT_BR;
                default:
                    break;
            }
        }
        default:
            break;
    }
    Error();
    exit(99); //for compiler ...should never happened
}

void parseExpression(tDLList *tokenList, tDLList *threeAddressCode) {

    bool lookingForTerminal = true;
    STACK_ELEMENT stackElement;
    tStack *stack = NULL;
    tStack *backStack = NULL;
    EA_TERMINAL_DATA terminalData;
    stackInit(stack);
    stackInit(backStack);
    // push $
    stackElement.type = EA_TERMINAL;
    stackElement.data.terminalData.type = EA_START_END;
    stackPush(stack, stackElement);

    terminalData.type = EA_EMPTY;
    while (true) {
        if(terminalData.type == EA_EMPTY) {
            // new from cache START
            terminalData.token = *getCachedToken(tokenList); // BECAUSE reasons
            terminalData.type = getTerminalDataType(&terminalData.token);
            // new from cache END

        }

        stackTop(stack, &stackElement);
        switch (stackElement.type){
            case EA_TERMINAL: {
                if(lookingForTerminal) {
                    char action = terminalTable[stackElement.data.terminalData.type][terminalData.type];
                    switch (action) {
                        case '<':
                            stackElement.type = EA_TERMINAL_ACTION;
                            stackPush(stack,stackElement);

                            while(stackEmpty(stack)){
                                stackTop(backStack, &stackElement);
                                stackPop(backStack);
                                stackPush(stack, stackElement);
                            }

                            stackElement.type = EA_TERMINAL;
                            stackElement.data.terminalData = terminalData;
                            stackPush(stack,stackElement);
                            terminalData.type = EA_EMPTY;
                            break;
                        case '>':
                            lookingForTerminal = false;
                            break;
                        case '=':
                            stackElement.type = EA_TERMINAL;
                            stackElement.data.terminalData = terminalData;
                            terminalData.type = EA_EMPTY;

                            stackPush(stack, stackElement);
                            break;
                        default:
                            Error();
                            exit(99);
                    }
                    break;
                }
            } //NO BREAL
            case EA_NOT_TERMINAL:
                stackPop(stack);
                stackPush(backStack, stackElement);
                break;
            case EA_TERMINAL_ACTION:
                if(lookingForTerminal){
                    Error();
                }
                lookingForTerminal = true;
                //TODO Generovat 3 adresny kod
            default:
                while(true){
                    stackTop(stack,&stackElement);
                    if(stackElement.type == EA_TERMINAL){
                        if(stackElement.data.terminalData.type == EA_START_END) {
                            return;
                        } else{
                            Error();
                        }
                    }
                }
        }
    }

}


//
// Created by rockuo on 19.11.16.
//

#include "ExpressionAnalizer.h"
#include "Stack.h"
#include "LexicalAnalyzerStructures.h"

void generate3AdressCode();

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
    static int brackets = 0;
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
                    brackets++;
                    return EA_LEFT_BR;
                case ')':
                    if(brackets > 0){
                        brackets--;
                        return EA_RIGHT_BR;
                    }else{
                        return EA_UNKNOWN;
                    }
                default:
                    break;
            }
        }
        default:
            break;
    }
    exit(99);
}

bool parseExpression(tDLList *threeAddressCode, char *returnVal) {

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

    //todo: validate
    TOKEN *token = getCachedToken();
    if((token->type == BRACKET && token->data.bracket.name == ')') ||
       !(token->type == OPERATOR_ARITHMETIC || token->type == OPERATOR_LOGIC ||
         token->type == IDENTIFIER || token->type == LITERAL_DOUBLE || token->type == LITERAL_INTEGER || token->type == LITERAL_STRING)
            ) {
        returnCachedTokens(1);
        return false;
    }
    returnCachedTokens(1);

    terminalData.type = EA_EMPTY;
    while (true) {
        if(terminalData.type == EA_EMPTY) {
            // new from cache START
            terminalData.token = *getCachedToken(); // BECAUSE reasons
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
                            returnCachedTokens(1);
                            while(true){
                                stackTop(stack,&stackElement);
                                if(stackElement.type == EA_TERMINAL){
                                    if(stackElement.data.terminalData.type == EA_START_END) {
                                        return true;
                                    } else{
                                        return  false;
                                    }
                                }
                            }
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
                    exit(99);
                }
                lookingForTerminal = true;
                stackPop(stack);

                generate3AdressCode();

                stackElement.type = EA_NOT_TERMINAL;
                stackPush(stack,stackElement);
                break;
            default:
                exit(99);
        }
    }

}

void generate3AdressCode(){

};
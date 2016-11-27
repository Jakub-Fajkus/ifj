//
// Created by Jakub Fajkus on 17.11.16.
//
#include "Debug.h"
#include "Stack.h"

void printAllTokens(tDLList *list) {
    LIST_ELEMENT *actualElement = malloc(sizeof(LIST_ELEMENT));
    ListFirst(list);

    while(DLActive(list) != 0) {
        ListElementCopy(list, actualElement);
        ListSuccessor(list);
        printToken(actualElement->data.token);
    }

    return;
}

void printEnumValue(int value) {
    switch (value) {
        case KEYWORD: debugPrintf("KEYWORD\n"); break;
        case IDENTIFIER_FULL: debugPrintf("IDENTIFIER_FULL\n"); break;
        case IDENTIFIER: debugPrintf("IDENTIFIER\n"); break;
        case LITERAL_STRING: debugPrintf("LITERAL_STRING\n"); break;
        case LITERAL_INTEGER: debugPrintf("LITERAL_INTEGER\n"); break;
        case LITERAL_DOUBLE: debugPrintf("LITERAL_DOUBLE\n"); break;
        case OPERATOR_ARITHMETIC: debugPrintf("OPERATOR_ARITHMETIC\n"); break;
        case OPERATOR_LOGIC: debugPrintf("OPERATOR_LOGIC\n"); break;
        case OPERATOR_ASSIGN: debugPrintf("OPERATOR_ASSIGN\n"); break;
        case SEPARATOR: debugPrintf("SEPARATOR\n"); break;
        case BRACKET: debugPrintf("BRACKET\n"); break;
        case SEMICOLON: debugPrintf("SEMICOLON\n"); break;
        case END_OF_FILE: debugPrintf("END_OF_FILE\n"); break;
        case LEX_ERROR: debugPrintf("LEX_ERROR\n"); break;
    }
}

void printToken(TOKEN *token) {
    switch (token->type) {
        case KEYWORD: debugPrintf("KEYWORD: %s\n", token->data.keyword.name); break;
        case IDENTIFIER_FULL: debugPrintf("IDENTIFIER_FULL: %s.%s\n", token->data.identifierFull.class, token->data.identifierFull.name); break;
        case IDENTIFIER: debugPrintf("IDENTIFIER: %s\n", token->data.identifier.name); break;
        case LITERAL_STRING: debugPrintf("LITERAL_STRING: %s\n",token->data.literalString.name ); break;
        case LITERAL_INTEGER: debugPrintf("LITERAL_INTEGER: %d\n", token->data.numberInteger.value); break;
        case LITERAL_DOUBLE: debugPrintf("LITERAL_DOUBLE: %lf\n", token->data.numberDouble.value); break;
        case OPERATOR_ARITHMETIC: debugPrintf("OPERATOR_ARITHMETIC: %c\n", token->data.operatorArithmetic.name); break;
        case OPERATOR_LOGIC: debugPrintf("OPERATOR_LOGIC: %s\n", token->data.operatorLogic.name); break;
        case OPERATOR_ASSIGN: debugPrintf("OPERATOR_ASSIGN:\n"); break;
        case SEPARATOR: debugPrintf("SEPARATOR: %c\n", ','); break;
        case BRACKET: debugPrintf("BRACKET: %c\n", token->data.bracket.name); break;
        case SEMICOLON: debugPrintf("SEMICOLON: %c\n", ';'); break;
        case END_OF_FILE: debugPrintf("END_OF_FILE:\n"); break;
        case LEX_ERROR: debugPrintf("LEX_ERROR: \n"); break;
    }
}

void printStack(tStack stack) {
    STACK_ELEMENT *actual = malloc(sizeof(STACK_ELEMENT));
    STACK_ELEMENT *last;

    stackTop(&stack, actual);

    debugPrintf("printing the stack from the top to the bottom:\n");

    //it is updated in the function stackTop
    while(!stackEmpty(&stack)) {
        debugPrintf("....");
        if (actual->type == STACK_ELEMENT_TYPE_EXPRESSION) {
            debugPrintf("type: STACK_ELEMENT_TYPE_EXPRESSION \n");
        } else if (actual->type == EA_TERMINAL) {
            debugPrintf("type: EA_TERMINAL \n");
            printToken(&actual->data.terminalData.token);
        } else if (actual->type == EA_NOT_TERMINAL) {
            debugPrintf("type: EA_NOT_TERMINAL \n");
        } else if (actual->type == EA_TERMINAL_ACTION) {
            debugPrintf("type: EA_TERMINAL_ACTION \n");
        } else {
            debugPrintf("unknown type \n");
        }

        printEaTerminalType(actual->data.terminalData.type);



        stackPop(&stack);
        last = actual;
        stackTop(&stack, actual);

    }

    debugPrintf("printing the stack is OVER:\n\n\n\n");
}


void printEaTerminalType(enum EA_TERMINAL_TYPE_ENUM type) {
    switch (type) {
        case EA_ADD: debugPrintf("terminal type: EA_ADD\n"); break;
        case EA_SUB: debugPrintf("terminal type: EA_SUB\n"); break;
        case EA_MUL: debugPrintf("terminal type: EA_MUL\n"); break;
        case EA_LEFT_BR: debugPrintf("terminal type: EA_LEFT_BR\n"); break;
        case EA_RIGHT_BR: debugPrintf("terminal type: EA_RIGHT_BR\n"); break;
        case EA_I: debugPrintf("terminal type: EA_I\n"); break;
        case EA_DIV: debugPrintf("terminal type: EA_DIV\n"); break;
        case EA_IS_LESS: debugPrintf("terminal type: EA_IS_LESS\n"); break;
        case EA_IS_MORE: debugPrintf("terminal type: EA_IS_MORE\n"); break;
        case EA_IS_LESS_EQUAL: debugPrintf("terminal type: EA_IS_LESS_EQUAL\n"); break;
        case EA_IS_MORE_EQUAL: debugPrintf("terminal type: EA_IS_MORE_EQUAL\n"); break;
        case EA_IS_EQUAL: debugPrintf("terminal type: EA_IS_EQUAL\n"); break;
        case EA_IS_NOT_EQUAL: debugPrintf("terminal type: EA_IS_NOT_EQUAL\n"); break;
        case EA_START_END: debugPrintf("terminal type: EA_START_END\n"); break;
        case EA_UNKNOWN: debugPrintf("terminal type: EA_UNKNOWN\n"); break;
        case EA_EMPTY: debugPrintf("terminal type: EA_EMPTY\n"); break;
    }
}
//
// Created by Jakub Fajkus on 17.11.16.
//
#include "Debug.h"
#include "Stack.h"

void printAllTokens(tDLList *list) {
    LIST_ELEMENT *actualElement = malloc(sizeof(LIST_ELEMENT));
    DLFirst(list);

    while(DLActive(list) != 0) {
        DLCopy(list, actualElement);
        DLSucc(list);
        printToken(actualElement->data.token);
    }

    return;
}

void printEnumValue(int value) {
    switch (value) {
        case KEYWORD: printf("KEYWORD\n"); break;
        case IDENTIFIER_FULL: printf("IDENTIFIER_FULL\n"); break;
        case IDENTIFIER: printf("IDENTIFIER\n"); break;
        case LITERAL_STRING: printf("LITERAL_STRING\n"); break;
        case LITERAL_INTEGER: printf("LITERAL_INTEGER\n"); break;
        case LITERAL_DOUBLE: printf("LITERAL_DOUBLE\n"); break;
        case OPERATOR_ARITHMETIC: printf("OPERATOR_ARITHMETIC\n"); break;
        case OPERATOR_LOGIC: printf("OPERATOR_LOGIC\n"); break;
        case OPERATOR_ASSIGN: printf("OPERATOR_ASSIGN\n"); break;
        case SEPARATOR: printf("SEPARATOR\n"); break;
        case BRACKET: printf("BRACKET\n"); break;
        case SEMICOLON: printf("SEMICOLON\n"); break;
        case END_OF_FILE: printf("END_OF_FILE\n"); break;
        case LEX_ERROR: printf("LEX_ERROR\n"); break;
    }
}

void printToken(TOKEN *token) {
    switch (token->type) {
        case KEYWORD: printf("KEYWORD: %s\n", token->data.keyword.name); break;
        case IDENTIFIER_FULL: printf("IDENTIFIER_FULL: %s.%s\n", token->data.identifierFull.class, token->data.identifierFull.name); break;
        case IDENTIFIER: printf("IDENTIFIER: %s\n", token->data.identifier.name); break;
        case LITERAL_STRING: printf("LITERAL_STRING: %s\n",token->data.literalString.name ); break;
        case LITERAL_INTEGER: printf("LITERAL_INTEGER: %d\n", token->data.numberInteger.value); break;
        case LITERAL_DOUBLE: printf("LITERAL_DOUBLE: %lf\n", token->data.numberDouble.value); break;
        case OPERATOR_ARITHMETIC: printf("OPERATOR_ARITHMETIC: %c\n", token->data.operatorArithmetic.name); break;
        case OPERATOR_LOGIC: printf("OPERATOR_LOGIC: %s\n", token->data.operatorLogic.name); break;
        case OPERATOR_ASSIGN: printf("OPERATOR_ASSIGN:\n"); break;
        case SEPARATOR: printf("SEPARATOR: %c\n", ','); break;
        case BRACKET: printf("BRACKET: %c\n", token->data.bracket.name); break;
        case SEMICOLON: printf("SEMICOLON: %c\n", ';'); break;
        case END_OF_FILE: printf("END_OF_FILE:\n"); break;
        case LEX_ERROR: printf("LEX_ERROR: \n"); break;
    }
}

void printStack(tStack stack) {
    STACK_ELEMENT *actual = malloc(sizeof(STACK_ELEMENT));
    STACK_ELEMENT *last;

    stackTop(&stack, actual);

    printf("printing the stack from the top to the bottom:\n");

    //it is updated in the function stackTop
    while(actual != NULL) {
        if (actual->type == STACK_ELEMENT_TYPE_EXPRESSION) {
            printf("type: STACK_ELEMENT_TYPE_EXPRESSION \n");
        } else if (actual->type == EA_TERMINAL) {
            printf("type: EA_TERMINAL \n");
            printToken(&actual->data.terminalData.token);
        } else if (actual->type == EA_NOT_TERMINAL) {
            printf("type: EA_NOT_TERMINAL \n");
        } else if (actual->type == EA_TERMINAL_ACTION) {
            printf("type: EA_TERMINAL_ACTION \n");
        } else {
            printf("unknown type \n");
        }

        printEaTerminalType(actual->data.terminalData.type);

        printToken(&actual->data.terminalData.token);


        stackPop(&stack);
        last = actual;
        stackTop(&stack, actual);

        if(last == actual) {
            break;
        }
    }

    printf("printing the stack is OVER:\n\n\n\n");
}


void printEaTerminalType(enum EA_TERMINAL_TYPE_ENUM type) {
    switch (type) {
        case EA_ADD: printf("terminal type: EA_ADD\n"); break;
        case EA_SUB: printf("terminal type: EA_SUB\n"); break;
        case EA_MUL: printf("terminal type: EA_MUL\n"); break;
        case EA_LEFT_BR: printf("terminal type: EA_LEFT_BR\n"); break;
        case EA_RIGHT_BR: printf("terminal type: EA_RIGHT_BR\n"); break;
        case EA_I: printf("terminal type: EA_I\n"); break;
        case EA_DIV: printf("terminal type: EA_DIV\n"); break;
        case EA_IS_LESS: printf("terminal type: EA_IS_LESS\n"); break;
        case EA_IS_MORE: printf("terminal type: EA_IS_MORE\n"); break;
        case EA_IS_LESS_EQUAL: printf("terminal type: EA_IS_LESS_EQUAL\n"); break;
        case EA_IS_MORE_EQUAL: printf("terminal type: EA_IS_MORE_EQUAL\n"); break;
        case EA_IS_EQUAL: printf("terminal type: EA_IS_EQUAL\n"); break;
        case EA_IS_NOT_EQUAL: printf("terminal type: EA_IS_NOT_EQUAL\n"); break;
        case EA_START_END: printf("terminal type: EA_START_END\n"); break;
        case EA_UNKNOWN: printf("terminal type: EA_UNKNOWN\n"); break;
        case EA_EMPTY: printf("terminal type: EA_EMPTY\n"); break;
    }
}
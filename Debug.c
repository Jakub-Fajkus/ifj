//
// Created by Jakub Fajkus on 17.11.16.
//
#include "Debug.h"

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
        case LITERAL_BOOL: printf("LITERAL_BOOL\n"); break;
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
        case LITERAL_BOOL: printf("LITERAL_BOOL: %s\n", (token->data.literalBool.value)? "true":"false"); break;
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
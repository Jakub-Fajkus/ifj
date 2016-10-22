#include "LexicalAnalyzer.h"
#include <assert.h>

#define keyword(keywordName){ \
    TOKEN *token = getToken();\
    assert(token->type == KEYWORD);\
    assert(strcmp(token->data.keyword.name, keywordName) == 0);}

#define identifier(identifierName) { \
    TOKEN *token = getToken(); \
    assert(token->type == IDENTIFIER); \
    assert(strcmp(token->data.identifier.name, identifierName) == 0);}

#define identifierFull(className, identifierName) { \
    TOKEN *token = getToken(); \
    assert(token->type == IDENTIFIER_FULL); \
    assert(strcmp(token->data.identifierFull.class, className) == 0); \
    assert(strcmp(token->data.identifierFull.name, identifierName) == 0);}

#define bracket(bracketChar) { \
    TOKEN *token = getToken(); \
    assert(token->type == BRACKET); \
    assert(token->data.bracket.name == bracketChar);}

#define semicolon() { \
    TOKEN *token = getToken(); \
    assert(token->type == SEMICOLON);}

#define assign() { \
    TOKEN *token = getToken(); \
    assert(token->type == OPERATOR_ASSIGN);}

#define stringLiteral(stringValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == LITERAL_STRING); \
    assert(strcmp(token->data.literalString.name, stringValue) == 0);}

#define integerLiteral(integerValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == LITERAL_INTEGER); \
    assert(token->data.numberInteger.value == integerValue);}

#define doubleLiteral(doubleValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == LITERAL_DOUBLE); \
    assert(token->data.numberDouble.value == doubleValue);}

#define operatorLogic(stringValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == OPERATOR_LOGIC); \
    assert(strcmp(token->data.operatorLogic.name, stringValue) == 0);}

#define operatorArithmetic(stringValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == OPERATOR_ARITHMETIC); \
    assert(token->data.operatorArithmetic.name == stringValue);}

#define ondOfFile(stringValue) { \
    TOKEN *token = getToken(); \
    assert(token->type == END_OF_FILE); }

void test1();
void printEnumValue(int value);
void printToken(TOKEN *token);

int main(int argc, char *argv[]) {

//    destroyStream();

    test1();
    return 0;
}

void test1(){
    initializeStream("test1.txt");
    TOKEN *tok;
    //class Main
    keyword("class")identifier("Main")
    bracket('{')
        //static void run()
        keyword("static")keyword("void")identifier("run")bracket('(')bracket(')')
        bracket('{')
            //int a;
            keyword("int")identifier("a")semicolon()
            //ifj16.print("Zadejte cislo pro vypocet faktorialu: ");
            identifierFull("ifj16", "print")bracket('(')stringLiteral("Zadejte cislo pro vypocet faktorialu: ")bracket(')')semicolon()
            //a = ifj16.readInt();
            identifier("a")assign()identifierFull("ifj16", "readInt")bracket('(')bracket(')')semicolon()
            //int vysl;
            keyword("int")identifier("vysl")semicolon()
            //if (a < 0) { // nacitani zaporneho cisla nemusite podporovat
            keyword("if")bracket('(')identifier("a")operatorLogic("<")integerLiteral(0)bracket(')')bracket('{')
                //ifj16.print("Faktorial nelze spocitat!\n");
                identifierFull("ifj16", "print")bracket('(')stringLiteral("Faktorial nelze spocitat!\\n")bracket(')')semicolon()
            bracket('}')
            //else {
            keyword("else")bracket('{')
                //vysl = 1;
                identifier("vysl")assign()integerLiteral(1)semicolon()
                //while (a > 0) {
                keyword("while")bracket('(')identifier("a")operatorLogic(">")integerLiteral(0)bracket(')')bracket('{')
                //vysl = vysl * a;
                identifier("vysl")assign()identifier("vysl")operatorArithmetic('*')identifier("a")semicolon()
                //a = a - 1;
                identifier("a")assign()identifier("a")operatorArithmetic('-')integerLiteral(1)semicolon()
                //}
                bracket('}')
                //ifj16.print("Vysledek je: " + vysl + "\n");
                identifierFull("ifj16", "print")bracket('(')stringLiteral("Vysledek je: ")operatorArithmetic('+')identifier("vysl")operatorArithmetic('+')stringLiteral("\\n")bracket(')')semicolon()
            bracket('}')
        bracket('}')
    bracket('}')
    ondOfFile()

    printf("**************** END OF test1 ****************\n");
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
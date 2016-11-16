//
// Created by Jakub Fajkus on 13.11.16.
//

#include "ifj16.h"
#include "SymbolTable.h"
#include "LexicalAnalyzer.h"
#include "SyntacticalAnalyzer.h"
#include "Stack.h"

void simulateSymbolTable();

int main(int argc, char *argv[])
{
    simulateSymbolTable();

    printf("\n\n\nXXXXKONECXXXX");
    return 0;
}

void simulateSymbolTable() {
    //simulace
    SYMBOL_TABLE_NODEPtr *globalSymbolTable;
    initializeSymbolTable(&globalSymbolTable);

    createAndInsertIntVariable(globalSymbolTable, "class1.firstVariable", true);
    createAndInsertIntVariable(globalSymbolTable, "class1.*", false);
    createAndInsertDoubleVariable(globalSymbolTable, "class1.double", true);
    createAndInsertStringVariable(globalSymbolTable, "class1.string", true);

    //try find a variable
    TREE_NODE_DATA foundNode;
    int found = BSTSearch(*globalSymbolTable, "class1.firstVariable", &foundNode);

    TREE_NODE_DATA foundNode2;
    int found2 = BSTSearch(*globalSymbolTable, "class1.*", &foundNode2);

    TREE_NODE_DATA foundNode3;
    int found3 = BSTSearch(*globalSymbolTable, "bulllshit", &foundNode3);

    TREE_NODE_DATA foundNode4;
    int found4 = BSTSearch(*globalSymbolTable, "class1.*", &foundNode3);
}
//
// Created by Jakub Fajkus on 13.11.16.
//

#include "IAL/ial.h"
#include "IFJ16Lib/ifj16.h"
#include "LexicalAnalyzer/LexicalAnalyzer.h"
#include "SyntacticalAnalyzer/SyntacticalAnalyzer.h"

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

    TREE_NODE_DATA *firstClassRecord = malloc(sizeof(TREE_NODE_DATA));
    firstClassRecord->item = malloc(sizeof(SYMBOL_TABLE_ITEM));
    firstClassRecord->item->variable = malloc(sizeof(SYMBOL_TABLE_VARIABLE));
    firstClassRecord->item->variable->name = "*";
    BSTInsert(globalSymbolTable, "class1.*", *firstClassRecord);

    VARIABLE_VALUE value;
    value.intValue = 71;
    SYMBOL_TABLE_VARIABLE *firstClassVariable = createVariable(globalSymbolTable, "class1.firstVariable", TYPE_INT, value, true);
    TREE_NODE_DATA treeData = createVariableData(firstClassVariable);

    BSTInsert(globalSymbolTable, firstClassVariable->name, treeData);
    TREE_NODE_DATA foundNode;
    int found = BSTSearch(*globalSymbolTable, "class1.firstVariable", &foundNode);
}
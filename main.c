//
// Created by Jakub Fajkus on 13.11.16.
//

#include "ifj16.h"
#include "SymbolTable.h"
#include "LexicalAnalyzer.h"
#include "SyntacticalAnalyzer.h"
#include "Stack.h"
#include "LexicalAnalyzerStructures.h"
#include "Interpret.h"
#include "instruction.h"
//#include "BasicStructures.h"
//#include "Debug.h"

void simulateSymbolTable();

int main(int argc, char *argv[])
{
    VARIABLE_VALUE val;
    val.doubleValue = 2.3;
    INSTRUCTION *instruction = createPushGlobalVariable("prom1", TYPE_DOUBLE, val);
//    simulateSymbolTable();

    if (argc != 2) {
        printf("no input file found\n");
        return 99;
    }

    runSyntacticalAnalysis(argv[1]);

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
//    TREE_NODE_DATA foundNode;
    SYMBOL_TABLE_VARIABLE *variable1 = getVariableFromTable(globalSymbolTable, "class1.firstVariable");
//    int found = BSTSearch(*globalSymbolTable, "class1.firstVariable", &foundNode);

//    TREE_NODE_DATA foundNode2;
    SYMBOL_TABLE_VARIABLE *variable2 = getVariableFromTable(globalSymbolTable, "class1.*");
//    int found2 = BSTSearch(*globalSymbolTable, "class1.*", &foundNode2);

//    TREE_NODE_DATA foundNode3;
//    int found3 = BSTSearch(*globalSymbolTable, "bulllshit", &foundNode3);
    SYMBOL_TABLE_VARIABLE *variable3 = getVariableFromTable(globalSymbolTable, "bullshit");

    SYMBOL_TABLE_FUNCTION *function1 = createAndInsertFunction(globalSymbolTable, "class1.function1", TYPE_VOID, 0, NULL);
    SYMBOL_TABLE_FUNCTION *function1Found = getFunctionFromTable(globalSymbolTable, "class1.function1");

    createAndInsertIntVariable(&function1->localSymbolTable, "a", true);
    createAndInsertIntVariable(&function1->localSymbolTable, "var1", false);
    createAndInsertIntVariable(&function1->localSymbolTable, "godDamnitLongVariableNameHeh", true);

    addFunctionParameter(function1, "firstParameter", TYPE_INT);
    addFunctionParameter(function1, "secondParameter", TYPE_DOUBLE);
    addFunctionParameter(function1, "thirdParameter", TYPE_STRING);
}
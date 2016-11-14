//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_SYMBOLTABLE_H
#define IFJ_SYMBOLTABLE_H

#include <stdbool.h>
#include "BasicStructures.h"
#include "DoubleLinkedList.h"

typedef struct {
    char *name; //name of the variable
    DATA_TYPE type; //type of the variable
    VARIABLE_VALUE value; //value of the variable
    unsigned int usages; //number of usages of the variable
    bool initialized; //whether the variable ahs been initialized(has value)
} SYMBOL_TABLE_VARIABLE;

typedef struct {
    char *name; //name of the function
    DATA_TYPE type; //return type of the function
    unsigned int usages; //number of usages of the function
    tDLList *parameters;
} SYMBOL_TABLE_FUNCTION;

typedef struct {
    SYMBOL_TABLE_FUNCTION *calledFunction; //pointer to the global symbol table
    VARIABLE_VALUE value; //value returned by the function
    struct SYMBOL_TABLE_NODE *symbolTable; //local symbol table for the function call
} SYMBOL_TABLE_FUNCTION_CALL;

//the symbol table can contain 3 types of nodes
typedef union{
    SYMBOL_TABLE_FUNCTION *function; //function node - for function definition in class
    SYMBOL_TABLE_VARIABLE *variable; //variable node - for variable definition in class or in a function
    SYMBOL_TABLE_FUNCTION_CALL *functionCall; //functionCall node - when there is a function called in an other function - it has a pointer to a new local symbol table
} SYMBOL_TABLE_ITEM;

typedef struct {
    SYMBOL_TABLE_ITEM *item;
    TREE_NODE_DATA_TYPE type;
} TREE_NODE_DATA;

typedef struct SYMBOL_TABLE_NODE {
    char* key;			                                         /* klíč */
    TREE_NODE_DATA * data;                                            /* užitečný obsah uzlu */
    struct SYMBOL_TABLE_NODE * lPtr;                                    /* levý podstrom */
    struct SYMBOL_TABLE_NODE * rPtr;                                   /* pravý podstrom */
} *SYMBOL_TABLE_NODEPtr;

void BSTInit   (SYMBOL_TABLE_NODEPtr *);
bool BSTSearch  (SYMBOL_TABLE_NODEPtr, char*, TREE_NODE_DATA *);
void BSTInsert (SYMBOL_TABLE_NODEPtr *, char*, TREE_NODE_DATA);
void BSTDelete (SYMBOL_TABLE_NODEPtr *, char*);
void BSTDispose(SYMBOL_TABLE_NODEPtr *);

/**
 * Get variable from given symbol table
 *
 * @param symbolTable
 * @param name
 * @return SYMBOL_TABLE_VARIABLE|null
 */
SYMBOL_TABLE_VARIABLE* getVariableFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name);

/**
 * Get variable from local symbol table. If the variable does not exist in the local symbol table, get it from the global table.
 * If the variable is not in the global table, return null.
 * @param symbolTable
 * @param name
 * @return
 */
SYMBOL_TABLE_VARIABLE* getVariable(SYMBOL_TABLE_NODEPtr *localSymbolTable, SYMBOL_TABLE_NODEPtr *globalSymbolTable, SYMBOL_TABLE_FUNCTION *calledFunction, char* name);

/**
 * Alloc memory for a symbol table
 *
 * @param symbolTable
 */
void initializeSymbolTable(SYMBOL_TABLE_NODEPtr **symbolTable);

/**
 * Create a SYMBOL_TABLE_VARIABLE pointer with given values. This represents creating a record for a symbol table.
 *
 * @param symbolTable
 * @param name
 * @param type
 * @param value1
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param value
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createIntVariable(char *name, int value, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param value
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createDoubleVariable(char *name, double value, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param value
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createStringVariable(char *name, char *value, bool initialized);

/**
 * Wrap SYMBOL_TABLE_VARIABLE into a structure which represents a tree node's data
 *
 * @param variable
 * @return
 */
TREE_NODE_DATA* createVariableData(SYMBOL_TABLE_VARIABLE *variable);

/**
 * Create variable and insert it in the given symbol table
 *
 * @param symbolTable
 * @param name
 * @param type
 * @param value
 * @param initialized
 */
void createAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, VARIABLE_VALUE value, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param value
 * @param initialized
 */
void createAndInsertIntVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, int value, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param value
 * @param initialized
 */
void createAndInsertDoubleVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, double value, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param value
 * @param initialized
 */
void createAndInsertStringVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, char* value, bool initialized);


#endif //IFJ_SYMBOLTABLE_H

//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_SYMBOLTABLE_H
#define IFJ_SYMBOLTABLE_H

#include <stdbool.h>
#include "BasicStructures.h"
#include "DoubleLinkedList.h"

/*
 * global symbol table will contain only variables(without their values) and functions
 * local symbol table will contain only variables
 * ech function has its own local symbol table
 * when calling a function's symbol table is taken, and a new frame is created from it
 * the old functionCall will be removed and replaced with frames
 */

/*
 * the local symbol table for function will probably used only in syntactical analysis to determine if a variable was declared and initialized
 */

typedef struct {
    char *name; //name of the variable
    DATA_TYPE type; //type of the variable
    unsigned int usages; //number of usages of the variable
    bool initialized; //whether the variable ahs been initialized(has value)
} SYMBOL_TABLE_VARIABLE;

typedef struct {
    char *name; //name of the function
    DATA_TYPE type; //return type of the function
    unsigned int usages; //number of usages of the function
    tDLList *parameters; //parameters of the function
} SYMBOL_TABLE_FUNCTION;

//the symbol table can contain 3 types of nodes
typedef union{
    SYMBOL_TABLE_FUNCTION *function; //function node - for function definition in class
    SYMBOL_TABLE_VARIABLE *variable; //variable node - for variable definition in class or in a function
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

SYMBOL_TABLE_FUNCTION* getFunctionFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name);

TREE_NODE_DATA *getNodeDataFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name);

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
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createVariable(char *name, DATA_TYPE type, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createIntVariable(char *name, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createDoubleVariable(char *name, bool initialized);

/**
 * Wraper arount createVariable
 *
 * @param name
 * @param initialized
 * @return
 */
SYMBOL_TABLE_VARIABLE* createStringVariable(char *name, bool initialized);

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
 * @param initialized
 */
void createAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param initialized
 */
void createAndInsertIntVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param initialized
 */
void createAndInsertDoubleVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized);

/**
 * Wrapper around createAndInsertVariable
 *
 * @param symbolTable
 * @param name
 * @param initialized
 */
void createAndInsertStringVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized);

/**
 * Create SYMBOL_TABLE_FUNCTION structure.
 * @param name
 * @param type
 * @param usages
 * @param parameters
 * @return
 */
SYMBOL_TABLE_FUNCTION* createFunction(char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters);

/**
 * Wrap SYMBOL_TABLE_FUNCTION into TREE_NODE_DATA which can be inserted into a tree.
 * @param function
 * @return
 */
TREE_NODE_DATA* createFunctionData(SYMBOL_TABLE_FUNCTION *function);

/**
 * @param symbolTable
 * @param name
 * @param type
 * @param usages
 * @param parameters
 */
SYMBOL_TABLE_FUNCTION *createAndInsertFunction(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters);

void addFunctionParameter(SYMBOL_TABLE_FUNCTION *function, char *name, DATA_TYPE type);

#endif //IFJ_SYMBOLTABLE_H

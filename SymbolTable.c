//
// Created by Jakub Fajkus on 14.11.16.
//

#include "SymbolTable.h"
#include "ifj16.h"

void BSTInit (SYMBOL_TABLE_NODEPtr *RootPtr) {
    *RootPtr = NULL;
}

bool BSTSearch (SYMBOL_TABLE_NODEPtr RootPtr, char* K, TREE_NODE_DATA *Content)	{
    if (RootPtr == NULL) {
        return false;
    }

    //the key is the root
    if (K == RootPtr->key) {
        *Content = *RootPtr->data;
        return true;
    } else if (K < RootPtr->key && RootPtr->lPtr != NULL) {
        //the has lower value than the root
        return BSTSearch(RootPtr->lPtr, K, Content);
    } else if (RootPtr->rPtr != NULL){
        //the has higher value than the root
        return BSTSearch(RootPtr->rPtr, K, Content);
    } else {
        return false;
    }
}

void BSTInsert (SYMBOL_TABLE_NODEPtr* RootPtr, char* K, TREE_NODE_DATA Content)	{
    if (NULL == *RootPtr) {
        SYMBOL_TABLE_NODEPtr newItem = malloc(sizeof(struct SYMBOL_TABLE_NODE));

        newItem->key = K;
        newItem->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        newItem->data->item = Content.item;
        newItem->data->type = Content.type;
        newItem->lPtr = NULL;
        newItem->rPtr = NULL;

        (*RootPtr) = newItem;
        return;
    }

    //the key is the root
    if (K == (*RootPtr)->key) {
        (*RootPtr)->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        (*RootPtr)->data->item = Content.item;
        (*RootPtr)->data->type = Content.type;
    } else if (K < (*RootPtr)->key) {
        //the has lower value than the root
        BSTInsert(&(*RootPtr)->lPtr, K, Content);
    } else {
        //the has higher value than the root
        BSTInsert(&(*RootPtr)->rPtr, K, Content);
    }
}

void ReplaceByRightmost (SYMBOL_TABLE_NODEPtr PtrReplaced, SYMBOL_TABLE_NODEPtr *RootPtr) {
    if (NULL == RootPtr) {
        return;
    }

    //if the current node has right subtree and the root node of the subtree has no right subtree - it is the rightmost
    if (NULL != (*RootPtr)->rPtr && NULL == (*RootPtr)->rPtr->rPtr) {
        PtrReplaced->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        PtrReplaced->data->item = (*RootPtr)->rPtr->data->item;
        PtrReplaced->data->type = (*RootPtr)->rPtr->data->type;
        PtrReplaced->key = (*RootPtr)->rPtr->key;

        free((*RootPtr)->rPtr);
        (*RootPtr)->rPtr = NULL;

        //the current node has no right subtree - it it the rightmost
    } else if(NULL == (*RootPtr)->rPtr) {
        TREE_NODE_DATA *data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        data->type = (*RootPtr)->data->type;
        data->item = (*RootPtr)->data->item;

        char* key = (*RootPtr)->key;

        BSTDelete(&PtrReplaced, (*RootPtr)->key);

        PtrReplaced->data = data;
        PtrReplaced->key = key;
    } else {
        ReplaceByRightmost(PtrReplaced, &(*RootPtr)->rPtr);
    }

}

void BSTDelete (SYMBOL_TABLE_NODEPtr *RootPtr, char* K) {
    SYMBOL_TABLE_NODEPtr actualItem = *RootPtr;
    SYMBOL_TABLE_NODEPtr temp;

    //the key is the root
    if (K == (*RootPtr)->key) {
        //let the game begin

        //if the item has only right subtree
        if (NULL == (*RootPtr)->lPtr && NULL != (*RootPtr)->rPtr) {
            temp = (*RootPtr)->rPtr;
            free(*RootPtr);
            *RootPtr = temp;

            //if the item has only left subtree
        } else if (NULL != (*RootPtr)->lPtr && NULL == (*RootPtr)->rPtr) {
            temp = (*RootPtr)->lPtr;
            free(*RootPtr);
            *RootPtr = temp;

            //the item has two subtrees
        } else if(NULL != (*RootPtr)->lPtr && NULL != (*RootPtr)->rPtr){
            ReplaceByRightmost((*RootPtr), &(*RootPtr)->lPtr);
            //both subtrees are null
        } else {
            (*RootPtr) = NULL;
            free(actualItem);
        }

    } else if (K < (*RootPtr)->key && (*RootPtr)->lPtr != NULL) {
        //the has lower value than the root
        BSTDelete(&(*RootPtr)->lPtr, K);
    } else if ((*RootPtr)->rPtr != NULL){
        //the has higher value than the root
        BSTDelete(&(*RootPtr)->rPtr, K);
    }
}

void BSTDispose (SYMBOL_TABLE_NODEPtr *RootPtr) {

    if (NULL == *RootPtr) {
        return;
    }

    if (NULL != (*RootPtr)->lPtr) {
        BSTDispose(&(*RootPtr)->lPtr);
    }

    if (NULL != (*RootPtr)->rPtr) {
        BSTDispose(&(*RootPtr)->rPtr);
    }

    if(NULL == (*RootPtr)->lPtr && NULL == (*RootPtr)->rPtr) {
        free(*RootPtr);
        *RootPtr = NULL;
    }
}

SYMBOL_TABLE_VARIABLE* getVariableFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name)
{
    TREE_NODE_DATA nodeData;

    if (true == BSTSearch(*symbolTable, name, &nodeData)) {
        if (nodeData.type != TREE_NODE_VARIABLE) {
            printf("ERROR: getVariableFromTable: searched for variable %s, but it is not a variable", name);
            exit(1);
        }
        //found
        return nodeData.item->variable;
    } else {
//        not found
        return NULL;
    }
}

SYMBOL_TABLE_VARIABLE* getVariable(SYMBOL_TABLE_NODEPtr *localSymbolTable, SYMBOL_TABLE_NODEPtr *globalSymbolTable, SYMBOL_TABLE_FUNCTION *calledFunction, char* name)
{
    SYMBOL_TABLE_VARIABLE *variable;

    variable = getVariableFromTable(localSymbolTable, name);

    //the variable was found and is local
    if(variable != NULL) {
        return variable;
    }

    //when searching in the global table, there are 2 situations:
    //1. the searching variable is ordinary identifier(e.g. property) - in this case we want to prefix the name with class name(the class that contains the function definition!)
    //2. the searching variable is fully qualified name(e.g. class.property) - in this case we want to search for the name as it is

    //if it does not contain a dot
    if (-1 != ifj16_find(name, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(calledFunction->name, 0, ifj16_find(calledFunction->name, "."));
        char *fullyQualifiedName = malloc(sizeof(char)*(strlen(name) + strlen(className) + 2)); //size of argument name + size of class + dot + \0
        strcat(fullyQualifiedName, className);
        strcat(fullyQualifiedName, ".");
        strcat(fullyQualifiedName, name);

        variable = getVariableFromTable(globalSymbolTable, fullyQualifiedName);

    } else {
        //the name is already fully qualified
        variable = getVariableFromTable(globalSymbolTable, name);
    }

    //the variable was either found(is global) or not found at all
    return variable;
}

void initializeSymbolTable(SYMBOL_TABLE_NODEPtr **symbolTable) {
    *symbolTable = malloc(sizeof(SYMBOL_TABLE_NODEPtr));
    **symbolTable = malloc(sizeof(struct SYMBOL_TABLE_NODE));
    BSTInit(*symbolTable);
}

SYMBOL_TABLE_VARIABLE* createVariable(char *name, DATA_TYPE type, bool initialized) {
    SYMBOL_TABLE_VARIABLE *variable = malloc(sizeof(SYMBOL_TABLE_VARIABLE));
    variable->name = name;
    variable->type = type;
    variable->usages = 0;
    variable->initialized = initialized;

    return variable;
}

SYMBOL_TABLE_VARIABLE* createIntVariable(char *name, bool initialized) {
    return createVariable(name, TYPE_INT, initialized);
}

SYMBOL_TABLE_VARIABLE* createDoubleVariable(char *name, bool initialized) {
    return createVariable(name, TYPE_DOUBLE, initialized);
}

SYMBOL_TABLE_VARIABLE* createStringVariable(char *name, bool initialized) {
    return createVariable(name, TYPE_STRING, initialized);
}

TREE_NODE_DATA* createVariableData(SYMBOL_TABLE_VARIABLE *variable) {
    TREE_NODE_DATA *treeData = malloc(sizeof(TREE_NODE_DATA));
    treeData->type = TREE_NODE_VARIABLE;
    treeData->item = malloc(sizeof(SYMBOL_TABLE_ITEM));
    treeData->item->variable = variable;

    return treeData;
}

void createAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, bool initialized) {
    SYMBOL_TABLE_VARIABLE *variable = createVariable(name, type, initialized);
    TREE_NODE_DATA *treeData = createVariableData(variable);
    BSTInsert(symbolTable, variable->name, *treeData);
}

void createAndInsertIntVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    createAndInsertVariable(symbolTable, name, TYPE_INT, initialized);
}
void createAndInsertDoubleVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    createAndInsertVariable(symbolTable, name, TYPE_DOUBLE, initialized);
}
void createAndInsertStringVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    createAndInsertVariable(symbolTable, name, TYPE_STRING, initialized);
}

SYMBOL_TABLE_FUNCTION* createFunction(char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters) {
    SYMBOL_TABLE_FUNCTION *function = malloc(sizeof(SYMBOL_TABLE_FUNCTION));

    function->type = type;
    function->name = name;
    function->usages = usages;
    function->parameters = parameters;

    return function;
}

TREE_NODE_DATA* createFunctionData(SYMBOL_TABLE_FUNCTION *function) {
    TREE_NODE_DATA *treeData = malloc(sizeof(TREE_NODE_DATA));
    treeData->type = TREE_NODE_FUNCTION;
    treeData->item = malloc(sizeof(SYMBOL_TABLE_ITEM));
    treeData->item->function = function;

    return treeData;
}

void createAndInsertFunction(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters) {
    SYMBOL_TABLE_FUNCTION *function = createFunction(name, type, usages, parameters);
    TREE_NODE_DATA *treeData = createFunctionData(function);
    BSTInsert(symbolTable, function->name, *treeData);
}
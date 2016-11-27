//
// Created by Jakub Fajkus on 14.11.16.
//

#include "SymbolTable.h"
#include "ifj16.h"
#include "Debug.h"
#include "Stack.h"

void BSTInit (SYMBOL_TABLE_NODEPtr *RootPtr) {
    *RootPtr = NULL;
}

bool BSTSearch (SYMBOL_TABLE_NODEPtr RootPtr, char* K, TREE_NODE_DATA *Content)	{
    if (RootPtr == NULL) {
        return false;
    }

    //the key is the root
    if (strcmp(K, RootPtr->key) == 0) {
        *Content = *RootPtr->data;
        return true;
    } else if (strcmp(K, RootPtr->key) == -1 && RootPtr->lPtr != NULL) {
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
        //aktalizacni semantika...
        printf("redeclaration of %s", K);
        exit(3);

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
    TREE_NODE_DATA *nodeData = getNodeDataFromTable(symbolTable, name);

    if(nodeData == NULL) {
        return NULL;
    }

    if (nodeData->type != TREE_NODE_VARIABLE) {
        debugPrintf("internal error, requested %s is not a variable", name);
        exit(99);
    }

    return nodeData->item->variable;
}

SYMBOL_TABLE_FUNCTION* getFunctionFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name)
{
    TREE_NODE_DATA *nodeData = getNodeDataFromTable(symbolTable, name);

    if(nodeData == NULL) {
        return NULL;
    }

    if (nodeData->type != TREE_NODE_FUNCTION) {
        debugPrintf("internal error, requested %s is not a function", name);
        exit(99);
    }

    return nodeData->item->function;
}

TREE_NODE_DATA *getNodeDataFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name) {
    TREE_NODE_DATA *nodeData = malloc(sizeof(TREE_NODE_DATA));

    if (true == BSTSearch(*symbolTable, name, nodeData)) {
        //found
        return nodeData;
    } else {
//        not found
        return NULL;
    }

    return NULL;
}

SYMBOL_TABLE_VARIABLE* getVariable(SYMBOL_TABLE_NODEPtr *localSymbolTable, SYMBOL_TABLE_NODEPtr *globalSymbolTable, SYMBOL_TABLE_FUNCTION *calledFunction, char* name)
{
    SYMBOL_TABLE_VARIABLE *variable = NULL;

    variable = getVariableFromTable(localSymbolTable, name);

    //the variable was found and is local
    if(variable != NULL) {
        return variable;
    }

    //when searching in the global table, there are 2 situations:
    //1. the searching variable is ordinary identifier(e.g. property) - in this case we want to prefix the name with class name(the class that contains the function definition!)
    //2. the searching variable is fully qualified name(e.g. class.property) - in this case we want to search for the name as it is

    //if it does not contain a dot
    if (-1 == ifj16_find(name, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(calledFunction->name, 0, ifj16_find(calledFunction->name, "."));
        char *nameWithDot = stringConcat(className, ".");
        char *fullyQualifiedName = stringConcat(nameWithDot, name);
        free(nameWithDot);

        variable = getVariableFromTable(globalSymbolTable, fullyQualifiedName);

    } else {
        //the name is already fully qualified
        variable = getVariableFromTable(globalSymbolTable, name);
    }

    //the variable was either found(is global) or not found at all
    return variable;
}

void initializeSymbolTable(struct SYMBOL_TABLE_NODE *symbolTable) {
    symbolTable = malloc(sizeof(struct SYMBOL_TABLE_NODE));
    BSTInit(&symbolTable);
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

SYMBOL_TABLE_VARIABLE* createAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, bool initialized) {
    //if the variable already exists
    if (NULL != getVariableFromTable(symbolTable, name)) {
        printf("redeclaration of variable %s\n", name);
        exit(3);
    }

    SYMBOL_TABLE_VARIABLE *variable = createVariable(name, type, initialized);
    TREE_NODE_DATA *treeData = createVariableData(variable);
    debugPrintf("inserting variable %s", variable->name);
    BSTInsert(symbolTable, variable->name, *treeData);

    return variable;
}

SYMBOL_TABLE_VARIABLE* createAndInsertIntVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    return createAndInsertVariable(symbolTable, name, TYPE_INT, initialized);
}
SYMBOL_TABLE_VARIABLE* createAndInsertDoubleVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    return createAndInsertVariable(symbolTable, name, TYPE_DOUBLE, initialized);
}
SYMBOL_TABLE_VARIABLE* createAndInsertStringVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, bool initialized) {
    return createAndInsertVariable(symbolTable, name, TYPE_STRING, initialized);
}

SYMBOL_TABLE_FUNCTION* createFunction(char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters, tDLList *instructions) {
    SYMBOL_TABLE_FUNCTION *function = malloc(sizeof(SYMBOL_TABLE_FUNCTION));

    //initialize symbol table
    function->localSymbolTable = NULL;

    function->type = type;
    function->name = name;
    function->usages = usages;
    function->parameters = parameters;
    function->instructions = instructions;

    return function;
}

TREE_NODE_DATA* createFunctionData(SYMBOL_TABLE_FUNCTION *function) {
    TREE_NODE_DATA *treeData = malloc(sizeof(TREE_NODE_DATA));
    treeData->type = TREE_NODE_FUNCTION;
    treeData->item = malloc(sizeof(SYMBOL_TABLE_ITEM));
    treeData->item->function = function;

    return treeData;
}

SYMBOL_TABLE_FUNCTION* createAndInsertFunction(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters, tDLList *instructions) {
    //if the function already exists

    if (NULL != getNodeDataFromTable(symbolTable, name)) {
        debugPrintf("redeclaration of function or variable %s", name);
        exit(3);
    }

    SYMBOL_TABLE_FUNCTION *function = createFunction(name, type, usages, parameters, instructions);
    if (function->parameters == NULL) {
        function->parameters = malloc(sizeof(tDLList));
        ListInit(function->parameters);
    }

    if (function->instructions == NULL) {
        function->instructions = malloc(sizeof(tDLList));
        ListInit(function->instructions);
    }

    TREE_NODE_DATA *treeData = createFunctionData(function);
    BSTInsert(symbolTable, function->name, *treeData);

//    function->localSymbolTable = malloc(sizeof(struct SYMBOL_TABLE_NODE *));
//    initializeSymbolTable(function->localSymbolTable);

    return function;
}

void addFunctionParameter(SYMBOL_TABLE_FUNCTION *function, char *name, DATA_TYPE type) {
    if (function->parameters == NULL) {
        function->parameters = malloc(sizeof(tDLList));
        ListInit(function->parameters);
    }

    LIST_ELEMENT element;
    element.type = LIST_ELEMENT_TYPE_FUNCTION_PARAMETER;
    element.data.parameter = malloc(sizeof(FUNCTION_PARAMETER));
    element.data.parameter->name = name;
    element.data.parameter->type = type;

    ListInsertLast (function->parameters, element);
}

void addFunctionInstruction(SYMBOL_TABLE_FUNCTION *function, INSTRUCTION *instruction) {
    if (function->instructions == NULL) {
        function->instructions = malloc(sizeof(tDLList));
        ListInit(function->instructions);
    }

    LIST_ELEMENT element;
    element.type = LIST_ELEMENT_TYPE_INSTRUCTION;
    element.data.parameter = malloc(sizeof(FUNCTION_PARAMETER));
    element.data.instr = instruction;

    ListInsertLast (function->instructions, element);
}

void checkIfVariableIsInitialized(SYMBOL_TABLE_NODEPtr *symbolTable, char *name) {
    SYMBOL_TABLE_VARIABLE* foundVariable = getVariableFromTable(symbolTable, name);

    if (foundVariable == NULL) {
        debugPrintf("checkIfVariableIsInitialized: variable %s was not found", name);
        exit(99);
    }

    if (foundVariable->initialized == false) {
        printf("variable %s is not initialized", name);
        exit(8);
    }
}

LIST_ELEMENT *createFunctionParamListElement(DATA_TYPE type, char* name) {
    FUNCTION_PARAMETER *param = malloc(sizeof(FUNCTION_PARAMETER));
    param->type = type;
    param->name = name;
    LIST_ELEMENT *listElement = malloc(sizeof(LIST_ELEMENT));
    listElement->type = LIST_ELEMENT_TYPE_FUNCTION_PARAMETER;
    listElement->data.parameter = param;

    return listElement;
}

void Leftmost_Inorder(struct SYMBOL_TABLE_NODE *ptr, struct STACK_STR *Stack){
    while ( ptr != NULL ) {
        struct STACK_ELEMENT element;
        element.type = STACK_ELEMENT_TYPE_SYMBOL_TABLE_PTR;
        element.data.symbolTableNode = ptr;
        stackPush(Stack, element);
        ptr = ptr->lPtr;
    }

}

tStack *BTInorder (struct SYMBOL_TABLE_NODE *RootPtr){
    tStack *StackHelper = malloc(sizeof(tStack));
    stackInit(StackHelper);
    tStack *ReturnStack = malloc(sizeof(tStack));
    stackInit(ReturnStack);

    Leftmost_Inorder (RootPtr, StackHelper);

    while ( !stackEmpty(StackHelper) ) {
        //get top element
        STACK_ELEMENT *elem = malloc(sizeof(STACK_ELEMENT));
        stackTop(StackHelper, elem);
        RootPtr = elem->data.symbolTableNode;
        //pop top element
        stackPop(StackHelper);

        //push to return stack
        stackPush(ReturnStack, *elem);
        Leftmost_Inorder (RootPtr->rPtr, StackHelper);
    }

    return ReturnStack;
}
/**
 * Implementace interpretu imperativního jazyka IFJ16.
 *
 * Jakub Fajkus
 * Richard Bureš
 * David Czernin
 */

#include "semanticalanalyzer.h"
#include "syntacticalanalyzer.h"
#include "ifj16.h"
#include "basicstructures.h"
#include "symboltable.h"
#include "debug.h"

struct SYMBOL_TABLE_NODE *globalSymbolTable;
extern SYMBOL_TABLE_FUNCTION *actualFunction;
extern char* actualClass;


void semanticCheckForFunctionRun() {
    // get function Main.run
    SYMBOL_TABLE_FUNCTION *function = getFunctionFromTable(&globalSymbolTable, "Main.run");
    if (function == NULL) {
        fprintf(stderr, "semanticCheckForFunctionRun: function %s not found", "Main.run");
        exit(3);
    }

    if(function->type != TYPE_VOID){
        fprintf(stderr, "semanticCheckForFunctionRun: function Main.Run is not void");
        exit(3);
    }
}

SYMBOL_TABLE_FUNCTION *semantic_getFunction(char * functionName) {
    SYMBOL_TABLE_FUNCTION *function = NULL;
    if (-1 == ifj16_find(functionName, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(actualFunction->name, 0, ifj16_find(actualFunction->name, "."));
        char *nameWithDot = stringConcat(className, ".");
        char *fullyQualifiedName = stringConcat(nameWithDot, functionName);
        free(nameWithDot);

        function = getFunctionFromTable(&globalSymbolTable, fullyQualifiedName);
        free(className);
        free(fullyQualifiedName);
    } else {
        //the name is already fully qualified
        function = getFunctionFromTable(&globalSymbolTable, functionName);
    }
    if(function == NULL){
        fprintf(stderr, "semantic_getFunction: function %s not found", functionName);
        exit(3);
    }
    return function;
}

//SYMBOL_TABLE_VARIABLE *semantic_getVariable(char * name) {
//    SYMBOL_TABLE_VARIABLE *variable = NULL;
//    if (-1 == ifj16_find(name, ".")) {
//        //add class prefix to it
//        char* className = ifj16_substr(actualFunction->name, 0, ifj16_find(actualFunction->name, "."));
//        char *nameWithDot = stringConcat(className, ".");
//        char *fullyQualifiedName = stringConcat(nameWithDot, name);
//        free(nameWithDot);
//
//        variable = getVariableFromTable(&globalSymbolTable, fullyQualifiedName);
//        free(className);
//        free(fullyQualifiedName);
//    } else {
//        //the name is already fully qualified
//        variable = getVariableFromTable(&globalSymbolTable, name);
//    }
//    if(variable == NULL){
//        exit(3);
//    }
//    //the variable was either found(is global) or not found at all
//    return variable;
//}

SYMBOL_TABLE_VARIABLE *semantic_getVariable(char * name) {

    SYMBOL_TABLE_VARIABLE *variable;

    if(actualFunction == NULL) {
         variable = getVariable(NULL, &globalSymbolTable, actualClass, name);
    } else {
        variable = getVariable(&actualFunction->localSymbolTable, &globalSymbolTable, actualClass, name);
    }
    if(variable == NULL){
        exit(3);
    }

    //the variable was either found(is global) or not found at all
    return variable;
}

SYMBOL_TABLE_VARIABLE *semantic_getInitializedVariable(char * name) {
    SYMBOL_TABLE_VARIABLE *variable = semantic_getVariable(name);
    if (variable->initialized == false) {
        fprintf(stderr, "variable %s is not initialized", name);
        exit(8);
    }
    return variable;
}

void semantic_firstPass_testStaticVariable(char * name) {
    char *fullName;
    if (-1 == ifj16_find(name, ".")) {
        char *nameWithDot = stringConcat(actualClass, ".");
        fullName = stringConcat(nameWithDot, name);
        free(nameWithDot);
    }else {
        fullName = name;
    }
    if(actualFunction == NULL){
        if (getVariable(NULL, &globalSymbolTable, actualClass, fullName) == NULL) {
            SYMBOL_TABLE_VARIABLE *variable = createAndInsertVariable(&globalSymbolTable, stringConcat("-!-!-", fullName), TYPE_INT, true);
            variable->isMissingStaticVar = true;
        }
    }

}

void checkFunctionParametersType(char *functionName, tDLList *parameters){
    struct SYMBOL_TABLE_FUNCTION_STR *function = semantic_getFunction(functionName);
    ListFirst(parameters);
    ListFirst(function->parameters);
    while(true){
        if(function->parameters->Act->element.data.parameter->type != parameters->Act->element.data.parameter->type){
            exit(4);
        }
        if(function->parameters->Act == function->parameters->Last && parameters->Act == parameters->Last){
            break;
        }else if((function->parameters->Act == function->parameters->Last && parameters->Act != parameters->Last) ||
                (function->parameters->Act != function->parameters->Last && parameters->Act == parameters->Last)){
            fprintf(stderr, "Wrong number of Parameters");
            exit(4);
        }
    }
}

void semantical_ExistAndCorrectTypeOfReturns(SYMBOL_TABLE_NODEPtr *symbolTable) {

    struct SYMBOL_TABLE_NODE *actualEL = NULL;

    //new stack for checking all functions
    tStack *MujStack = malloc(sizeof(tStack));
    stackInit(MujStack);

    //put all there
    MujStack = BTInorder(globalSymbolTable);

    STACK_ELEMENT *elem = malloc(sizeof(STACK_ELEMENT));

    while(!stackEmpty(MujStack)){

        stackTop(MujStack, elem);
        actualEL = elem->data.symbolTableNode;

        if (actualEL != NULL && actualEL->data->type == TREE_NODE_FUNCTION && actualEL->data->item->function->type != TYPE_VOID) {
            if(actualEL->data->item->function->hasReturn == 0){
                fprintf(stderr, "Incompatible type of return value or missing return of non-void function");
                exit(8);
            }
        }

        //pop top element
        stackPop(MujStack);
    }
}



bool canConvertTypes(DATA_TYPE destination, DATA_TYPE source) {

    if ((destination == TYPE_INT && source == TYPE_INT)||(destination == TYPE_STRING && source == TYPE_STRING) ||(destination == TYPE_DOUBLE && source == TYPE_DOUBLE)){
        return true;
    }

    if (destination == TYPE_DOUBLE && source == TYPE_INT) {
        return true;
    }

    return false;
}

bool semantical_checkTypes(DATA_TYPE destination, DATA_TYPE source) {
    if (!canConvertTypes(destination, source)) {
        exit(4);
    }
}

bool semantical_checkFunctionCall(tDLList *declaredParameters, tDLList *usedParameters) {
    ListFirst(usedParameters);
    ListFirst(declaredParameters);

    if(DLSize(declaredParameters) != DLSize(usedParameters)) {
        fprintf(stderr, "the number of arguments does not match");
        exit(4);
    }

    while(DLActive(declaredParameters) ) {
        if(!canConvertTypes(declaredParameters->Act->element.data.parameter->type, usedParameters->Act->element.data.parameter->type)) {
            fprintf(stderr, "can not convert %d to %d", usedParameters->Act->element.data.parameter->type, declaredParameters->Act->element.data.parameter->type);
            exit(4);
        }

        //move to the next parameter
        ListSuccessor(declaredParameters);
        ListSuccessor(usedParameters);
    }
}

bool isFunctionFromIfj16(char *name) {
    return stringEquals("ifj16.readInt", name) || stringEquals("ifj16.readDouble", name) ||stringEquals("ifj16.readString", name) ||
        stringEquals("ifj16.print", name) ||stringEquals("ifj16.length", name) ||stringEquals("ifj16.substr", name) ||
        stringEquals("ifj16.compare", name) ||stringEquals("ifj16.find", name) ||stringEquals("ifj16.sort", name);
}

void createInstructionsToCallIfj16Function(char *functionName, tDLList *instructions, tDLList *givenParameters, char *returnValueToVariable) {
    //do not call DLFirst!
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));
    instruction->address_dst = NULL;
    instruction->address_src1 = NULL;
    instruction->address_src2 = NULL;

    if(stringEquals("ifj16.readInt", functionName)) {
        instruction->type = Instruction_Function_readInt;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.readDouble", functionName)) {

        instruction->type = Instruction_Function_readDouble;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.readString", functionName)) {
        instruction->type = Instruction_Function_readString;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.print", functionName)) {
        if (returnValueToVariable != NULL) {
            fprintf(stderr, "cannot use return value of void function %s\n", functionName);
            exit(8);
        }

        instruction->type = Instruction_Function_Print;
//        if(givenParameters->First->element.data.parameter->type != TYPE_STRING) {
//            exit(4);
//        }

        instruction->address_dst = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.length", functionName)) {
        instruction->type = Instruction_Function_Length;
        instruction->address_dst = returnValueToVariable;
        if(givenParameters->First->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.substr", functionName)) {
        ListFirst(givenParameters);

        instruction->type = Instruction_Function_Substr;
        tDLList *args = malloc(sizeof(tDLList));
        ListInit(args);

        instruction->address_dst = returnValueToVariable;

        InsertFirst(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, TYPE_STRING));
        ListFirst(args);
        if(givenParameters->Act->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        ListSuccessor(givenParameters);


        DLPostInsert(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, givenParameters->Act->element.data.parameter->type));
        if(givenParameters->Act->element.data.parameter->type != TYPE_INT) {
            exit(4);
        }
        ListSuccessor(givenParameters);


        DLPostInsert(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, givenParameters->Act->element.data.parameter->type));
        if(givenParameters->Act->element.data.parameter->type != TYPE_INT) {
            exit(4);
        }
        ListSuccessor(givenParameters);


        instruction->address_src1 = args;
        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));

    }else if(stringEquals("ifj16.compare", functionName)) {
        instruction->type = Instruction_Function_Compare;
        instruction->address_dst = returnValueToVariable;

        if(givenParameters->First->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;

        if(givenParameters->First->rptr->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src2 = givenParameters->First->rptr->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.find", functionName)) {

        instruction->type = Instruction_Function_Find;
        instruction->address_dst = returnValueToVariable;
        if(givenParameters->First->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;
        if(givenParameters->First->rptr->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src2 = givenParameters->First->rptr->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.sort", functionName)) {
        instruction->type = Instruction_Function_Sort;
        instruction->address_dst = returnValueToVariable;
        if(givenParameters->First->element.data.parameter->type != TYPE_STRING) {
            exit(4);
        }
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));

    } else {
        fprintf(stderr, "unknown function %s\n", functionName);
        exit(3);
    }
}

void semanticCheckForMissingStaticVar(){
    struct SYMBOL_TABLE_NODE *actualEL = NULL;

    //new stack for checking all functions
    tStack *MujStack = malloc(sizeof(tStack));
    stackInit(MujStack);

    //put all there
    MujStack = BTInorder(globalSymbolTable);

    STACK_ELEMENT *elem = malloc(sizeof(STACK_ELEMENT));

    while(!stackEmpty(MujStack)){

        stackTop(MujStack, elem);
        actualEL = elem->data.symbolTableNode;

        //check for errors 6
        if (actualEL != NULL && actualEL->data->type == TREE_NODE_VARIABLE && actualEL->data->item->variable->isMissingStaticVar == true) {
            SYMBOL_TABLE_VARIABLE *var = actualEL->data->item->variable;
            char *variableName = ifj16_substr(var->name, strlen("-!-!-"), strlen(var->name) - strlen("-!-!-"));

            SYMBOL_TABLE_VARIABLE *foundVar = getVariableFromTable(&globalSymbolTable, variableName);
            if (foundVar != NULL) {
                exit(6);
            }
        }

        //pop top element
        stackPop(MujStack);
    }
}

void initializeVariable(char *name) {
    SYMBOL_TABLE_VARIABLE *var = semantic_getVariable(name);
    var->initialized = true;
}

SYMBOL_TABLE_VARIABLE* semanticCreateAndInsertVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, bool initialized) {
    //1. inserting static var with name as static func
    //2. inserting local var with name as static func

    //covers the 1. case
    //find function with name actualClass.name
    TREE_NODE_DATA *node;
    int indexOfDot = ifj16_find(name, ".");
    if (indexOfDot != -1) {
    //there is a dot
        node = getNodeDataFromTable(symbolTable, name);

        if(node != NULL && node->type != TREE_NODE_VARIABLE) {
            fprintf(stderr, "the variable name can not have a name as a static function in the same class\n");
            exit(3);
        }
        //covers the 2. case
    } else {
        char *newName = stringConcat(stringConcat(actualClass, "."), name);
        node = getNodeDataFromTable(&globalSymbolTable, newName);

        if(node != NULL && node->type != TREE_NODE_VARIABLE) {
            fprintf(stderr, "the variable name can not have a name as a static function in the same class\n");
            exit(3);
        }
    }

    return createAndInsertVariable(symbolTable, name, type, initialized);
}

SYMBOL_TABLE_FUNCTION* semanticCreateAndInsertFunction(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, unsigned int usages, tDLList *parameters, tDLList *instructions, bool hasReturn) {
    //1. inserting static func with name as static var

    //covers the 1.
    //find function with name actualClass.name
    TREE_NODE_DATA *node;
    int indexOfDot = ifj16_find(name, ".");
    if (indexOfDot != -1) {
        //there is a dot
        node = getNodeDataFromTable(symbolTable, name);

        if(node != NULL) {
            fprintf(stderr, "the variable name can not have a name as a static function in the same class\n");
            exit(3);
        }

        //find the name without the dot
        char *substring = ifj16_substr(name, indexOfDot+1, strlen(name) - indexOfDot - 1);
        node = getNodeDataFromTable(symbolTable, substring);
    } else {
        node = getNodeDataFromTable(symbolTable, name);
    }

    if(node != NULL) {
        fprintf(stderr, "the variable name can not have a name as a static function in the same class\n");
        exit(3);
    }

    return createAndInsertFunction(symbolTable, name, type, usages, parameters, instructions, hasReturn);
}
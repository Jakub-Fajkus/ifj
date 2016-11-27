//
// Created by Jakub Fajkus on 23.11.16.
//

#include "semanticalanalyzer.h"
#include "syntacticalanalyzer.h"
#include "ifj16.h"
#include "basicstructures.h"
#include "symboltable.h"

struct SYMBOL_TABLE_NODE *globalSymbolTable;
SYMBOL_TABLE_FUNCTION *actualFunction;

void semanticCheckForFunctionRun() {
    // get function Main.run
    SYMBOL_TABLE_FUNCTION *function = getFunctionFromTable(&globalSymbolTable, "Main.run");
    if (function == NULL) {
        printf("semanticCheckForFunctionRun: function %s not found", "Main.run");
        exit(3);
    }
    if(function->type != TYPE_VOID){
        printf("semanticCheckForFunctionRun: function Main.Run is not void");
        exit(8);
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
        printf("semantic_getFunction: function %s not found", functionName);
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
    SYMBOL_TABLE_VARIABLE *variable = getVariable(&actualFunction->localSymbolTable, &globalSymbolTable, actualFunction, name);
    if(variable == NULL){
        exit(3);
    }

    //the variable was either found(is global) or not found at all
    return variable;
}

SYMBOL_TABLE_VARIABLE *semantic_getInitializedVariable(char * name) {
    SYMBOL_TABLE_VARIABLE *variable = semantic_getVariable(name);
    if (variable->initialized == false) {
        printf("variable %s is not initialized", name);
        exit(8);
    }
    return variable;
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
            printf("Wrong number of Parameters");
            exit(4);
        }
    }
}

bool canConvertTypes(DATA_TYPE destination, DATA_TYPE source) {
    if (destination == source) {
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
        printf("the number of arguments does not match");
        exit(4);
    }

    while(DLActive(declaredParameters) ) {
        if(!canConvertTypes(declaredParameters->Act->element.data.parameter->type, usedParameters->Act->element.data.parameter->type)) {
            printf("can not convert %d to %d", usedParameters->Act->element.data.parameter->type, declaredParameters->Act->element.data.parameter->type);
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

    if(stringEquals("ifj16.readInt", functionName)) {
        instruction->type = Instruction_Function_readInt;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.readDouble", functionName)) {

        instruction->type = Instruction_Function_readDouble;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.readString", functionName)) {
        if (returnValueToVariable != NULL) {
            printf("cannot use return value of void function %s\n", functionName);
            exit(8);
        }
        instruction->type = Instruction_Function_readString;
        instruction->address_dst = returnValueToVariable;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.print", functionName)) {
        if (returnValueToVariable != NULL) {
            printf("cannot use return value of void function %s\n", functionName);
            exit(8);
        }

        instruction->type = Instruction_Function_Print;
        instruction->address_dst = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.length", functionName)) {
        instruction->type = Instruction_Function_Length;
        instruction->address_dst = returnValueToVariable;
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.substr", functionName)) {
        ListFirst(givenParameters);

        instruction->type = Instruction_Function_Substr;
        tDLList *args = malloc(sizeof(tDLList));
        ListInit(args);

        ListInsertLast(args, createListElementWithFunctionParamameter(returnValueToVariable, TYPE_STRING));
        ListSuccessor(givenParameters);
        ListInsertLast(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, givenParameters->Act->element.data.parameter->type));
        ListSuccessor(givenParameters);
        ListInsertLast(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, givenParameters->Act->element.data.parameter->type));
        ListSuccessor(givenParameters);
        ListInsertLast(args, createListElementWithFunctionParamameter(givenParameters->Act->element.data.parameter->name, givenParameters->Act->element.data.parameter->type));

        instruction->address_dst = args;
        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));

    }else if(stringEquals("ifj16.compare", functionName)) {
        instruction->type = Instruction_Function_Compare;
        instruction->address_dst = returnValueToVariable;
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;
        instruction->address_src2 = givenParameters->First->rptr->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.find", functionName)) {

        instruction->type = Instruction_Function_Find;
        instruction->address_dst = returnValueToVariable;
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;
        instruction->address_src2 = givenParameters->First->rptr->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));
    }else if(stringEquals("ifj16.sort", functionName)) {
        instruction->type = Instruction_Function_Sort;
        instruction->address_dst = returnValueToVariable;
        instruction->address_src1 = givenParameters->First->element.data.parameter->name;

        ListInsertLast(instructions, wrapInstructionIntoListElement(instruction));

    } else {
        printf("unknown function %s\n", functionName);
        exit(3);
    }
}

void initializeVariable(char *name) {
    SYMBOL_TABLE_VARIABLE *var = semantic_getVariable(name);
    var->initialized = true;
}
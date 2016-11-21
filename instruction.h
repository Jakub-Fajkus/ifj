//
// Created by Andrej on 21.11.2016.
//

#ifndef IFJ_INSTRUCTION_H
#define IFJ_INSTRUCTION_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"

typedef enum {
    Instruction_Jump,
    Instruction_Assign,
    //----------------------- Math operations
    Instruction_Addition, // 3 params
    Instruction_Subtraction, // 3 params
    Instruction_Multiply, // 3 params
    Instruction_Divide, // 3 params
    //----------------------- Calling user function
    Instruction_CallFunction, // == Instruction_Push_Local_Frame
    Instruction_ReturnFunction, //not sure
    //-------------------------------- Built-in functions
    Instruction_Function_readInt, // 1 param (retvalue, type INT)
    Instruction_Function_readDouble, // 1 param (retvalue, type DOUBLE)
    Instruction_Function_readString, // 1 param (retvalue, type STRING)
    Instruction_Function_Print, // 1-n params, ANY TYPE
    Instruction_Function_Length, // 2 params (retvalue-int, param-string)
    Instruction_Function_Substr, // 4 params (retvalue-string, 3 params-string,int,int)
    Instruction_Function_Compare, // 3 params (retvalue-int, 2 params-string, string)
    Instruction_Function_Find, // 3 params (retvalue-int, 2 params-string,string)
    Instruction_Function_Sort, // 2 params (retvalue-string, param-string)
    //-------------------------------- Work with frames
    Instruction_Create_Global_Frame, // IDEA: do we even need any param?
    Instruction_Push_Global_Variable, // 3 params: variable name, variable type, variable value : best in this format
    Instruction_Create_Local_Frame, // IDEA: do we even need any param?
    Instruction_Push_Local_Variable // 3 params as in global var?

    //  Instruction_Begin,
    //  Instruction_End,
    //----------------------- Bool operations
//  Instruction_Bool_Equals,
//  Instruction_Bool_EqualsNot,
//  Instruction_Bool_More,
//  Instruction_Bool_Less,
//  Instruction_Bool_MoreEqual,
//  Instruction_Bool_LessEqual,
    //-----------------------
//  Instruction_Complex,
//  Instruction_IF, // Bool operations inside?
//  Instruction_WHILE,

} INSTRUCTION_TYPE;

typedef struct sINSTRUCTION{
    INSTRUCTION_TYPE type;
    void *address_dst;
    void *address_src1;
    void *address_src2;
} INSTRUCTION;

VARIABLE *createGlobalVariable();
tDLList *createGlobalFrame();
void pushToGlobalFrame(tDLList *frame);

/* ************************************************ EXECUTE ************************************************/
void executeInstructionAssign(INSTRUCTION *instr);
void executeInstructionMathOperation(INSTRUCTION *instr);
/* ************************************************ CREATE *************************************************/
/**
 * Create insturction Instruction_Push_Global_Variable.
 *
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *createPushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);
INSTRUCTION *createInstructionAssign(char *nameDst, char *nameSrc);
//TODO
INSTRUCTION *createInstructionMathOperation( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2);

#endif //IFJ_INSTRUCTION_H

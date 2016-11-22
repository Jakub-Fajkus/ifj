//
// Created by Andrej on 21.11.2016.
//

#ifndef IFJ_INSTRUCTION_H
#define IFJ_INSTRUCTION_H

#include "BasicStructures.h"
#include "DoubleLinkedList.h"
#include "Stack.h"

typedef enum {
    Instruction_Create_GlobalFrame_And_LocalStack = 10000,  // maybe the first instruction possible. NO PARAMS in DST, SRC1 or SRC2
    Instruction_Push_Global_Variable = 10001, // 3 params: variable name, variable type, variable value : best in this format
    Instruction_Create_Local_Frame = 10002, // IDEA: do we even need any param?
    Instruction_Push_Local_Variable = 10003, // 3 params as in global var?
    Instruction_CallFunction = 10004, // == Instruction_PushToStack_LocalFrame
    Instruction_ReturnFunction = 10005, // == Instruction_PopFromStack_LocalFrame
    Instruction_Jump = 10006,
    Instruction_Assign = 10007,
    //----------------------- Math operations
    Instruction_Addition = 10008, // Params: address_dst = address_src1 + address_src2;
    Instruction_Subtraction = 10009, // Params: address_dst = address_src1 - address_src2;
    Instruction_Multiply = 10010, // 3 params
    Instruction_Divide = 10011, // 3 params
    //-------------------------------- Built-in functions
    Instruction_Function_readInt = 10012, // 1 param (retvalue, type INT)
    Instruction_Function_readDouble = 10013, // 1 param (retvalue, type DOUBLE)
    Instruction_Function_readString = 10014, // 1 param (retvalue, type STRING)
    Instruction_Function_Print = 10015, // 1-n params, ANY TYPE
    Instruction_Function_Length = 10016, // 2 params (retvalue-int, param-string)
    Instruction_Function_Substr = 10017, // 4 params (retvalue-string, 3 params-string,int,int)
    Instruction_Function_Compare = 10018, // 3 params (retvalue-int, 2 params-string, string)
    Instruction_Function_Find = 10019, // 3 params (retvalue-int, 2 params-string,string)
    Instruction_Function_Sort =  10020// 2 params (retvalue-string, param-string)
    //-------------------------------- Work with frames
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


// Used for Global frame & Local frames
tDLList *createFrame();
void pushToFrame(tDLList *globalFrame, INSTRUCTION *instruction);
tStack *createFrameStack();
void pushFrameToStack(tStack *localFrameStack,tDLList *frame);

VARIABLE *findFrameVariable(tDLList *frame, char *name);
tDLList *getActualLocalFrame(tStack *stackOfLocalFrames);


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

/***
 *
 * Push local variable to interpret local frame
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *createPushLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);

INSTRUCTION *createInstructionAssign(char *nameDst, char *nameSrc);
//TODO
INSTRUCTION *createInstructionMathOperation( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2);

#endif //IFJ_INSTRUCTION_H

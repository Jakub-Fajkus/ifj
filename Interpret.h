//
// Created by Andrej on 19.11.2016.
//

#ifndef IFJ_INTERPRET_H
#define IFJ_INTERPRET_H

#include "BasicStructures.h";

typedef enum {
    Instruction_NOP,
    Instruction_Jump,
    Instruction_Assign,
//  Instruction_Begin,
//  Instruction_End,
    //----------------------- Math operations
    Instruction_Addition,
    Instruction_Substraction,
    Instruction_Multiply,
    Instruction_Divide,
    //----------------------- Bool operations
    Instruction_Bool_Equals,
    Instruction_Bool_EqualsNot,
    Instruction_Bool_More,
    Instruction_Bool_Less,
    Instruction_Bool_MoreEqual,
    Instruction_Bool_LessEqual,
    //-----------------------
//  Instruction_Complex,
//  Instruction_IF,
//  Instruction_WHILE,
    //----------------------- Calling user function
    Instruction_CallFunction,
    Instruction_ReturnFunction, //not sure
    //-------------------------------- Built-in functions
    Instruction_Function_readInt,
    Instruction_Function_readDouble,
    Instruction_Function_readString,
    Instruction_Function_Print,
    Instruction_Function_Length,
    Instruction_Function_Substr,
    Instruction_Function_Compare,
    Instruction_Function_Find,
    Instruction_Function_Sort,
    //-------------------------------- Work with frames
    Instruction_Create_Global_Frame,
    Instruction_Push_Global_Variable,
    Instruction_Create_Local_Frame,
    Instruction_Push_Local_Frame,
    Instruction_Push_Local_Variable

} INSTRUCTION_TYPE;


typedef struct sINSTRUCTION{
    INSTRUCTION_TYPE type;
    void *address_dst;
    void *address_src1;
    void *address_src2;
} INSTRUCTION;

void InstructionExecute(INSTRUCTION *);

#endif //IFJ_INTERPRET_H

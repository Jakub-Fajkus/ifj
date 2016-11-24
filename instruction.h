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
    Instruction_Create_Global_Variable = 10002,
    Instruction_Create_Local_Frame = 10003, // IDEA: do we even need any param?
    Instruction_Push_Local_Variable = 10004, // 3 params as in global var?
    Instruction_Create_Local_Variable = 10005,
    Instruction_CallFunction = 10006, // == Instruction_PushToStack_LocalFrame
    Instruction_ReturnFunction = 10007, // == Instruction_PopFromStack_LocalFrame
    Instruction_Jump = 10008,
    Instruction_Assign = 10009,
    //----------------------- Math operations
    Instruction_Addition = 10010, // Params: address_dst = address_src1 + address_src2;
    Instruction_Subtraction = 10011, // Params: dst=src1-src2;
    Instruction_Multiply = 10012, // Params: dst=src1*src2;
    Instruction_Divide = 10013, // Params: dst=src1/src2;
    //----------------------- Bool operations, params: dst=name, src1=name, src2=name
    Instruction_IF = 10014,
    Instruction_Bool_Equals = 10015,    // dst = (src1==src2) ? 1 : 0
    Instruction_Bool_EqualsNot = 10016, // dst = (src1!=src2) ? 1 : 0
    Instruction_Bool_More = 10017,      // dst = (src1>src2) ? 1 : 0
    Instruction_Bool_Less = 10018,      // dst = (src1<src2) ? 1 : 0
    Instruction_Bool_MoreEqual = 10019, // dst = (src1>=src2) ? 1 : 0
    Instruction_Bool_LessEqual = 10020, // dst = (src1<=src2) ? 1 : 0
    //-------------------------------- Built-in functions
    Instruction_Function_readInt = 10021, // 1 param (retvalue, type INT)
    Instruction_Function_readDouble = 10022, // 1 param (retvalue, type DOUBLE)
    Instruction_Function_readString = 10023, // 1 param (retvalue, type STRING)
    Instruction_Function_Print = 10024, // 1-n params, ANY TYPE
    Instruction_Function_Length = 10025, // 2 params (retvalue-int, param-string)
    Instruction_Function_Substr = 10026, // 4 params (retvalue-string, 3 params-string,int,int)
    Instruction_Function_Compare = 10027, // 3 params (retvalue-int, 2 params-string, string)
    Instruction_Function_Find = 10028, // 3 params (retvalue-int, 2 params-string,string)
    Instruction_Function_Sort =  10029, // 2 params (retvalue-string, param-string)
    //-----------------------
//  Instruction_WHILE

} INSTRUCTION_TYPE;

typedef struct sINSTRUCTION{
    INSTRUCTION_TYPE type;
    void *address_dst;
    void *address_src1;
    void *address_src2;
} INSTRUCTION;

/// Constructor of frame stack
/// \return Ptr to Stack
struct tStack_struct *createFrameStack();

/// Pushes Local frame on the top of the Stack
/// \param localFrameStack  - Ptr to frame stack
/// \param frame - upcoming frame, already full of variables
void pushFrameToStack(struct tStack_struct *localFrameStack,tDLList *frame);

/// Constructor of frame (used for global frame once, and for local ones)
/// \return Ptr to frame
tDLList *createFrame();

/// Fills frame with one variable
/// \param frame
/// \param instruction
void pushToFrame(tDLList *globalFrame, INSTRUCTION *instruction);

/// Finds variable with given name
/// \param frame - global frame or top local frame
/// \param name - given name
/// \return NULL if not found, otherwise pointer to the variable
VARIABLE *findFrameVariable(tDLList *frame, char *name);

/// Returns pointer to local frame on the top of localFrameStack
/// \param stackOfLocalFrames - Ptr to Stack
/// \return Ptr of actual local frame
tDLList *getActualLocalFrame(struct tStack_struct *stackOfLocalFrames);


/* ************************************************ CREATE *************************************************/

/**
 * INSTRUCTION CONSTRUCTOR: Create GLOBAL variable WITH value
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *pushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);

/**
 * INSTRUCTION CONSTRUCTOR: Create GLOBAL variable WITHOUT value
 * @param name
 * @param type
 * @return
 */
INSTRUCTION *createGlobalVariable(char *name, DATA_TYPE type);

/**
 * INSTRUCTION CONSTRUCTOR: Create LOCAL variable WITH value
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *pushLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);

/**
 * INSTRUCTION CONSTRUCTOR: Create LOCAL variable WITHOUT value
 * @param name
 * @param type
 * @return
 */
INSTRUCTION *createLocalVariable(char *name, DATA_TYPE type);

/**
 * INSTRUCTION CONSTRUCTOR: Assign
 * @param nameDst <- pointer to name of assigned variable
 * @param nameSrc <- pointer to name variable with value
 * @return
 */
INSTRUCTION *createInstructionAssign(char *nameDst, char *nameSrc);

/**
 * INSTRUCTION CONSTRUCTOR: Math Operation
 * @param instType
 *      Instruction_Addition: dst=src1+src2, CONCATENATES TWO STRINGS
 *      Instruction_Subtraction: dst=src1-src2
 *      Instruction_Multiply: dst=src1*src2
 *      Instruction_Divide: dst=src1/src2
 * @param nameDst
 * @param nameSrc1
 * @param nameSrc2
 * @return
 */
INSTRUCTION *createInstructionMathOperation( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2);

/**
 * INSTRUCTION CONSTRUCTOR: Bool expression evaluation
 * @param instType
 *      Instruction_Bool_Equals:    dst = (src1==src2) ? 1 : 0
 *      Instruction_Bool_EqualsNot: dst = (src1!=src2) ? 1 : 0
 *      Instruction_Bool_More:      dst = (src1>src2) ? 1 : 0
 *      Instruction_Bool_Less:      dst = (src1<src2) ? 1 : 0
 *      Instruction_Bool_MoreEqual: dst = (src1>=src2) ? 1 : 0
 *      Instruction_Bool_LessEqual: dst = (src1<=src2) ? 1 : 0
 * @param nameDst
 * @param nameSrc1
 * @param nameSrc2
 * @return
 */
INSTRUCTION *createInstructionExpressionEvaluation( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2 );

/**
 * INSTRUCTION CONSTRUCTOR: IF Statement
 * @param nameDst - Name of TEMPORARY VARIABLE with true/false value
 *                  value 1/true : Interpret jumps to instruction pointed by trueDst
 *                  value 0/false: ~ ~ ~ pointed by falseDst
 * @param trueDst - POINTER TO INSTRUCTION (IN THE SAME LIST)
 * @param falseDst - ~the same
 * @return
 */
INSTRUCTION *createInstructionIf(char *nameDst, INSTRUCTION *trueDst, INSTRUCTION *falseDst);

/**
 * INSTRUCTION CONSTRUCTOR: Create GlobalFrame and LocalStack
 * Due to dumb implementation of the interpret,
 * THIS NEEDS TO BE THE FIRST INSTRUCTION IN THE FIRST LIST
 * WARNING: DO NOT CALL THIS INSTRUCTION MORE THAN ONCE
 * IN WHOLE PROGRAM. Many thanks.
 * @return
 */
INSTRUCTION *createFirstInstruction();






LIST_ELEMENT createUpcomingInstruction(INSTRUCTION *instr);
void printInstructionTest(int x, tDLList *TestInstructionList);








#endif //IFJ_INSTRUCTION_H

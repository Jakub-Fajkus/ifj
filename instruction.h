//
// Created by Andrej on 21.11.2016.
//

#ifndef IFJ_INSTRUCTION_H
#define IFJ_INSTRUCTION_H

#include "basicstructures.h"
#include "doublelinkedlist.h"
#include "stack.h"

typedef enum {
    Instruction_Create_GlobalFrame_And_LocalStack = 10000,  // NO PARAMS in DST, SRC1 or SRC2. CALLED ONCE.
    Instruction_Push_Global_Variable = 10001, // 3 params: variable name, variable type, variable value : best in this format
    Instruction_Create_Global_Variable = 10002,
    Instruction_Create_Local_Frame = 10003, // IDEA: do we even need any param?
    Instruction_Push_Local_Variable = 10004, // 3 params as in global var?
    Instruction_Create_Local_Variable = 10005,
    Instruction_CallFunction = 10006, // == Instruction_PushToStack_LocalFrame
    Instruction_ReturnFunction = 10007, // == Instruction_PopFromStack_LocalFrame
    //----------------------- Math & assign operations
    Instruction_Assign = 10008,
    Instruction_Addition = 10009, // Params: address_dst = address_src1 + address_src2;
    Instruction_Subtraction = 10010, // Params: dst=src1-src2;
    Instruction_Multiply = 10011, // Params: dst=src1*src2;
    Instruction_Divide = 10012, // Params: dst=src1/src2;
    //----------------------- Bool operations, params: dst=name, src1=name, src2=name
    Instruction_IF = 10013,
    Instruction_WHILE = 10014,
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
    Instruction_Function_Print = 10024, // 1 param, ANY TYPE
    Instruction_Function_Length = 10025, // 2 params (retvalue-int, param-string)
    Instruction_Function_Substr = 10026, // 4 params (retvalue-string, 3 params-string,int,int)
    Instruction_Function_Compare = 10027, // 3 params (retvalue-int, 2 params-string, string)
    Instruction_Function_Find = 10028, // 3 params (retvalue-int, 2 params-string,string)
    Instruction_Function_Sort =  10029, // 2 params (retvalue-string, param-string)
    //-----------------------
    Instruction_End_Interpret = 10030,
    Instruction_Push_Actual_Local_Variable = 10031,
    Instruction_Create_Actual_Local_Variable = 10032,
    Instruction_Copy_From_Actual_To_Upcoming_Frame = 10034
} INSTRUCTION_TYPE;

typedef struct sINSTRUCTION{
    INSTRUCTION_TYPE type;
    void *address_dst;
    void *address_src1;
    void *address_src2;
} INSTRUCTION;

/// Constructor of frame stack
/// \return Ptr to Stack
struct STACK_STR *createFrameStack();

/// Pushes Local frame on the top of the Stack
/// \param localFrameStack  - Ptr to frame stack
/// \param frame - upcoming frame, already full of variables
void pushFrameToStack(struct STACK_STR *localFrameStack,tDLList *frame);

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
tDLList *getActualLocalFrame(struct STACK_STR *stackOfLocalFrames);

/* ************************************************** CREATE **********************************************************/
/* ******************************************* Variable constructors **************************************************/

/**
 * INSTRUCTION CONSTRUCTOR: Create GLOBAL variable WITH OR WITHOUT value
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *pushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);
INSTRUCTION *createGlobalVariable(char *name, DATA_TYPE type);

/**
 * INSTRUCTION CONSTRUCTOR: Create LOCAL variable WITH or WITHOUT value
 * WARNING: inserts into UPCOMING FRAME
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *pushLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);
INSTRUCTION *createLocalVariable(char *name, DATA_TYPE type);


/**
 * INSTRUCTION CONSTRUCTOR: Create LOCAL variable WITH or WITHOUT value
 * WARNING: inserts into ACTUAL FRAME - e.g. frame on the top of stack
 * @param name
 * @param type
 * @param value
 * @return
 */
INSTRUCTION *pushActualLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value);
INSTRUCTION *createActualLocalVariable(char *name, DATA_TYPE type);


/* ************************************************** CREATE **********************************************************/
/* ******************************************* Operations constructors ************************************************/


/**
 * INSTRUCTION CONSTRUCTOR: COPY value from global/actual-local frame
 * into variable in upcoming local frame
 * @param upcoming
 * @param actual
 * @return
 */
INSTRUCTION *createInstrCopyFromActualToUpcomingFrame (char *upcoming, char *actual);

/**
 * INSTRUCTION CONSTRUCTOR: Assign
 * @param nameDst <- pointer to name of assigned variable
 * @param nameSrc <- pointer to name variable with value
 * @return
 */
INSTRUCTION *createInstrAssign(char *nameDst, char *nameSrc);

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
INSTRUCTION *createInstrMath( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2);

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
INSTRUCTION *createInstrExprEval( INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2 );

/**
 * INSTRUCTION CONSTRUCTOR: IF Statement
 *
 * This function needs TWO INSTRUCTION LISTS and one TEMP VAR (that will contain evaluated expr)
 *
 * @param nameDst - Name of TEMPORARY VARIABLE with true/false value
 *                  value 1/true : Interpret jumps to instruction pointed by trueDst
 *                  value 0/false: ~ ~ ~ pointed by falseDst
 * @param trueDst - POINTER TO NEW LIST OF INSTRUCTIONS
 * @param falseDst - ~the same
 * @return
 */
INSTRUCTION *createInstrIf(char *boolVar, tDLList *trueDstList, tDLList *falseDstList);


/**
 * INSTRUCTION CONSTRUCTOR: Cycle WHILE
 *
 * Warning: involves ONE OR THREE MINIMUM RECURSION CALLS
 *
 * @param boolVar - NAME of global, local or temporary variable that will get value 0/1
 * @param exprInstrList - List of instructions (math&expr), the last assigns 0/1 to "boolVar"
 * @param cycleList - executes or not depending on value of boolVar
 * @return
 */
INSTRUCTION *createInstrWhile(char *boolVar, tDLList *exprInstrList, tDLList *cycleList);

/* ************************************************** CREATE **********************************************************/
/* **************************************** User Function constructors ************************************************/

/**
 * INSTRUCTION CONSTRUCTOR: Call Function
 *
 * @param functionInstrList Instructions of the function
 * @param functionToCall Pointer to the symbol table
 *
 * @return
 */
INSTRUCTION *createInstrCallFunction(tDLList *functionInstrList, struct SYMBOL_TABLE_FUNCTION_STR *functionToCall);

/**
 * INSTRUCTION CONSTRUCTOR: Return from Function
 *
 * @return
 */
INSTRUCTION *createInstrReturnFunction();


/**
 * INSTRUCTION CONSTRUCTOR: Let interpret know you are going to
 * fill upcoming local frame & call user function soon! (even for main.run)
 * @return
 */
INSTRUCTION *createInstrFillLocalFrame();


/* ************************************************** CREATE **********************************************************/
/* ******************************************* Special instructions ***************************************************/

/**
 * INSTRUCTION CONSTRUCTOR: Create GlobalFrame and LocalStack
 * Due to dumb implementation of the interpret,
 * THIS NEEDS TO BE THE FIRST INSTRUCTION IN THE FIRST LIST
 * WARNING: DO NOT CALL THIS INSTRUCTION MORE THAN ONCE
 * IN WHOLE PROGRAM. Many thanks.
 * @return
 */
INSTRUCTION *createFirstInstruction();

/**
 * INSTRUCTION CONSTRUCTOR: End Interpret.
 * Signal that the last instruction tape has been finished.
 * Performs freeing dynamically allocated space.
 * @return
 */
INSTRUCTION *createLastInstruction();



/* ************************************************** CREATE **********************************************************/
/* ********************************************* Built in functions ***************************************************/

/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction Read[type]
 *
 * @param instrType
 *
 *      Instruction_Function_readInt
 *      Instruction_Function_readDouble
 *      Instruction_Function_readString
 *
 * @return
 */
INSTRUCTION *createInstructionRead(INSTRUCTION_TYPE instrType, char *nameDst);


/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction Print
 *
 * @param nameDst - name of variable, prints it's value
 * @return
 */
INSTRUCTION *createInstrPrint(char *nameDst);


/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction int Length(char*);
 * @param retValue - int
 * @param param    - string
 * @return
 */
INSTRUCTION *createInstrBFLength(char *retValue, char *param);


/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction int Compare(char*, char*);
 * @param retValue  - int
 * @param param1    - string
 * @param param2    - string
 * @return
 */
INSTRUCTION *createInstrBFCompare(char *retValue, char *param1, char *param2);

/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction int Find(char*, char*);
 * @param retValue  - int
 * @param param1    - string
 * @param param2    - string
 * @return
 */
INSTRUCTION *createInstrBFFind(char *retValue, char *param1, char *param2);


/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction int Length(char*);
 * @param retValue  - string
 * @param param     - string
 * @return
 */
INSTRUCTION *createInstrBFSort(char *retValue, char *param);

/**
 * INSTRUCTION CONSTRUCTOR: Built in instruction int substr(char*);
 * @param retValue  - string
 * @param params    - tDLList
 * @return
 */
INSTRUCTION *createInstrBFSubstr(char *retValue, tDLList *params);

// ------------------------------------------------------ wrapper, seems like unused...

/**
 * CONSTRUCTOR FOR SINGLE INSTRUCTION AS A LIST ELEMENT
 * RETURN VALUE CAN BE USED FOR INSERTION TO THE
 * @param instr
 * @return
 */
LIST_ELEMENT createInstruction(INSTRUCTION *instr);


// to be deleted
void printInstructionTest(int x, tDLList *TestInstructionList);

#endif //IFJ_INSTRUCTION_H

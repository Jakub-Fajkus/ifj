//
// Created by Andrej on 21.11.2016.
//

#include "instruction.h"
#include "Interpret.h"
#include "BasicStructures.h"
#include "Debug.h"

/* ************************************************ USED BY INTERPRET *************************************************/
/* ************************************************ DATA CONSTRUCTORS *************************************************/

/// Constructor of frame stack
/// \return Ptr to Stack
struct STACK_STR *createFrameStack() {
    struct STACK_STR *localFrameStack = malloc(sizeof(struct STACK_STR));
    // WARNING: also allocates memory for the array inside!
    stackInit(localFrameStack);
    localFrameStack->arr->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    return localFrameStack;
}

/// Pushes Local frame on the top of the Stack
/// \param localFrameStack - Ptr to frame stack
/// \param frame - upcoming frame, already full of variables
void pushFrameToStack(struct STACK_STR *localFrameStack, tDLList *frame) {
    STACK_ELEMENT *newLocalFrame = malloc(sizeof(STACK_ELEMENT));

    newLocalFrame->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    newLocalFrame->data.localFrame = frame;

    stackPush(localFrameStack, *newLocalFrame);
}

/// Constructor of frame (used for global frame once, and for local ones)
/// \return Ptr to frame
tDLList *createFrame() {
    tDLList *frame = malloc(sizeof(tDLList));
    ListInit(frame);
    return frame;
}

/// Fills frame with one variable
/// \param frame
/// \param instruction
void pushToFrame(tDLList *frame, INSTRUCTION *instruction){
    LIST_ELEMENT *listElem = malloc(sizeof(LIST_ELEMENT));
    VARIABLE *var = malloc(sizeof(VARIABLE));

    listElem->type = LIST_ELEMENT_TYPE_FRAME_ELEMENT;
    listElem->data.variable = var;

    var->name = (char *)instruction->address_dst;
    var->type = *(DATA_TYPE*)instruction->address_src1;

    if (instruction->address_src2 != NULL)  // Instruction is inserting VAR with value
        var->value = *(VARIABLE_VALUE*)instruction->address_src2;

    /*DEBUG PRINT
    if (instruction->address_src2 == NULL)
        debugPrintf("Successfully inserted this variable: %s, %d\n", listElem->data.variable->name, listElem->data.variable->type);
    else {
        if (var->type == TYPE_INT) debugPrintf("Successfully inserted this variable: %s, %d %d\n", listElem->data.variable->name, listElem->data.variable->type, listElem->data.variable->value.intValue);
        if (var->type == TYPE_DOUBLE) debugPrintf("Successfully inserted this variable: %s, %d %g\n", listElem->data.variable->name, listElem->data.variable->type, listElem->data.variable->value.doubleValue);
        if (var->type == TYPE_STRING) debugPrintf("Successfully inserted this variable: %s, %d %s\n", listElem->data.variable->name, listElem->data.variable->type, listElem->data.variable->value.stringValue);
    }
    */
    ListInsertLast(frame, *listElem);
}

/// Finds variable with given name
/// \param frame - global frame or top local frame
/// \param name - given name
/// \return NULL if not found, otherwise pointer to the variable
VARIABLE *findFrameVariable(tDLList *frame, char *name) {

    //debugPrintf("i am seeking for: %s\t", name);

    if ( name == NULL ) return NULL; // error handling
    int compare;
    ListFirst(frame);
    do { // Search for the variable in the globalFrame
        compare = strcmp(name, frame->Act->element.data.variable->name);
        if ( compare == 0 ) {   // found the right variable
            //debugPrintf("i have found: %s\n", frame->Act->element.data.variable->name);
            return frame->Act->element.data.variable;
        }
        ListSuccessor(frame);
    } while ( frame->Act != NULL );
    return NULL;
}

/// Returns pointer to local frame on the top of localFrameStack
/// \param stackOfLocalFrames - Ptr to Stack
/// \return Ptr of actual local frame
tDLList *getActualLocalFrame(struct STACK_STR *stackOfLocalFrames) {

    tDLList *actualLocalFrame = NULL;

    if (!stackEmpty(stackOfLocalFrames)) {
        STACK_ELEMENT* stackElement = malloc(sizeof(STACK_ELEMENT));
        stackTop(stackOfLocalFrames, stackElement);
        actualLocalFrame = stackElement->data.localFrame;
    }

    return actualLocalFrame;
}


/* ************************************************ USED BY PARSER ****************************************************/
/* ******************************************* INSTRUCTION CONSTRUCTORS  **********************************************/


INSTRUCTION *pushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value) {
    // malloc?
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Push_Global_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    instruction->address_src2 = malloc(sizeof(VARIABLE_VALUE));
    *(VARIABLE_VALUE*)instruction->address_src2 = value;

    return instruction;
}


INSTRUCTION *createGlobalVariable(char *name, DATA_TYPE type) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Create_Global_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    // DOES NOT malloc mem for this
    instruction->address_src2 = NULL;
    return instruction;
}


INSTRUCTION *pushLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Push_Local_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    instruction->address_src2 = malloc(sizeof(VARIABLE_VALUE));
    *(VARIABLE_VALUE*)instruction->address_src2 = value;

    return instruction;
}


INSTRUCTION *createLocalVariable(char *name, DATA_TYPE type) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Create_Local_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    // DOES NOT malloc mem for this
    instruction->address_src2 = NULL;
    return instruction;
}


INSTRUCTION *pushActualLocalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value){
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Push_Actual_Local_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    instruction->address_src2 = malloc(sizeof(VARIABLE_VALUE));
    *(VARIABLE_VALUE*)instruction->address_src2 = value;

    return instruction;
}


INSTRUCTION *createActualLocalVariable(char *name, DATA_TYPE type) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Create_Actual_Local_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;

    instruction->address_src2 = NULL;
    return instruction;
}


INSTRUCTION *createInstrCopyToUpcomingFrame (char *upcomingFrameVar, char *actualFrameVar){
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Copy_To_Upcoming_Frame;
    instruction->address_dst = upcomingFrameVar;
    instruction->address_src1 = actualFrameVar;
    instruction->address_src2 = NULL;

    return instruction;
}


INSTRUCTION *createInstrAssign(char *nameDst, char *nameSrc) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Assign;
    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc;
    instruction->address_src2 = NULL;

    return instruction;
}


INSTRUCTION *createInstrMath(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = instType;

    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc1;
    instruction->address_src2 = nameSrc2;

    return instruction;
}


INSTRUCTION *createInstrExprEval(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = instType;

    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc1;
    instruction->address_src2 = nameSrc2;

    return instruction;
}


INSTRUCTION *createInstrIf(char *boolVar, tDLList *trueDstList, tDLList *falseDstList) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_IF;
    instruction->address_dst = boolVar;
    instruction->address_src1 = trueDstList;
    instruction->address_src2 = falseDstList;

    return instruction;
}


INSTRUCTION *createInstrWhile(char *boolVar, tDLList *exprInstrList, tDLList *cycleList) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_WHILE;
    instruction->address_dst = boolVar;
    instruction->address_src1 = exprInstrList;
    instruction->address_src2 = cycleList;

    return instruction;
}


INSTRUCTION *createInstrCallFunction(tDLList *functionInstrList) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_CallFunction;

    instruction->address_dst = functionInstrList;
    instruction->address_src1 = NULL;
    instruction->address_src2 = NULL;

    return instruction;
}

INSTRUCTION *createInstrReturnFunction(char *returnVariableName) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_ReturnFunction;

    instruction->address_dst = returnVariableName;
    instruction->address_src1 = NULL;
    instruction->address_src2 = NULL;

    return instruction;
}


INSTRUCTION *createInstrFillLocalFrame() {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Create_Local_Frame;
    instruction->address_dst = NULL;
    instruction->address_src1 = NULL;
    instruction->address_src2 =  NULL;

    return instruction;
}


INSTRUCTION *createFirstInstruction() {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Create_GlobalFrame_And_LocalStack;
    instruction->address_dst = NULL;
    instruction->address_src1 = NULL;
    instruction->address_src2 =  NULL;

    return instruction;
}


INSTRUCTION *createLastInstruction() {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_End_Interpret;
    instruction->address_dst = NULL;
    instruction->address_src1 = NULL;
    instruction->address_src2 =  NULL;

    return instruction;
}


LIST_ELEMENT createInstruction(INSTRUCTION *instruction){
    LIST_ELEMENT listElement;
    listElement.type = LIST_ELEMENT_TYPE_INSTRUCTION;
    listElement.data.instr = instruction;
    return listElement;
}

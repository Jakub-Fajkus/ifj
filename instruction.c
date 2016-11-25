//
// Created by Andrej on 21.11.2016.
//

#include "instruction.h"
#include "Interpret.h"
#include "BasicStructures.h"

/* ************************************************ USED BY INTERPRET *************************************************/
/* ************************************************ EXECUTE           *************************************************/

/// Constructor of frame stack
/// \return Ptr to Stack
struct tStack_struct *createFrameStack() {
    tStack *localFrameStack = malloc(sizeof(tStack));
    // WARNING: also mallocs those arrays
    stackInit(localFrameStack);
    localFrameStack->arr->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    printf("Creating empty stack of frames... ");
    return localFrameStack;
}

/// Pushes Local frame on the top of the Stack
/// \param localFrameStack - Ptr to frame stack
/// \param frame - upcoming frame, already full of variables
void pushFrameToStack(tStack *localFrameStack, tDLList *frame) {
    STACK_ELEMENT *newLocalFrame = malloc(sizeof(STACK_ELEMENT));

    newLocalFrame->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    newLocalFrame->data.localFrame = frame;

    stackPush(localFrameStack, *newLocalFrame);
}

/// Constructor of frame (used for global frame once, and for local ones)
/// \return Ptr to frame
tDLList *createFrame() {
    tDLList *frame = malloc(sizeof(tDLList));
    checkMalloc(frame);
    ListInit(frame);
    printf("::::: Successfully created frame, type of:");
    return frame;
}

/// Fills frame with one variable
/// \param frame
/// \param instruction
void pushToFrame(tDLList *frame, INSTRUCTION *instruction){
    LIST_ELEMENT variable;

    variable.type = LIST_ELEMENT_TYPE_FRAME_ELEMENT;
    variable.data.variable->name = (char *)instruction->address_dst;
    variable.data.variable->type = (DATA_TYPE)instruction->address_src1;
    //TODO: what if using only "create variable", without value? ~src2 is null
    variable.data.variable->value = *(VARIABLE_VALUE*)instruction->address_src2;

    printf("Successfully inserted this variable: %s, %d", variable.data.variable->name, variable.data.variable->type);
    ListInsertLast(frame, variable);
}

/// Finds variable with given name
/// \param frame - global frame or top local frame
/// \param name - given name
/// \return NULL if not found, otherwise pointer to the variable
VARIABLE *findFrameVariable(tDLList *frame, char *name) {

    if ( name == NULL ) return NULL; // error handling
    int compare;
    ListFirst(frame);
    do { // Search for the variable in the globalFrame
        compare = strcmp(name, frame->Act->element.data.variable->name);
        if ( compare == 0 ) {   // found the right variable
            return frame->Act->element.data.variable;
        }
        ListSuccessor(frame);
    } while ( frame->Act != frame->Last );
    return NULL;
}

/// Returns pointer to local frame on the top of localFrameStack
/// \param stackOfLocalFrames - Ptr to Stack
/// \return Ptr of actual local frame
tDLList *getActualLocalFrame(struct STACK_STR *stackOfLocalFrames) {

    STACK_ELEMENT* stackElement = malloc(sizeof(STACK_ELEMENT));
    //TODO: malloc
    stackTop(stackOfLocalFrames, stackElement);
    tDLList *actualLocalFrame = stackElement->data.localFrame;
    return actualLocalFrame;
}


/* ************************************************ USED BY PARSER ****************************************************/
/* ************************************************ CREATE         ****************************************************/


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


INSTRUCTION *createInstructionAssign(char *nameDst, char *nameSrc) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Assign;
    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc;
    instruction->address_src2 = NULL;

    return instruction;
}


INSTRUCTION *createInstructionMathOperation(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = instType;

    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc1;
    instruction->address_src2 = nameSrc2;

    return instruction;
}


INSTRUCTION *createInstructionExpressionEvaluation(INSTRUCTION_TYPE instType, char *nameDst, char *nameSrc1, char *nameSrc2) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = instType;

    instruction->address_dst = nameDst;
    instruction->address_src1 = nameSrc1;
    instruction->address_src2 = nameSrc2;

    return instruction;
}

INSTRUCTION *createInstructionIf(char *nameDst, INSTRUCTION *trueDst, INSTRUCTION *falseDst) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_IF;
    instruction->address_dst = nameDst;
    *(INSTRUCTION*)instruction->address_src1 = *trueDst;
    *(INSTRUCTION*)instruction->address_src2 = *falseDst;

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

LIST_ELEMENT createInstruction(INSTRUCTION *instruction){
    LIST_ELEMENT listElement;
    listElement.type = LIST_ELEMENT_TYPE_INSTRUCTION;
    listElement.data.instr = instruction;
    return listElement;
}

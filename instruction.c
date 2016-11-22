//
// Created by Andrej on 21.11.2016.
//

#include "instruction.h"
#include "Interpret.h"
//#include "BasicStructures.h"



/* ************************************************ USED BY INTERPRET *************************************************/
/* ************************************************ EXECUTE           *************************************************/

tStack *createFrameStack() {
    tStack *localFrameStack = malloc(sizeof(tStack));
    //TODO: check malloc
    // WARNING: also mallocs those arrays
    stackInit(localFrameStack);
    localFrameStack->arr->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    return localFrameStack;
}

void pushFrameToStack(tStack *localFrameStack,tDLList *frame) {
    STACK_ELEMENT *newLocalFrame = malloc(sizeof(STACK_ELEMENT));

    newLocalFrame->type = STACK_ELEMENT_TYPE_LOCAL_FRAME;
    newLocalFrame->data.localFrame = frame;

    stackPush(localFrameStack, *newLocalFrame);
}


tDLList *createFrame() {
    tDLList *frame = malloc(sizeof(tDLList));
    checkMalloc(frame);
    ListInit(frame);
    return frame;
}

void pushToFrame(tDLList *frame, INSTRUCTION *instruction){
    LIST_ELEMENT variable;

    variable.type = LIST_ELEMENT_TYPE_FRAME_ELEMENT;
    variable.data.variable->name = (char *)instruction->address_dst;
    variable.data.variable->type = (DATA_TYPE)instruction->address_src1;
    variable.data.variable->value = *(VARIABLE_VALUE*)instruction->address_src2;

    ListInsertLast(frame, variable);
}

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

tDLList *getActualLocalFrame(tStack *stackOfLocalFrames) {

    STACK_ELEMENT* stackElement = malloc(sizeof(STACK_ELEMENT));
    // ošetruj malloc
    stackTop(stackOfLocalFrames, stackElement);
    tDLList *actualLocalFrame = stackElement->data.localFrame;
    return actualLocalFrame;

}


/* ************************************************ USED BY PARSER ****************************************************/
/* ************************************************ CREATE         ****************************************************/

INSTRUCTION *createPushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));
    checkMalloc(instruction); // delete this if you think it is not necessary

    instruction->type = Instruction_Push_Global_Variable;
    instruction->address_dst = name;

    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    checkMalloc(instruction->address_src1); // delete this if you think it is not necessary
    *(int*)instruction->address_src1 = type;

    instruction->address_src2 = malloc(sizeof(VARIABLE_VALUE));
    checkMalloc(instruction->address_src2); // delete this if you think it is not necessary
    *(VARIABLE_VALUE*)instruction->address_src2 = value;

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
    //todo
    return NULL;
}
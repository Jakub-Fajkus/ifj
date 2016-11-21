//
// Created by Andrej on 21.11.2016.
//

#include "Interpret.h"
#include "instruction.h"

//* ************************************************ USED BY INTERPRET *************************************************

FRAME_ELEMENT *createGlobalVariable() {

}

tDLList *createGlobalFrame() {

    tDLList *frame = malloc(sizeof(tDLList));
    checkMalloc(frame);
    DLInitList(frame);

    return frame;
}

void pushToGlobalFrame(tDLList *frame){



}


//* ************************************************ USED BY PARSER ****************************************************


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

//
// Created by Andrej on 21.11.2016.
//

#ifndef IFJ_INSTRUCTION_H
#define IFJ_INSTRUCTION_H

FRAME_ELEMENT *createGlobalVariable();
tDLList *createGlobalFrame();
void pushToGlobalFrame(tDLList *frame);




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

//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"

void callInterpret() {

    printf("----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
    printf("----- -----   Interpret test  ----- ----- ----- ----- -----\n");
    printf("----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n\n");


    tDLList *TestInstructionList = malloc(sizeof(tDLList));
    ListInit(TestInstructionList);

    printf("----- Test 0: Inserting first instruction.\n");

    // Inserting instruction "Create Global Frame & Local Stack of Frames"
    ListInsertLast(TestInstructionList, createUpcomingInstruction(createFirstInstruction()));

    ListFirst(TestInstructionList);
    printInstructionTest(1,TestInstructionList);


    // Inserting instruction "Create Global Variable Without Value"
    ListInsertLast(TestInstructionList, createUpcomingInstruction(createGlobalVariable("Main.x", TYPE_INT)));
    ListSuccessor(TestInstructionList);
    printInstructionTest(2,TestInstructionList);
}


LIST_ELEMENT createUpcomingInstruction(INSTRUCTION *instr) {
    LIST_ELEMENT newElement;

    newElement.type = LIST_ELEMENT_TYPE_INSTRUCTION;
    newElement.data.instr = instr;

    return newElement;
}

void printInstructionTest(int x, tDLList *TestInstructionList) {

    printf("\n----- Test %d: Looking into instruction.\n",x);
    printf("TYPE: %d\n", TestInstructionList->Act->element.data.instr->type);
    printf("Ptr DstAddr: %p\n", TestInstructionList->Act->element.data.instr->address_dst);
    printf("Ptr Src1 Addr: %p\n", TestInstructionList->Act->element.data.instr->address_src1);
    printf("Ptr Src2 Addr: %p\n", TestInstructionList->Act->element.data.instr->address_src2);
    printf("----- TEST %d OK ----- ----- ----- ----- ----- ----- ----- -----\n",x);

}
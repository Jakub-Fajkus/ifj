//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"
#include "BasicStructures.h"

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
    //printInstructionTest(1,TestInstructionList);


    // Inserting instruction "Create Global Variable Without Value"
    ListInsertLast(TestInstructionList, createUpcomingInstruction(createGlobalVariable("Main.x", TYPE_INT)));
    //printf("-------------------------- fuck you guys --------------------");
    //printf("\n----- %s", (char *)TestInstructionList->Last->element.data.instr->address_dst);
    //printf("\n----- %d", *(DATA_TYPE*)TestInstructionList->Last->element.data.instr->address_src1);


    ListSuccessor(TestInstructionList);
    //printInstructionTest(2,TestInstructionList);

    VARIABLE_VALUE insertValue;
    insertValue.doubleValue = 4.2;
    ListInsertLast(TestInstructionList, createUpcomingInstruction(pushGlobalVariable("Main.a", TYPE_DOUBLE, insertValue)));



    ListSuccessor(TestInstructionList);
    //printInstructionTest(3,TestInstructionList);

    printf("-------------------------- fuck you guys --------------------");
    printf("\n----- %s", (char *)TestInstructionList->Last->element.data.instr->address_dst);
    printf("\n----- %d", *(DATA_TYPE*)TestInstructionList->Last->element.data.instr->address_src1);
    printf("\n----- %g\n", *(VARIABLE_VALUE*)TestInstructionList->Last->element.data.instr->address_src2);


    printf("\n----- LAST TEST: Print All Instructions.\n");
    printListOfInstructions(TestInstructionList);


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

void printListOfInstructions(tDLList *TestInstructionList) {

    int number=1;
    ListFirst(TestInstructionList);
    do {
        printf("Instruction number %d: %d\n", number, TestInstructionList->Act->element.data.instr->type);
        ListSuccessor(TestInstructionList);
    } while( TestInstructionList->Act != NULL );


}
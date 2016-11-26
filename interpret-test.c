//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"
#include "BasicStructures.h"

void callInterpret() {

    printf("----- -----   Interpret test  ----- ----- ----- ----- -----\n");

    VARIABLE_VALUE insertValue;
    tDLList *TestInstructionList = malloc(sizeof(tDLList));
    ListInit(TestInstructionList);

    // Inserting instruction "Create Global Frame & Local Stack of Frames"
    InsertFirst(TestInstructionList, createInstruction(createFirstInstruction()));

    // Inserting instruction "Create Global Variable Without Value"
    ListInsertLast(TestInstructionList, createInstruction(createGlobalVariable("Main.x", TYPE_DOUBLE)));

    // Inserting instruction "Create Global Variable with value"
    insertValue.doubleValue = 4.2;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.a", TYPE_DOUBLE, insertValue)));

    insertValue.doubleValue = 0.5;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.dest", TYPE_DOUBLE, insertValue)));

    insertValue.intValue = 99;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.cele", TYPE_INT, insertValue)));

    // Inserting instruction Assign
    ListInsertLast(TestInstructionList, createInstruction((createInstrAssign("Main.dest", "Main.cele"))));


    printf("\n----- Print All Instructions.\n");
    printListOfInstructions(TestInstructionList);

    //------ HERE COMES THE HELL
    printf("----- Drzte si kabaty, volame interpret!!!\n");
    Interpret(TestInstructionList, NULL, NULL);

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
        number++;
    } while( TestInstructionList->Act != NULL );


}
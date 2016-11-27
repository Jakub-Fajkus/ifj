//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"
#include "BasicStructures.h"

int callInterpret() {

    printf("----- -----   Interpret test  ----- ----- ----- ----- -----\n");

    VARIABLE_VALUE insertValue;
    tDLList *TestInstructionList = malloc(sizeof(tDLList));
    ListInit(TestInstructionList);

    // Inserting instruction "Create Global Frame & Local Stack of Frames"
    InsertFirst(TestInstructionList, createInstruction(createFirstInstruction()));

    insertValue.doubleValue = 1.0;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.x", TYPE_DOUBLE, insertValue)));
    insertValue.doubleValue = 0.1;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.dbl", TYPE_DOUBLE, insertValue)));
    insertValue.doubleValue = 2.0;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("#tmp1", TYPE_DOUBLE, insertValue)));

    ListInsertLast(TestInstructionList, createInstruction(createGlobalVariable("#bool1", TYPE_INT)));


    tDLList *exprList = malloc(sizeof(tDLList));
    ListInit(exprList);
    ListInsertLast(exprList, createInstruction(createInstrMath(Instruction_Bool_Less, "#bool1", "Main.x", "#tmp1")));


    tDLList *cycleList = malloc(sizeof(tDLList));
    ListInit(cycleList);
    ListInsertLast(cycleList, createInstruction(createInstrMath(Instruction_Addition, "Main.x", "Main.x", "Main.dbl")));

    ListInsertLast(TestInstructionList, createInstruction(createInstrWhile("#bool1", exprList, cycleList)));

    // Inserting last instruction
    ListInsertLast(TestInstructionList, createInstruction(createLastInstruction()));





    printf("\n----- Print All Instructions.\n");
    printListOfInstructions(TestInstructionList);

    //------ HERE COMES THE HELL
    printf("----- Drzte si kabaty, volame interpret!!!\n");
    return Interpret(TestInstructionList, NULL, NULL);
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
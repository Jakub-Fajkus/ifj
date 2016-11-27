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

    insertValue.intValue = 42;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("x", TYPE_INT, insertValue)));

    insertValue.doubleValue = 0.01;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("y", TYPE_DOUBLE, insertValue)));

    ListInsertLast(TestInstructionList, createInstruction(createGlobalVariable("vysledok", TYPE_INT)));

    ListInsertLast(TestInstructionList, createInstruction(createInstrMath(Instruction_Divide, "vysledok", "x", "y")));

    ListInsertLast(TestInstructionList, createInstruction(createInstrMath(Instruction_Divide, "vysledok", "vysledok", "x")));
    ListInsertLast(TestInstructionList, createInstruction(createInstrMath(Instruction_Addition, "vysledok", "vysledok", "y")));

    ListInsertLast(TestInstructionList, createInstruction(createGlobalVariable("vysledok2", TYPE_DOUBLE)));

    insertValue.intValue = 42;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("mulx", TYPE_INT, insertValue)));

    ListInsertLast(TestInstructionList, createInstruction(createInstrMath(Instruction_Multiply, "vysledok2", "vysledok", "mulx")));

    insertValue.doubleValue = 4000.42;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("subx", TYPE_DOUBLE, insertValue)));

    ListInsertLast(TestInstructionList, createInstruction(createInstrMath(Instruction_Subtraction, "vysledok2", "vysledok2", "subx")));


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
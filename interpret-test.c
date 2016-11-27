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

    insertValue.intValue = 4;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.x", TYPE_INT, insertValue)));
    insertValue.intValue = 0;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.bool", TYPE_INT, insertValue)));

    insertValue.doubleValue = 10.5;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.dbl", TYPE_DOUBLE, insertValue)));

    tDLList *TrueIfList = malloc(sizeof(tDLList));
    ListInit(TrueIfList);
    ListInsertLast(TrueIfList, createInstruction(createInstrMath(Instruction_Multiply, "Main.dbl", "Main.dbl", "Main.x")));

    tDLList *FalseIfList = malloc(sizeof(tDLList));
    ListInit(FalseIfList);
    ListInsertLast(FalseIfList, createInstruction(createInstrMath(Instruction_Subtraction, "Main.dbl", "Main.dbl", "Main.x")));

    ListInsertLast(TestInstructionList, createInstruction(createInstrIf("Main.bool", TrueIfList, FalseIfList)));


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
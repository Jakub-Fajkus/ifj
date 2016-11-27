//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"
#include "Debug.h"
#include "BasicStructures.h"

int callInterpret() {

    debugPrintf("----- -----   Interpret test  ----- ----- ----- ----- -----\n");

    VARIABLE_VALUE insertValue;
    tDLList *TestInstructionList = malloc(sizeof(tDLList));
    ListInit(TestInstructionList);

    // Inserting instruction "Create Global Frame & Local Stack of Frames"
    InsertFirst(TestInstructionList, createInstruction(createFirstInstruction()));
    // Inserting instruction "Prepare to fill upcoming local frame"
    ListInsertLast(TestInstructionList, createInstruction(createInstrFillLocalFrame()));
    // Fill upcoming frame with variables
    ListInsertLast(TestInstructionList, createInstruction(createLocalVariable("a",TYPE_INT)));
    insertValue.intValue = 3;
    ListInsertLast(TestInstructionList, createInstruction(pushLocalVariable("#tmp1",TYPE_INT, insertValue)));
    insertValue.intValue = 5;
    ListInsertLast(TestInstructionList, createInstruction(pushLocalVariable("#tmp2",TYPE_INT, insertValue)));

    // creating new function list
    tDLList *functionList = malloc(sizeof(tDLList));
    ListInit(functionList);
    ListInsertLast(functionList, createInstruction(createInstrMath(Instruction_Addition, "a","#tmp1","#tmp2")));

    // Creating calling function
    ListInsertLast(TestInstructionList, createInstruction(createInstrCallFunction(NULL, functionList)));

    // Inserting last instruction
    ListInsertLast(TestInstructionList, createInstruction(createLastInstruction()));





    debugPrintf("\n----- Print All Instructions.\n");
    printListOfInstructions(TestInstructionList);

    //------ HERE COMES THE HELL
    debugPrintf("----- Drzte si kabaty, volame interpret!!!\n");
    return Interpret(TestInstructionList, NULL, NULL);
}


void printInstructionTest(int x, tDLList *TestInstructionList) {

    debugPrintf("\n----- Test %d: Looking into instruction.\n",x);
    debugPrintf("TYPE: %d\n", TestInstructionList->Act->element.data.instr->type);
    debugPrintf("Ptr DstAddr: %p\n", TestInstructionList->Act->element.data.instr->address_dst);
    debugPrintf("Ptr Src1 Addr: %p\n", TestInstructionList->Act->element.data.instr->address_src1);
    debugPrintf("Ptr Src2 Addr: %p\n", TestInstructionList->Act->element.data.instr->address_src2);
    debugPrintf("----- TEST %d OK ----- ----- ----- ----- ----- ----- ----- -----\n",x);

}

void printListOfInstructions(tDLList *TestInstructionList) {

    int number=1;
    ListFirst(TestInstructionList);
    do {
        debugPrintf("Instruction number %d: %d\n", number, TestInstructionList->Act->element.data.instr->type);
        ListSuccessor(TestInstructionList);
        number++;
    } while( TestInstructionList->Act != NULL );


}
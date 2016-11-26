//
// Created by Andrej on 24.11.2016.
//

#include "Interpret.h"
#include "BasicStructures.h"

void callInterpret() {

    printf("----- -----   Interpret test  ----- ----- ----- ----- -----\n");

    tDLList *TestInstructionList = malloc(sizeof(tDLList));
    ListInit(TestInstructionList);
    // Inserting instruction "Create Global Frame & Local Stack of Frames"
    InsertFirst(TestInstructionList, createInstruction(createFirstInstruction()));
    ListFirst(TestInstructionList);
    //printInstructionTest(1,TestInstructionList);
    // Inserting instruction "Create Global Variable Without Value"
    ListInsertLast(TestInstructionList, createInstruction(createGlobalVariable("Main.x", TYPE_DOUBLE)));
    ListSuccessor(TestInstructionList);
    //printInstructionTest(2,TestInstructionList);

    VARIABLE_VALUE insertValue;
    insertValue.doubleValue = 4.2;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.a", TYPE_DOUBLE, insertValue)));
    ListSuccessor(TestInstructionList);
    //printInstructionTest(3,TestInstructionList);

    //printf("-------------------------- fuck you guys --------------------");
    //printf("\n----- %s", (char *)TestInstructionList->Last->element.data.instr->address_dst);
    //printf("\n----- %d", *(DATA_TYPE*)TestInstructionList->Last->element.data.instr->address_src1);
    //printf("\n----- %g\n", *(VARIABLE_VALUE*)TestInstructionList->Last->element.data.instr->address_src2);


    insertValue.doubleValue = 0.5;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.dest", TYPE_DOUBLE, insertValue)));

    insertValue.intValue = 99;
    ListInsertLast(TestInstructionList, createInstruction(pushGlobalVariable("Main.cele", TYPE_INT, insertValue)));

    ListInsertLast(TestInstructionList, createInstruction((createInstrAssign("Main.dest", "Main.cele"))));
    //printf("-------------------------- fuck you guys --------------------");
    //printf("\n----- %s", (char *)TestInstructionList->Last->element.data.instr->address_dst);
    //printf("\n----- %s", (char *)TestInstructionList->Last->element.data.instr->address_src1);
    //printf("\n----- %p\n", TestInstructionList->Last->element.data.instr->address_src2);


    printf("\n----- Print All Instructions.\n");
    printListOfInstructions(TestInstructionList);


    //--------------------------------------------------------------------------------------------------------------
    //------ HERE COMES THE HELL

    printf("----- Drzte si kabaty, volame interpret!!!\n");
    printf("Calling Instructions: \n");
    printf("\tInstruction_Create_GlobalFrame_And_LocalStack\tZERO PARAMS\n");
    printf("\tInstruction_Create_Global_Variable\t Name:Main.x, Type:double\n");
    printf("\tInstruction_Push_Global_Variable\t Name:Main.a, Type:double, Val: 4.2\n");
    printf("\tInstruction_Push_Global_Variable\t Name:Main.dest, Type:double, Val: 0.5\n");
    printf("\tInstruction_Push_Global_Variable\t Name:Main.cele, Type:int, Val: 99\n");
    printf("\tInstruction_Assign\t Main.dest = Main.cele\n");

    Interpret(TestInstructionList, NULL, NULL);

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
        number++;
    } while( TestInstructionList->Act != NULL );


}
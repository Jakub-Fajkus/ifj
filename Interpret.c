//
// Created by Andrej on 19.11.2016.
//

// contains:
/*    #include<stdio.h>
      #include<stdlib.h>
      #include<string.h>
      #include<stdarg.h>
      #include<stdbool.h>
*/
#include "ifj16.h"  // Built-in functions & most libraries
#include "Stack.h"
#include "Interpret.h"
//#include "DoubleLinkedList.h"
//#include "BasicStructures.h"


int Interpret(tDLList *InstructionList);
void InstructionExecute(INSTRUCTION *instr);
void executeInstructionMathOperation(INSTRUCTION *instr);
void executeInstructionAssign(VARIABLE *dst, VARIABLE *src);

// TODO: funkcie pre tvorbu a prácu s rámcom

int Interpret( tDLList *InstructionList ){

    if (InstructionList == NULL) {
        fprintf(stderr, "Interpret obtained NULL pointer to list of instructions.\n");
        return 1;
    }

    INSTRUCTION *Instr = malloc(sizeof(INSTRUCTION));
        checkMalloc(Instr);
    LIST_ELEMENT *NewPtr = malloc(sizeof(struct LIST_ELEMENT));
        checkMalloc(NewPtr);
    tDLList *globalFrame = createFrame();
    // create the stack for localFrame here


    ListFirst(InstructionList);

    while ( 1 ) {

        ListElementCopy(InstructionList, NewPtr);
        Instr = NewPtr->data.instr;

        if ( Instr->type == Instruction_Push_Global_Variable ) {
            pushToFrame(globalFrame, Instr);
            ListSuccessor(InstructionList);
            continue; // Jump to another instruction
        }

        // prečítaj mená inštrukcii a vytvor novú inštrukciu
        // do ktorej narveš priamo hodnoty a jebeš na všetko

        //... sem sa dostane program až po odchytení inštrukcii, ktoré majú

        //TODO: first look into top local frame (in stack)
        // ...
        // ...
        // ...

        // Not found in local frame
        VARIABLE *dst = findGlobalVariable(globalFrame, Instr->address_dst);
        VARIABLE *src1 = findGlobalVariable(globalFrame, Instr->address_src1);
        VARIABLE *src2 = findGlobalVariable(globalFrame, Instr->address_src1);

        switch (Instr->type) {
            case Instruction_Assign:    // expecting DST & SRC variable name
                ;
                if ( dst ==NULL || src1 == NULL || src2 != NULL ){
                    //TODO: dst or src1 not found, or accidentally found src2
                }
                executeInstructionAssign(dst, src1);
                break;

            case Instruction_Addition:
            case Instruction_Subtraction:
            case Instruction_Multiply:
            case Instruction_Divide:
                ;
                if ( dst ==NULL || src1 == NULL || src2 == NULL ){
                    //TODO: dst, src1 or src2 not found
                }
                // passing 3 pointers to variables in FRAMES!
                INSTRUCTION *mathInstruction = malloc(sizeof(INSTRUCTION));
                //TODO: try to think out new way of exiting the code

                mathInstruction->type = Instr->type;
                //todo: validate last change!
                *(VARIABLE *)mathInstruction->address_dst = *dst;
                *(VARIABLE *)mathInstruction->address_src1 = *src1;
                *(VARIABLE *)mathInstruction->address_src2 = *src2;

                executeInstructionMathOperation(mathInstruction);

                dst = (VARIABLE *)mathInstruction->address_dst;
                src1 = (VARIABLE *)mathInstruction->address_src1;
                src2 = (VARIABLE *)mathInstruction->address_src2;

                break;

            default: InstructionExecute(Instr); break;
        }

        // po vykonaní novo vytvorenej špeci inštrukcie a narvatí
        // premennných do nej sa dostaň k týmto číslam a vraz ich
        // do frames... zas podľa mena

        // p.s. pri assignment to už netreba. asi to ošéfujem tak aby to vôbec nebolo treba

        if (InstructionList->Last->element.data.instr == Instr) break;
        ListSuccessor(InstructionList);
    } // end of while
    return 0; // I had no idea what have I done
}

//...

void InstructionExecute(INSTRUCTION *instr){
// Always: overiť či sedia typy premenných ktoré idem použiť na danú operáciu, inak bude behová chyba
    // error 7 (behová chyba pri načítaní číselnej hodnoty zo vstupu)
    // error 8 (behová chyba pri práci s neinicializovanou premennou)
    // error 9 (behová chyba delenia nulou)
    // error 10 (ostatné behové chyby) <- zlá premenná... možno sem?

    // code shortening
    VARIABLE *dstAddr = ((VARIABLE*)instr->address_dst);
    VARIABLE *srcAddr1 = ((VARIABLE*)instr->address_src1);
    VARIABLE *srcAddr2 = ((VARIABLE*)instr->address_src2);

    VARIABLE_VALUE *dstVal = &dstAddr->value;
    VARIABLE_VALUE *src1Val = &srcAddr1->value;
    VARIABLE_VALUE *src2Val = &srcAddr2->value;

    //The Giant Switch
    switch ( instr -> type ) {

        case Instruction_CallFunction: /* Tu sa budu diat zazraky s ramcami */ break;
        case Instruction_ReturnFunction: break;
        case Instruction_Jump: /* ulozim si na zasobnik pointer na aktualnu instrukciu */ break;
        case Instruction_Create_Local_Frame: break;
        case Instruction_Push_Local_Variable: break;

        //----------------------------------------------------------------------------------------------------------------
        // BUILD-IN FUNCTIONS
        //----------------------------------------------------------------------------------------------------------------
        case Instruction_Function_readInt:
            // int ifj16_readInt();
            dstVal->intValue = ifj16_readInt();
            break;

        case Instruction_Function_readDouble:
            // double ifj16_readDouble();
            dstVal->doubleValue = ifj16_readDouble();
            break;

        case Instruction_Function_readString:
            // char *ifj16_readString();
            dstVal->stringValue = ifj16_readString();
            break;

        case Instruction_Function_Print:
            // void ifj16_print(char *s);
            //TODO: lots of things to figure out
            break;

        case Instruction_Function_Length:
            // int ifj16_length(char *);
            dstVal->intValue = ifj16_length( src1Val->stringValue );
            break;

        case Instruction_Function_Substr:
            // char *ifj16_substr(char *, int, int);
            // asi mame kurva problem
            break;

        case Instruction_Function_Compare:
            // int ifj16_compare(char *, char *);
            dstVal->intValue = ifj16_compare( src1Val->stringValue , src2Val->stringValue );
            break;

        case Instruction_Function_Find:
            // int ifj16_find(char *, char *);
            dstVal->intValue = ifj16_find( src1Val->stringValue , src2Val->stringValue );
            break;

        case Instruction_Function_Sort:
            // char *ifj16_sort(char *s);
            dstVal->stringValue = ifj16_sort( src1Val->stringValue );
            break;

        //----------------------------------------------------------------------------------------------------------------
        //  BOOL OPERATIONS
        //----------------------------------------------------------------------------------------------------------------
        /*    IF-ONLY?
        case Instruction_Bool_Equals: break;
        case Instruction_Bool_EqualsNot: break;
        case Instruction_Bool_More: break;
        case Instruction_Bool_Less: break;
        case Instruction_Bool_MoreEqual: break;
        case Instruction_Bool_LessEqual: break;
         */
        //----------------------------------------------------------------------------------------------------------------
        default: ;
    } // end of giant switch

} // end of InstructionExecute

//(VARIABLE *dst, VARIABLE *src1, VARIABLE *src2)
void executeInstructionMathOperation(INSTRUCTION *instr) {

    // code shortening: access to data value
    VARIABLE_VALUE *dstVal = &((VARIABLE*)instr->address_dst)->value;
    VARIABLE_VALUE *src1Val = &((VARIABLE*)instr->address_src1)->value;
    VARIABLE_VALUE *src2Val = &((VARIABLE*)instr->address_src2)->value;
    // code shortening: access to data type
    DATA_TYPE dstType = ((VARIABLE*)instr->address_dst)->type;
    DATA_TYPE src1Type = ((VARIABLE*)instr->address_src1)->type;
    DATA_TYPE src2Type = ((VARIABLE*)instr->address_src2)->type;

    switch ( instr->type ) {

    case Instruction_Addition:
        ;

        if (src1Type==TYPE_STRING && src2Type==TYPE_STRING) {
            // Concatenate strings, withtout casting
            if (dstType!=TYPE_STRING) exit(42); //TODO: not sure which errcode to call

            dstVal->stringValue = malloc( strlen(src1Val->stringValue)+strlen(src1Val->stringValue)+1 );
            checkMalloc(dstVal->stringValue);
            strcpy(dstVal->stringValue, src1Val->stringValue);
            strcat(dstVal->stringValue, src2Val->stringValue);

            return; // end instruction
        }
        if ((src1Type==TYPE_STRING && src2Type!=TYPE_STRING) || (src1Type!=TYPE_STRING && src2Type==TYPE_STRING)) {
            // Concatenate strings, with casting
            if (dstType!=TYPE_STRING) exit(42); //TODO: not sure which errcode to call
            char tempString[100];

            if ( src1Type!=TYPE_STRING ){
                // Switch data type of src1 to string
                if ( src1Type==TYPE_INT ) {
                    sprintf(tempString,"%d", src1Val->intValue);
                    int len = (int)strlen(tempString);
                    src1Val->stringValue = malloc( sizeof(char)*len+1 );
                    checkMalloc( src1Val->stringValue );
                    strcpy(src1Val->stringValue, tempString);
                }
                else if ( src1Type==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", src1Val->doubleValue);
                    int len = (int)strlen(tempString);
                    src1Val->stringValue = malloc( sizeof(char)*len+1 );
                    checkMalloc(src1Val->stringValue);
                    strcpy(src1Val->stringValue, tempString);
                }
            }   //end of "type casting" of SRC1

            if ( src2Type!=TYPE_STRING ) {
                // Switch data type of src2 to string
                if ( src2Type==TYPE_INT ) {
                    sprintf(tempString,"%d", src2Val->intValue);
                    int len = (int)strlen(tempString);
                    src2Val->stringValue = malloc( sizeof(char)*len+1 );
                    checkMalloc(src2Val->stringValue);
                    strcpy(src2Val->stringValue, tempString);
                }
                else if ( src2Type==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", src2Val->doubleValue);
                    int len = (int)strlen(tempString);
                    src2Val->stringValue = malloc( sizeof(char)*len+1 );
                    checkMalloc(src2Val->stringValue);
                    strcpy(src2Val->stringValue, tempString);
                }
            }   //end of "type casting" of SRC2

            dstVal->stringValue = malloc( strlen(src1Val->stringValue)+strlen(src1Val->stringValue)+1 );
            checkMalloc(dstVal->stringValue);
            strcpy(dstVal->stringValue, src1Val->stringValue);
            strcat(dstVal->stringValue, src2Val->stringValue);

            return; // end instruction
        }
        switch ( src1Type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2Type==TYPE_INT) dstVal->intValue = src1Val->intValue + src2Val->intValue;
                else {  // SRC2 is double
                    if (dstType==TYPE_INT){ // DST is int
                        dstType = TYPE_DOUBLE;
                        dstVal->doubleValue = (double)dstVal->intValue;
                    }
                    dstVal->doubleValue = (double)src1Val->intValue + src2Val->doubleValue;
                }
            return; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dstType==TYPE_INT) {
                    dstType = TYPE_DOUBLE;
                    dstVal->doubleValue = (double)dstVal->intValue;
                }
                if (src2Type==TYPE_DOUBLE) dstVal->doubleValue = src1Val->doubleValue + src2Val->doubleValue;
                else dstVal->doubleValue = src1Val->doubleValue + (double)src2Val->intValue;
            return; // end instruction

            default: ;
        }
    break;

    case Instruction_Subtraction:
        ;
        if (src1Type == TYPE_STRING || src2Type == TYPE_STRING) exitInterpret(10);
        switch ( src1Type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2Type==TYPE_INT) {
                    dstVal->intValue = src1Val->intValue - src2Val->intValue;
                    //TODO: not sure which exit code to call
                    if (dstVal->intValue < 0) exit(42);
                }
                else {  // SRC2 is double
                    if (dstType==TYPE_INT){ // DST is int
                        dstType = TYPE_DOUBLE;
                        dstVal->doubleValue = (double)dstVal->intValue;
                    }
                    dstVal->doubleValue = (double)src1Val->intValue - src2Val->doubleValue;
                    //TODO: not sure which exit code to call
                    if (dstVal->doubleValue < 0) exit(42);
                }

            return; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dstType==TYPE_INT) {
                    dstType = TYPE_DOUBLE;
                    dstVal->doubleValue = (double)dstVal->intValue;
                }
                if (src2Type==TYPE_DOUBLE) dstVal->doubleValue = src1Val->doubleValue - src2Val->doubleValue;
                else dstVal->doubleValue = src1Val->doubleValue - (double)src2Val->intValue;
                if (dstVal->doubleValue < 0) exit(42);
            return; // end instruction

            default: ;
        }
    break;

    case Instruction_Multiply:
        ;
        if (src1Type == TYPE_STRING || src2Type == TYPE_STRING) exitInterpret(10);
        switch ( src1Type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2Type==TYPE_INT) dstVal->intValue = src1Val->intValue * src2Val->intValue;
                else {  // SRC2 is double
                    if (dstType==TYPE_INT){ // DST is int
                        dstType = TYPE_DOUBLE;
                        dstVal->doubleValue = (double)dstVal->intValue;
                    }
                    dstVal->doubleValue = (double)src1Val->intValue * src2Val->doubleValue;
                }
            return; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dstType==TYPE_INT) {
                    dstType = TYPE_DOUBLE;
                    dstVal->doubleValue = (double)dstVal->intValue;
                }
                if (src2Type==TYPE_DOUBLE) dstVal->doubleValue = src1Val->doubleValue * src2Val->doubleValue;
                else dstVal->doubleValue = src1Val->doubleValue * (double)src2Val->intValue;
            return; // end instruction

            default: ;
        }
    break;

    case Instruction_Divide:
        ;
        if (src1Type == TYPE_STRING || src2Type == TYPE_STRING) exitInterpret(10);
        switch ( src1Type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2Type==TYPE_INT) {
                    if (src2Val->intValue==0) exitInterpret(9); // dividing by zero
                    dstVal->intValue = src1Val->intValue / src2Val->intValue;
                }
                else {  // SRC2 is double
                    if (dstType==TYPE_INT){ // DST is int
                        dstType = TYPE_DOUBLE;
                        dstVal->doubleValue = (double)dstVal->intValue;
                    }
                    if (src2Val->doubleValue==0.0) exitInterpret(9); // dividing by zero
                    dstVal->doubleValue = (double)src1Val->intValue / src2Val->doubleValue;
                }
            return; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dstType==TYPE_INT) {
                    dstType = TYPE_DOUBLE;
                    dstVal->doubleValue = (double)dstVal->intValue;
                }
                if (src2Type==TYPE_DOUBLE) {
                    if (src2Val->doubleValue==0.0) exitInterpret(9); // dividing by zero
                    dstVal->doubleValue = src1Val->doubleValue / src2Val->doubleValue;
                }
                else {
                    if (src2Val->intValue==0) exitInterpret(9); // dividing by zero
                    dstVal->doubleValue = src1Val->doubleValue / (double)src2Val->intValue;
                }
            return; // end instruction

            default: ;
        }
    break;

    default: ;
    }   // end if Math Instructions switch

}   // end of Executing

//...

void executeInstructionAssign(VARIABLE *dst, VARIABLE *src) {
    int type_dst = dst->type;
    int type_src = src->type;

    switch (type_dst) {

        case TYPE_INT:
            if (type_src==TYPE_INT) dst->value.intValue = src->value.intValue;
            else exitInterpret(6); // Incompatibile assign
            break;

        case TYPE_DOUBLE:
            if (type_src==TYPE_DOUBLE) { // Correct assignemt
                dst->value.doubleValue = src->value.doubleValue;
            }
            else if (type_src==TYPE_INT) { // Type cast INT->DOUBLE & assign
                dst->value.doubleValue = (double)src->value.intValue;
            }
            else exitInterpret(6); // Incompatibile assign
            break;

        case TYPE_STRING:
            if (type_src==TYPE_STRING) dst->value.stringValue = src->value.stringValue;
            else exitInterpret(6);
            break;

        default: ;
    }
}   // end of Assign instruction

//...

void exitInterpret(int errNumber) {
    switch (errNumber) {
        case 6:
            fprintf(stderr, "Semantic error: other.\n");
        case 7:
            fprintf(stderr, "Runtime error: loading values from input.\n");
        case 8:
            fprintf(stderr, "Runtime error: working with uninitialized variables\n");
        case 9:
            fprintf(stderr, "Runtime error: dividing by zero.\n");
        case 10:
            fprintf(stderr, "Interpret: Other runtime error.\n");
        case 99:
            fprintf(stderr, "Intern error: Unable to allocate memory.");
        default: ;
    } // end of switch

    //TODO: free all allocated memory

    exit(errNumber);
}

//...

void checkMalloc(void *ptr){
    if ( ptr == NULL ) {
        fprintf(stderr, "Intern error: Unable to allocate memory.\n");
        exitInterpret(99);
    }
}
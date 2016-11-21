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

void InstructionExecute(INSTRUCTION *Instr);
void checkMalloc(void *ptr);
// TODO: funkcie pre tvorbu a prácu s rámcom


///
/// \param InstructionList
/// \return
int Interpret( tDLList *InstructionList ){

    if (InstructionList == NULL) {
        fprintf(stderr, "Obtained NULL pointer.\n");
        return 1;
    }

    INSTRUCTION *Instr = malloc(sizeof(INSTRUCTION));
    if (Instr == NULL) {
        fprintf(stderr, "Unable to allocate memory in function Interpret.\n");
        //TODO: free all allocated memory
        exit(99);
    }
    LIST_ELEMENT *NewPtr = malloc(sizeof(struct LIST_ELEMENT));
    if (NewPtr == NULL) {
        fprintf(stderr, "Unable to allocate memory in function Interpret.\n");
        //TODO: free all allocated memory
        exit(99);
    }

    ListFirst(InstructionList);

    while ( 1 ) {
        ListElementCopy(InstructionList, NewPtr);
        Instr = NewPtr->data.instr;

        InstructionExecute(Instr);

        if (InstructionList->Last->element.data.instr == Instr) break;
        ListSuccessor(InstructionList);
    } // end of while
    return 0; // I had no idea what have I done
}

///
/// \param Instr
void InstructionExecute(INSTRUCTION *Instr){
// Always: overiť či sedia typy premenných ktoré idem použiť na danú operáciu, inak bude behová chyba
    // error 7 (behová chyba pri načítaní číselnej hodnoty zo vstupu)
    // error 8 (behová chyba pri práci s neinicializovanou premennou)
    // error 9 (behová chyba delenia nulou)
    // error 10 (ostatné behové chyby) <- zlá premenná... možno sem?

    // code shortening
    VARIABLE *dstAddr = ((VARIABLE*)Instr->address_dst);
    VARIABLE *srcAddr1 = ((VARIABLE*)Instr->address_src1);
    VARIABLE *srcAddr2 = ((VARIABLE*)Instr->address_src2);

    VARIABLE_VALUE *dstVal = &dstAddr->value;
    VARIABLE_VALUE *src1Val = &srcAddr1->value;
    VARIABLE_VALUE *src2Val = &srcAddr2->value;

    //The Giant Switch
    switch ( Instr -> type ) {

        case Instruction_Assign: /* Kontrola typov, nieco v style dst=src1 */
            ;
            int a_type1 = dstAddr->type;
            int a_type2 = srcAddr1->type;
            switch ( a_type1 ) {
                case TYPE_INT:
                    if ( a_type2==TYPE_INT ) dstVal->intValue = src1Val->intValue; // Correct assignemt
                    else exitInterpret(6); // Incompatibile assign

                    break;
                case TYPE_DOUBLE:
                    if ( a_type2==TYPE_DOUBLE ) { // Correct assignemt
                        dstVal->doubleValue = src1Val->doubleValue;
                    }
                    else if ( a_type2==TYPE_INT ) { // Type cast INT->DOUBLE & assign
                        srcAddr1->type = TYPE_DOUBLE;
                        src1Val->doubleValue = (double)src1Val->intValue;
                        dstVal->doubleValue = src1Val->doubleValue;
                    }
                    else exitInterpret(6); // Incompatibile assign

                    break;
                case TYPE_STRING:
                    if ( a_type2==TYPE_STRING ) { // Correct assignemt
                        dstVal->stringValue = src1Val->stringValue;
                    }
                    else exitInterpret(6); // Incompatibile assign

                    break;
            }
            break;

        case Instruction_Create_Global_Frame: break;
        case Instruction_Push_Global_Variable: break;

            //----------------------------------------------------------------------------------------------------------------
            //  MATH OPERATOINS
            //----------------------------------------------------------------------------------------------------------------
        case Instruction_Addition:
        case Instruction_Subtraction:
        case Instruction_Multiply:
        case Instruction_Divide:
            ;
            DATA_TYPE type1 = srcAddr1->type;  //Data type of Src1
            DATA_TYPE type2 = srcAddr2->type;  //Data Type of Src2
            if ( (type1==TYPE_INT && type2==TYPE_DOUBLE) || (type1==TYPE_DOUBLE && type2==TYPE_INT) ) {
                // type cast of Src1 from int to double
                if ( type1 == TYPE_INT ) {
                    srcAddr1->type = TYPE_DOUBLE;
                    // TODO: môže to tu zhavarovať
                    src1Val->doubleValue = (double)src1Val->intValue;
                }
                // type cast of Src2 from int to double
                if ( type2 == TYPE_INT ) {
                    srcAddr2->type = TYPE_DOUBLE;
                    src2Val->doubleValue = (double)src2Val->intValue;
                }
            }
            else if ( type1==TYPE_STRING || type2==TYPE_STRING ){
                // anything else than concatenation ends with program failure
                if ( Instr->type != Instruction_Addition ) exitInterpret(10);

                char tempString[100]; // do not move
                if ( type1!=TYPE_STRING ){
                    // Switch data type of src1 to string
                    if ( type1==TYPE_INT ) {
                        sprintf(tempString,"%d", src1Val->intValue);
                        int len = (int)strlen(tempString);
                        src1Val->stringValue = malloc( sizeof(char)*len+1 );
                        checkMalloc( src1Val->stringValue );
                        strcpy(src1Val->stringValue, tempString);
                    }
                    else if ( type1==TYPE_DOUBLE ) {
                        sprintf(tempString,"%g", src1Val->doubleValue);
                        int len = (int)strlen(tempString);
                        src1Val->stringValue = malloc( sizeof(char)*len+1 );
                        checkMalloc(src1Val->stringValue);
                        strcpy(src1Val->stringValue, tempString);
                    }
                }   //end of type casting
                if ( type2!=TYPE_STRING ) {
                    // Switch data type of src2 to string
                    if ( type2==TYPE_INT ) {
                        sprintf(tempString,"%d", src2Val->intValue);
                        int len = (int)strlen(tempString);
                        src2Val->stringValue = malloc( sizeof(char)*len+1 );
                        checkMalloc(src2Val->stringValue);
                        strcpy(src2Val->stringValue, tempString);
                    }
                    else if ( type2==TYPE_DOUBLE ) {
                        sprintf(tempString,"%g", src2Val->doubleValue);
                        int len = (int)strlen(tempString);
                        src2Val->stringValue = malloc( sizeof(char)*len+1 );
                        checkMalloc(src2Val->stringValue);
                        strcpy(src2Val->stringValue, tempString);
                    }
                }   //end of type casting

                // Concatenating two strings now
                dstVal->stringValue = malloc( strlen(src1Val->stringValue)+strlen(src1Val->stringValue)+1 );
                checkMalloc(dstVal->stringValue);
                strcpy(dstVal->stringValue, src1Val->stringValue);
                strcat(dstVal->stringValue, src2Val->stringValue);

                break;
            } // End of string IF
            if( Instr->type == Instruction_Addition ){
                if( srcAddr1->type == TYPE_INT && srcAddr2->type == TYPE_INT ){
                    dstVal->intValue = src1Val->intValue + src2Val->intValue;
                }
                else{
                    dstVal->doubleValue = src1Val->doubleValue + src2Val->doubleValue;
                }
            } // end of ADD

            else if ( Instr->type == Instruction_Subtraction ) {
                if( srcAddr1->type == TYPE_INT && srcAddr2->type == TYPE_INT ){
                    dstVal->intValue = src1Val->intValue - src2Val->intValue;
                    if ( dstVal->intValue < 0 ) exitInterpret(10); // result is less than zero
                }
                else {
                    dstVal->doubleValue = src1Val->doubleValue - src2Val->doubleValue;
                    if ( dstVal->doubleValue < 0.0 ) exitInterpret(10); // result is less than zero
                }
            } // end of SUB

            else if ( Instr->type == Instruction_Multiply ) {
                if( srcAddr1->type == TYPE_INT && srcAddr2->type == TYPE_INT ){
                    dstVal->intValue = src1Val->intValue * src2Val->intValue;
                }
                else{
                    dstVal->doubleValue = src1Val->doubleValue * src2Val->doubleValue;
                }
            } // end of MUL

            else if ( Instr->type == Instruction_Divide ) {
                // + ošetrenie: delenie nulou
                if( srcAddr1->type == TYPE_INT && srcAddr2->type == TYPE_INT ){
                    if ( src2Val->intValue == 0 ) exitInterpret(9); // dividing by zero
                    dstVal->intValue = src1Val->intValue / src2Val->intValue;
                }
                else{
                    if ( src2Val->doubleValue == 0 ) exitInterpret(9); // dividing by zero
                    dstVal->doubleValue = src1Val->doubleValue / src2Val->doubleValue;
                }
            } // end of DIV

            break;  // end of math operations
            //----------------------------------------------------------------------------------------------------------------

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

    } // end of giant switch

} // end of InstructionExecute

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
    } // end of switch

    //TODO: free all allocated memory

    exit(errNumber);
}

void checkMalloc(void *ptr){
    if ( ptr == NULL ) {
        fprintf(stderr, "Intern error: Unable to allocate memory.\n");
        exitInterpret(99);
    }
}
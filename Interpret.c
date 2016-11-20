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
//#include "Interpret.h"
//#include "DoubleLinkedList.h"
//#include "BasicStructures.h"

int Interpret(tDLList *InstructionList);
void InstructionExecute(INSTRUCTION *Instr);
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

    DLFirst(InstructionList);

    while ( 1 ) {
        DLCopy(InstructionList, NewPtr);
        Instr = NewPtr->data.instr;

        InstructionExecute(Instr);

        if (InstructionList->Last->element.data.instr == Instr) break;
        DLSucc(InstructionList);
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

    VARIABLE_VALUE *dstVal = &dstAddr->var_value;
    VARIABLE_VALUE *src1Val = &srcAddr1->var_value;
    VARIABLE_VALUE *src2Val = &srcAddr2->var_value;

    //The Giant Switch
    switch ( Instr -> type ) {

    case Instruction_Assign: /* Kontrola typov, nieco v style dst=src1 */
        ;
        int a_type1 = dstAddr->var_type;
        int a_type2 = srcAddr1->var_type;
        switch ( a_type1 ) {
            case TYPE_INT:
                if ( a_type2==TYPE_INT )  // Correct assignemt
                    dstVal->intValue = src1Val->intValue;
                else { // Incompatibile assign
                    fprintf(stderr, "Semantic error: Incompatibile variable assignment.\n");
                    exit(6);
                }
                break;
            case TYPE_DOUBLE:
                if ( a_type2==TYPE_DOUBLE) { /* everything is fine, i may assign */
                    dstVal->doubleValue = src1Val->doubleValue;
                }
                else if ( a_type2==TYPE_INT ) { // Type cast INT->DOUBLE & assign
                    srcAddr1->var_type = TYPE_DOUBLE;
                    src1Val->doubleValue = (double)src1Val->intValue;
                    dstVal->doubleValue = src1Val->doubleValue;
                }
                else { // Incompatibile assign
                    fprintf(stderr, "Semantic error: Incompatibile variable assignment.\n");
                    exit(6);
                }
                break;
            case TYPE_STRING:
                if ( a_type2==TYPE_STRING ) { // Correct assignemt
                    dstVal->stringValue = src1Val->stringValue;
                }
                else { // Incompatibile assign
                    fprintf(stderr, "Semantic error: Incompatibile variable assignment.\n");
                    exit(6);
                }
                break;
        }
        break;

    case Instruction_Create_Global_Frame: break;
    case Instruction_Push_Global_Variable: break;

    //----------------------------------------------------------------------------------------------------------------
    //  MATH OPERATOINS
    //----------------------------------------------------------------------------------------------------------------
    case Instruction_Addition:
    case Instruction_Substraction:
    case Instruction_Multiply:
    case Instruction_Divide:
        ;
        DATA_TYPE type1 = srcAddr1->var_type;  //Data type of Src1
        DATA_TYPE type2 = srcAddr2->var_type;  //Data Type of Src2
        if ( (type1==TYPE_INT && type2==TYPE_DOUBLE) || (type1==TYPE_DOUBLE && type2==TYPE_INT) ) {
            // type cast of Src1 from int to double
            if ( type1 == TYPE_INT ) {
                srcAddr1->var_type = TYPE_DOUBLE;
                // TODO: môže to tu zhavarovať
                srcAddr1->var_value.doubleValue = (double)srcAddr1->var_value.intValue;
            }
            // type cast of Src2 from int to double
            if ( type2 == TYPE_INT ) {
                srcAddr2->var_type = TYPE_DOUBLE;
                srcAddr2->var_value.doubleValue = (double)srcAddr2->var_value.intValue;
            }
        }
        else if ( type1==TYPE_STRING || type2==TYPE_STRING ){
            if ( Instr->type != Instruction_Addition ) {    // anything else than concatenation ends with program failure
                fprintf(stderr, "Type mismatch.\n");
                exit(10);
            }
            char tempString[100]; // do not move
            if ( type1!=TYPE_STRING ){
                // Switch data type of src1 to string
                if ( type1==TYPE_INT ) {
                    sprintf(tempString,"%d", srcAddr1->var_value.intValue);
                    int len = (int)strlen(tempString);
                    srcAddr1->var_value.stringValue = malloc( sizeof(char)*len+1 );
                    if ( srcAddr1->var_value.stringValue == NULL ){
                        fprintf(stderr, "Runtime error: Unable to allocate memory.");
                        //TODO: free all allocated memory
                        exit(99);
                    }
                    strcpy(srcAddr1->var_value.stringValue, tempString);
                }
                else if ( type1==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", srcAddr1->var_value.doubleValue);
                    int len = (int)strlen(tempString);
                    srcAddr1->var_value.stringValue = malloc( sizeof(char)*len+1 );
                    if ( srcAddr1->var_value.stringValue == NULL ){
                        fprintf(stderr, "Runtime error: Unable to allocate memory.");
                        //TODO: free all allocated memory
                        exit(99);
                    }
                    strcpy(srcAddr1->var_value.stringValue, tempString);
                }
            }   //end of type casting
            if ( type2!=TYPE_STRING ) {
                // Switch data type of src2 to string
                if ( type2==TYPE_INT ) {
                    sprintf(tempString,"%d", srcAddr2->var_value.intValue);
                    int len = (int)strlen(tempString);
                    srcAddr2->var_value.stringValue = malloc( sizeof(char)*len+1 );
                    if ( srcAddr2->var_value.stringValue == NULL ){
                        fprintf(stderr, "Runtime error: Unable to allocate memory.");
                        //TODO: free all allocated memory
                        exit(99);
                    }
                    strcpy(srcAddr2->var_value.stringValue, tempString);
                }
                else if ( type2==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", srcAddr2->var_value.doubleValue);
                    int len = (int)strlen(tempString);
                    srcAddr2->var_value.stringValue = malloc( sizeof(char)*len+1 );
                    strcpy(srcAddr2->var_value.stringValue, tempString);
                }
            }   //end of type casting

            //TODO: fix case: dst is not string
            // nieje nahodou DST nieco ine ako string?

            // Concatenating two strings now
            dstAddr->var_value.stringValue = malloc( strlen(srcAddr1->var_value.stringValue)+strlen(srcAddr1->var_value.stringValue)+1 );
            if ( dstAddr->var_value.stringValue == NULL ){
                fprintf(stderr, "Runtime error: Unable to allocate memory.");
                //TODO: free all allocated memory
                exit(99);
            }

            strcpy(dstAddr->var_value.stringValue, srcAddr1->var_value.stringValue);
            strcat(dstAddr->var_value.stringValue, srcAddr2->var_value.stringValue);

            printf("ZKONKATENOVANY STRING: %s\n", dstAddr->var_value.stringValue);

        } // End of string IF
        if( Instr->type == Instruction_Addition ){
            if( srcAddr1->var_type == TYPE_INT && srcAddr2->var_type == TYPE_INT ){
                dstAddr->var_value.intValue = srcAddr1->var_value.intValue + srcAddr2->var_value.intValue;
            }
            else{
                dstAddr->var_value.doubleValue = srcAddr1->var_value.doubleValue + srcAddr2->var_value.doubleValue;
            }
        } // end of ADD

        else if ( Instr->type == Instruction_Substraction ) {
            if( srcAddr1->var_type == TYPE_INT && srcAddr2->var_type == TYPE_INT ){
                dstAddr->var_value.intValue = srcAddr1->var_value.intValue - srcAddr2->var_value.intValue;
                if ( dstAddr->var_value.intValue < 0 ) { // result is less than zero
                    fprintf(stderr, "Substraction error: result is less than zero.\n");
                    exit(10);
                }
            }
            else{
                dstAddr->var_value.doubleValue = srcAddr1->var_value.doubleValue - srcAddr2->var_value.doubleValue;
                if ( dstAddr->var_value.doubleValue < 0.0 ) { // result is less than zero
                    fprintf(stderr, "Substraction error: result is less than zero.\n");
                    exit(10);
                }
            }
        } // end of SUB

        else if ( Instr->type == Instruction_Multiply ) {
            if( srcAddr1->var_type == TYPE_INT && srcAddr2->var_type == TYPE_INT ){
                dstAddr->var_value.intValue = srcAddr1->var_value.intValue * srcAddr2->var_value.intValue;
            }
            else{
                dstAddr->var_value.doubleValue = srcAddr1->var_value.doubleValue * srcAddr2->var_value.doubleValue;
            }
        } // end of MUL

        else if ( Instr->type == Instruction_Divide ) {
            // + ošetrenie: delenie nulou
            if( srcAddr1->var_type == TYPE_INT && srcAddr2->var_type == TYPE_INT ){
                if ( srcAddr2->var_value.intValue == 0 ) { // dividing by zero
                    fprintf(stderr, "Division error: dividing by zero.\n");
                    exit(9);
                }
                dstAddr->var_value.intValue = srcAddr1->var_value.intValue / srcAddr2->var_value.intValue;
            }
            else{
                if ( srcAddr2->var_value.doubleValue == 0 ) { // dividing by zero
                    fprintf(stderr, "Division error: dividing by zero.\n");
                    exit(9);
                }
                dstAddr->var_value.doubleValue = srcAddr1->var_value.doubleValue / srcAddr2->var_value.doubleValue;
            }
        } // end of DIV

        break;  // end of math operations
        //----------------------------------------------------------------------------------------------------------------

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

        case Instruction_CallFunction: /* Tu sa budu diat zazraky s ramcami */ break;
        case Instruction_ReturnFunction: break;
        case Instruction_NOP: break;
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

    } // end of giant switch

} // end of InstructionExecute




//* ************************************************ CREATE INSTRUCTION ************************************************

INSTRUCTION *createPushGlobalVariable(char *name, DATA_TYPE type, VARIABLE_VALUE value) {
    INSTRUCTION *instruction = malloc(sizeof(INSTRUCTION));

    instruction->type = Instruction_Push_Global_Variable;
    instruction->address_dst = name;
    instruction->address_src1 = malloc(sizeof(DATA_TYPE));
    *(int*)instruction->address_src1 = type;
    instruction->address_src2 = malloc(sizeof(VARIABLE_VALUE));
    *(VARIABLE_VALUE*)instruction->address_src2 = value;

    return instruction;
}

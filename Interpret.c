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
#include "DoubleLinkedList.h"
#include "BasicStructures.h"

int Interpret(tDLList *InstructionList);
void InstructionExecute(INSTRUCTION *Instr);
// TODO: funkcie pre tvorbu a prácu s rámcom


int Interpret( tDLList *InstructionList ){

    if (InstructionList == NULL) {
        fprintf(stderr, "Obtained NULL pointer.\n");
        return 1;
    }

    INSTRUCTION *Instr = malloc(sizeof(INSTRUCTION));
    if (Instr == NULL) {
        fprintf(stderr, "Unable to allocate memory in function Interpret.\n");
        //free_all
        exit(99);
    }
    LIST_ELEMENT *NewPtr = malloc(sizeof(struct LIST_ELEMENT));
    if (NewPtr == NULL) {
        fprintf(stderr, "Unable to allocate memory in function Interpret.\n");
        //free_all
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

    //The Giant Switch
    switch ( Instr -> type ) {

        case Instruction_NOP: break;
        case Instruction_Jump:
            // ulozim si na zasobnik pointer na aktualnu instrukciu
            break;
        case Instruction_Assign:
            // KONTROLA TYPOV
            //((VARIABLE*)Instr->address_dst) = ((VARIABLE*)Instr->address_src1);
            break;

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
                // Pretypovanie premennej na Src1
                if ( type1 == TYPE_INT ) {
                    srcAddr1->var_type = TYPE_DOUBLE;
                    // TODO: môže to tu zhavarovať
                    srcAddr1->var_value.doubleValue = (double)srcAddr1->var_value.intValue;
                }
                // Pretypovanie premennej na Src2
                if ( type2 == TYPE_INT ) {
                    srcAddr2->var_type = TYPE_DOUBLE;
                    srcAddr2->var_value.doubleValue = (double)srcAddr2->var_value.intValue;
                }
            }
            else if ( type1==TYPE_STRING || type2==TYPE_STRING ){
                if ( Instr->type != Instruction_Addition ) {
                    fprintf(stderr, "Type mismatch.\n");
                    exit(10);
                }
                char tempString[100]; // do not move
                if ( type1!=TYPE_STRING ){
                    // Switch data type of src1 to string
                    if ( type1==TYPE_INT ) {
                        //printf(" Vstupujeme do ifu, kde typ je int\n");
                        sprintf(tempString,"%d", srcAddr1->var_value.intValue);
                        int len = (int)strlen(tempString);
                        //printf("provedl se prevod int na string\n");
                        srcAddr1->var_value.stringValue = malloc( sizeof(char)*len+1 );
                        strcpy(srcAddr1->var_value.stringValue, tempString);
                        //printf("vyhodnoceno: %s\n", srcAddr1->var_value.stringValue);
                    }
                    else if ( type1==TYPE_DOUBLE ) {
                        //printf(" Vstupujeme do ifu, kde typ je double\n");
                        sprintf(tempString,"%g", srcAddr1->var_value.doubleValue);
                        int len = (int)strlen(tempString);
                        //printf("provedl se prevod double na string\n");
                        srcAddr1->var_value.stringValue = malloc( sizeof(char)*len+1 );
                        strcpy(srcAddr1->var_value.stringValue, tempString);
                        //printf("vyhodnoceno: %s\n", srcAddr1->var_value.stringValue);
                    }

                }
                if ( type2!=TYPE_STRING ) {
                    // Switch data type of src2 to string
                    if ( type2==TYPE_INT ) {
                        sprintf(tempString,"%d", srcAddr2->var_value.intValue);
                        int len = (int)strlen(tempString);
                        srcAddr2->var_value.stringValue = malloc( sizeof(char)*len+1 );
                        strcpy(srcAddr2->var_value.stringValue, tempString);
                    }
                    else if ( type2==TYPE_DOUBLE ) {
                        sprintf(tempString,"%g", srcAddr2->var_value.doubleValue);
                        int len = (int)strlen(tempString);
                        srcAddr2->var_value.stringValue = malloc( sizeof(char)*len+1 );
                        strcpy(srcAddr2->var_value.stringValue, tempString);
                    }
                }

                // nieje nahodou DST nieco ine ako string?
                //TODO

                // Concatenating two strings now
                printf("KONKATENUJEM\n");
                dstAddr->var_value.stringValue = malloc( strlen(srcAddr1->var_value.stringValue)+strlen(srcAddr1->var_value.stringValue)+1 );

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

            break;
            //----------------------------------------------------------------------------------------------------------------

            //----------------------------------------------------------------------------------------------------------------
            //  BOOL OPERATIONS
            //----------------------------------------------------------------------------------------------------------------
        case Instruction_Bool_Equals: break;
        case Instruction_Bool_EqualsNot: break;
        case Instruction_Bool_More: break;
        case Instruction_Bool_Less: break;
        case Instruction_Bool_MoreEqual: break;
        case Instruction_Bool_LessEqual: break;
            //----------------------------------------------------------------------------------------------------------------

        case Instruction_CallFunction:
            // Tu sa budu diat zazraky s ramcami
            break;
        case Instruction_ReturnFunction:
            break;


            //----------------------------------------------------------------------------------------------------------------
            // BUILD-IN FUNCTIONS
            //----------------------------------------------------------------------------------------------------------------
        case Instruction_Function_readInt:
            // int ifj16_readInt();
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ifj16_readInt();
            break;

        case Instruction_Function_readDouble:
            // double ifj16_readDouble();
            ((VARIABLE*)Instr->address_dst)->var_value.doubleValue = ifj16_readDouble();
            break;

        case Instruction_Function_readString:
            // char *ifj16_readString();
            ((VARIABLE*)Instr->address_dst)->var_value.stringValue = ifj16_readString();
            break;

        case Instruction_Function_Print:
            // void ifj16_print(char *s);
            //TODO
            break;

        case Instruction_Function_Length:
            // int ifj16_length(char *);
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ifj16_length( ((VARIABLE*)Instr->address_src1)->var_value.stringValue );
            break;

        case Instruction_Function_Substr:
            // char *ifj16_substr(char *, int, int);
            // asi mame kurva problem
            break;

        case Instruction_Function_Compare:
            // int ifj16_compare(char *, char *);
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ifj16_compare( ((VARIABLE*)Instr->address_src1)->var_value.stringValue , ((VARIABLE*)Instr->address_src2)->var_value.stringValue );
            break;

        case Instruction_Function_Find:
            // int ifj16_find(char *, char *);
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ifj16_find( ((VARIABLE*)Instr->address_src1)->var_value.stringValue , ((VARIABLE*)Instr->address_src2)->var_value.stringValue );
            break;

        case Instruction_Function_Sort:
            // char *ifj16_sort(char *s);
            ((VARIABLE*)Instr->address_dst)->var_value.stringValue = ifj16_sort( ((VARIABLE*)Instr->address_src1)->var_value.stringValue );
            break;

    } // end of giant switch

} // end of InstructionExecute









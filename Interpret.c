//
// Created by Andrej on 19.11.2016.
//

// TODO: Pridať 3 riadky kódu do DLlist.h
/* TODO: tipujem že budeme niekedy potrebovať zásobník rámcov.
  Prerobiť Stack.h na kompatibilný pre rámce (FRAMES)
*/

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

    VARIABLE *DstAddress = ((VARIABLE*)Instr->address_dst);
    VARIABLE *SrcAddress1 = ((VARIABLE*)Instr->address_src1);
    VARIABLE *SrcAddress2 = ((VARIABLE*)Instr->address_src2);

    (void)DstAddress;
    (void)SrcAddress1;
    (void)SrcAddress2;

//The Giant Switch
    switch ( Instr -> type ) {
        // Nejaky kratky koment co sa tu vlastne kurva deje
        //case 99: break;

        //----------------------------------------------------------------------------------------------------------------
        //  MATH OPERATOINS
        //  Pozor: odcitanie moze prejst do zaporneho cisla, osetrit
        //----------------------------------------------------------------------------------------------------------------
        case Instruction_Addition:
            // ošetrenia: int+int, int+dbl, dbl+dbl
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ((VARIABLE*)Instr->address_src1)->var_value.intValue + ((VARIABLE*)Instr->address_src2)->var_value.intValue;
            break;

        case Instruction_Substraction:
            // ošetrenia: int&int, int&dbl, dbl&dbl
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ((VARIABLE*)Instr->address_src1)->var_value.intValue - ((VARIABLE*)Instr->address_src2)->var_value.intValue;
            break;

        case Instruction_Multiply:
            // ošetrenia: int&int, int&dbl, dbl&dbl
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ((VARIABLE*)Instr->address_src1)->var_value.intValue * ((VARIABLE*)Instr->address_src2)->var_value.intValue;
            break;

        case Instruction_Divide:
            // ošetrenia: int&int, int&dbl, dbl&dbl
            // + ošetrenie: delenie nulou
            ((VARIABLE*)Instr->address_dst)->var_value.intValue = ((VARIABLE*)Instr->address_src1)->var_value.intValue / ((VARIABLE*)Instr->address_src2)->var_value.intValue;
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












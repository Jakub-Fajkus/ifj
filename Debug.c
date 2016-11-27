//
// Created by Jakub Fajkus on 17.11.16.
//
#include "Debug.h"
#include "Stack.h"
#include "BasicStructures.h"

void printAllTokens(tDLList *list) {
    LIST_ELEMENT *actualElement = malloc(sizeof(LIST_ELEMENT));
    ListFirst(list);

    while(DLActive(list) != 0) {
        ListElementCopy(list, actualElement);
        ListSuccessor(list);
        printToken(actualElement->data.token);
    }

    return;
}

void printEnumValue(int value) {
    switch (value) {
        case KEYWORD: debugPrintf("KEYWORD\n"); break;
        case IDENTIFIER_FULL: debugPrintf("IDENTIFIER_FULL\n"); break;
        case IDENTIFIER: debugPrintf("IDENTIFIER\n"); break;
        case LITERAL_STRING: debugPrintf("LITERAL_STRING\n"); break;
        case LITERAL_INTEGER: debugPrintf("LITERAL_INTEGER\n"); break;
        case LITERAL_DOUBLE: debugPrintf("LITERAL_DOUBLE\n"); break;
        case OPERATOR_ARITHMETIC: debugPrintf("OPERATOR_ARITHMETIC\n"); break;
        case OPERATOR_LOGIC: debugPrintf("OPERATOR_LOGIC\n"); break;
        case OPERATOR_ASSIGN: debugPrintf("OPERATOR_ASSIGN\n"); break;
        case SEPARATOR: debugPrintf("SEPARATOR\n"); break;
        case BRACKET: debugPrintf("BRACKET\n"); break;
        case SEMICOLON: debugPrintf("SEMICOLON\n"); break;
        case END_OF_FILE: debugPrintf("END_OF_FILE\n"); break;
        case LEX_ERROR: debugPrintf("LEX_ERROR\n"); break;
    }
}

void printToken(TOKEN *token) {
    switch (token->type) {
        case KEYWORD: debugPrintf("KEYWORD: %s\n", token->data.keyword.name); break;
        case IDENTIFIER_FULL: debugPrintf("IDENTIFIER_FULL: %s.%s\n", token->data.identifierFull.class, token->data.identifierFull.name); break;
        case IDENTIFIER: debugPrintf("IDENTIFIER: %s\n", token->data.identifier.name); break;
        case LITERAL_STRING: debugPrintf("LITERAL_STRING: %s\n",token->data.literalString.name ); break;
        case LITERAL_INTEGER: debugPrintf("LITERAL_INTEGER: %d\n", token->data.numberInteger.value); break;
        case LITERAL_DOUBLE: debugPrintf("LITERAL_DOUBLE: %lf\n", token->data.numberDouble.value); break;
        case OPERATOR_ARITHMETIC: debugPrintf("OPERATOR_ARITHMETIC: %c\n", token->data.operatorArithmetic.name); break;
        case OPERATOR_LOGIC: debugPrintf("OPERATOR_LOGIC: %s\n", token->data.operatorLogic.name); break;
        case OPERATOR_ASSIGN: debugPrintf("OPERATOR_ASSIGN:\n"); break;
        case SEPARATOR: debugPrintf("SEPARATOR: %c\n", ','); break;
        case BRACKET: debugPrintf("BRACKET: %c\n", token->data.bracket.name); break;
        case SEMICOLON: debugPrintf("SEMICOLON: %c\n", ';'); break;
        case END_OF_FILE: debugPrintf("END_OF_FILE:\n"); break;
        case LEX_ERROR: debugPrintf("LEX_ERROR: \n"); break;
    }
}

void printStack(tStack stack) {
    STACK_ELEMENT *actual = malloc(sizeof(STACK_ELEMENT));
    STACK_ELEMENT *last;

    stackTop(&stack, actual);

    debugPrintf("printing the stack from the top to the bottom:\n");

    //it is updated in the function stackTop
    while(!stackEmpty(&stack)) {
        debugPrintf("....");
        if (actual->type == STACK_ELEMENT_TYPE_EXPRESSION) {
            debugPrintf("type: STACK_ELEMENT_TYPE_EXPRESSION \n");
        } else if (actual->type == EA_TERMINAL) {
            debugPrintf("type: EA_TERMINAL \n");
            printToken(&actual->data.terminalData.token);
        } else if (actual->type == EA_NOT_TERMINAL) {
            debugPrintf("type: EA_NOT_TERMINAL \n");
        } else if (actual->type == EA_TERMINAL_ACTION) {
            debugPrintf("type: EA_TERMINAL_ACTION \n");
        } else {
            debugPrintf("unknown type \n");
        }

        printEaTerminalType(actual->data.terminalData.type);



        stackPop(&stack);
        last = actual;
        stackTop(&stack, actual);

    }

    debugPrintf("printing the stack is OVER:\n\n\n\n");
}


void printEaTerminalType(enum EA_TERMINAL_TYPE_ENUM type) {
    switch (type) {
        case EA_ADD: debugPrintf("terminal type: EA_ADD\n"); break;
        case EA_SUB: debugPrintf("terminal type: EA_SUB\n"); break;
        case EA_MUL: debugPrintf("terminal type: EA_MUL\n"); break;
        case EA_LEFT_BR: debugPrintf("terminal type: EA_LEFT_BR\n"); break;
        case EA_RIGHT_BR: debugPrintf("terminal type: EA_RIGHT_BR\n"); break;
        case EA_I: debugPrintf("terminal type: EA_I\n"); break;
        case EA_DIV: debugPrintf("terminal type: EA_DIV\n"); break;
        case EA_IS_LESS: debugPrintf("terminal type: EA_IS_LESS\n"); break;
        case EA_IS_MORE: debugPrintf("terminal type: EA_IS_MORE\n"); break;
        case EA_IS_LESS_EQUAL: debugPrintf("terminal type: EA_IS_LESS_EQUAL\n"); break;
        case EA_IS_MORE_EQUAL: debugPrintf("terminal type: EA_IS_MORE_EQUAL\n"); break;
        case EA_IS_EQUAL: debugPrintf("terminal type: EA_IS_EQUAL\n"); break;
        case EA_IS_NOT_EQUAL: debugPrintf("terminal type: EA_IS_NOT_EQUAL\n"); break;
        case EA_START_END: debugPrintf("terminal type: EA_START_END\n"); break;
        case EA_UNKNOWN: debugPrintf("terminal type: EA_UNKNOWN\n"); break;
        case EA_EMPTY: debugPrintf("terminal type: EA_EMPTY\n"); break;
    }
}

void printInstruction(INSTRUCTION *instruction) {
    switch (instruction->type) {
        case Instruction_Create_GlobalFrame_And_LocalStack: printf("Instruction_Create_GlobalFrame_And_LocalStack\n"); break;
        case Instruction_Push_Global_Variable:              printf("Instruction_Push_Global_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Global_Variable:            printf("Instruction_Create_Global_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Local_Frame:                printf("Instruction_Create_Local_Frame: \n"); break;
        case Instruction_Push_Local_Variable:               printf("Instruction_Push_Local_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Local_Variable:             printf("Instruction_Create_Local_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_CallFunction:                      printf("Instruction_CallFunction: returnTo: %s\n", (char*)instruction->address_src1); break;
        case Instruction_ReturnFunction:                    printf("Instruction_ReturnFunction\n"); break;
        case Instruction_Assign:                            printf("Instruction_Assign: %s = %s\n", (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_Addition:                          printf("Instruction_Addition %s = %s + %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Subtraction:                       printf("Instruction_Subtraction %s = %s - %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Multiply:                          printf("Instruction_Multiply %s = %s * %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Divide:                            printf("Instruction_Divide %s = %s / %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_IF:                                {

            printf("Instruction_IF: ridiciProm:%s\n", (char*)instruction->address_dst);
            printf("     *****Instructions for true branch:\n");
            printInstructions((tDLList*)instruction->address_src1);
            printf("     *****End of true branch:\n");
            printf("     *****Instructions for false branch:\n");
            printInstructions((tDLList*)instruction->address_src2);
            printf("     *****End of false branch:\n");
            break;
        } ;
        case Instruction_WHILE:                             {
            printf("Instruction_WHILE, result in var: %s\n", (char*)instruction->address_dst);
            printf("     *****Instructions for condition:\n");
            printInstructions((tDLList*)instruction->address_src1);
            printf("     *****End of instructions for condition:\n");

            printf("     *****Instructions for body:\n");
            printInstructions((tDLList*)instruction->address_src2);
            printf("     *****End of instructions for body:\n");
            break;
        }
        case Instruction_Bool_Equals:                       printf("Instruction_Bool_Equals: %s = %s == %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_EqualsNot:                    printf("Instruction_Bool_EqualsNot: %s = %s != %s + \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_More:                         printf("Instruction_Bool_More: %s = %s > %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_Less:                         printf("Instruction_Bool_Less: %s = %s < %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_MoreEqual:                    printf("Instruction_Bool_MoreEqual: %s = %s >= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_LessEqual:                    printf("Instruction_Bool_LessEqual: %s = %s <= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Function_readInt:                  printf("Instruction_Function_readInt returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_readDouble:               printf("Instruction_Function_readDouble returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_readString:               printf("Instruction_Function_readString returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_Print:                    printf("Instruction_Function_Print: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_Length:                   printf("Instruction_Function_Length: %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst); break;
        case Instruction_Function_Substr:                   printf("Instruction_Function_Substr:  returnTo %s\n", (((tDLList*)instruction->address_dst)->First->element.data.parameter->name)); break;
        case Instruction_Function_Compare:                  printf("Instruction_Function_Compare: %s %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_Function_Find:                     printf("Instruction_Function_Find: %s %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_End_Interpret:                     printf("Instruction_End_Interpret\n"); break;
        case Instruction_Push_Actual_Local_Variable: printf("Instruction_Push_Actual_Local_Variable:name %s\n", (char*)instruction->address_dst);break;
        case Instruction_Create_Actual_Local_Variable: printf("Instruction_Create_Actual_Local_Variable: %s\n", (char*)instruction->address_dst);break;
        case Instruction_Copy_To_Upcoming_Frame: printf("Instruction_Copy_To_Upcoming_Frame: %s <= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1);break;

        default: printf("UNKNOWN INSTRUCTION");
    }
}

void printInstructions(tDLList *instructions) {
    tDLElemPtr backup = instructions->Act;

    ListFirst(instructions);
    while(DLActive(instructions)) {
        printInstruction(instructions->Act->element.data.instr);
        ListSuccessor(instructions);
    }

    instructions->Act = backup;
}
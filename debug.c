//
// Created by Jakub Fajkus on 17.11.16.
//
#include "debug.h"
#include "stack.h"
#include "basicstructures.h"

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
        case Instruction_Create_GlobalFrame_And_LocalStack: debugPrintf("Instruction_Create_GlobalFrame_And_LocalStack\n"); break;
        case Instruction_Push_Global_Variable:              debugPrintf("Instruction_Push_Global_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Global_Variable:            debugPrintf("Instruction_Create_Global_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Local_Frame:                debugPrintf("Instruction_Create_Local_Frame: \n"); break;
        case Instruction_Push_Local_Variable:               debugPrintf("Instruction_Push_Local_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Create_Local_Variable:             debugPrintf("Instruction_Create_Local_Variable: %s\n", (char*)instruction->address_dst); break;
        case Instruction_CallFunction:                      debugPrintf("Instruction_CallFunction: \n"); break;
        case Instruction_ReturnFunction:                    debugPrintf("Instruction_ReturnFunction\n"); break;
        case Instruction_Assign:                            debugPrintf("Instruction_Assign: %s = %s\n", (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_Addition:                          debugPrintf("Instruction_Addition %s = %s + %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Subtraction:                       debugPrintf("Instruction_Subtraction %s = %s - %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Multiply:                          debugPrintf("Instruction_Multiply %s = %s * %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Divide:                            debugPrintf("Instruction_Divide %s = %s / %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_IF:                                {

            debugPrintf("\nInstruction_IF: ridiciProm:%s\n", (char*)instruction->address_dst);
            debugPrintf("     *****Instructions for true branch:\n");
            printInstructions((tDLList*)instruction->address_src1);
            debugPrintf("     *****End of true branch:\n\n");
            debugPrintf("     *****Instructions for false branch:\n");
            printInstructions((tDLList*)instruction->address_src2);
            debugPrintf("     *****End of false branch:\n\n");
            break;
        } ;
        case Instruction_WHILE:                             {
            debugPrintf("\nInstruction_WHILE, result in var: %s\n", (char*)instruction->address_dst);
            debugPrintf("     *****Instructions for condition:\n");
            printInstructions((tDLList*)instruction->address_src1);
            debugPrintf("     *****End of instructions for condition:\n");

            debugPrintf("     *****Instructions for body:\n");
            printInstructions((tDLList*)instruction->address_src2);
            debugPrintf("     *****End of instructions for body:\n\n");
            break;
        }
        case Instruction_Bool_Equals:                       debugPrintf("Instruction_Bool_Equals: %s = %s == %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_EqualsNot:                    debugPrintf("Instruction_Bool_EqualsNot: %s = %s != %s + \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_More:                         debugPrintf("Instruction_Bool_More: %s = %s > %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_Less:                         debugPrintf("Instruction_Bool_Less: %s = %s < %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_MoreEqual:                    debugPrintf("Instruction_Bool_MoreEqual: %s = %s >= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Bool_LessEqual:                    debugPrintf("Instruction_Bool_LessEqual: %s = %s <= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1,(char*)instruction->address_src2); break;
        case Instruction_Function_readInt:                  debugPrintf("Instruction_Function_readInt returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_readDouble:               debugPrintf("Instruction_Function_readDouble returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_readString:               debugPrintf("Instruction_Function_readString returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_Print:                    debugPrintf("Instruction_Function_Print: %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_Length:                   debugPrintf("Instruction_Function_Length: %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst); break;
        case Instruction_Function_Sort:                     debugPrintf("Instruction_Function_Sort: %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst); break;
        case Instruction_Function_Substr:                   debugPrintf("Instruction_Function_Substr:  returnTo %s\n", (char*)instruction->address_dst); break;
        case Instruction_Function_Compare:                  debugPrintf("Instruction_Function_Compare: %s %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_Function_Find:                     debugPrintf("Instruction_Function_Find: %s %s returnTo %s\n", (char*)instruction->address_dst, (char*)instruction->address_dst, (char*)instruction->address_src1); break;
        case Instruction_End_Interpret:                     debugPrintf("Instruction_End_Interpret\n"); break;
        case Instruction_Push_Actual_Local_Variable: debugPrintf("Instruction_Push_Actual_Local_Variable:name %s\n", (char*)instruction->address_dst);break;
        case Instruction_Create_Actual_Local_Variable: debugPrintf("Instruction_Create_Actual_Local_Variable: %s\n", (char*)instruction->address_dst);break;
        case Instruction_Copy_From_Actual_To_Upcoming_Frame: debugPrintf("Instruction_Copy_From_Actual_To_Upcoming_Frame: %s <= %s \n", (char*)instruction->address_dst, (char*)instruction->address_src1);break;

        default: debugPrintf("UNKNOWN INSTRUCTION %d", instruction->type);
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

void printFrame(tDLList *frame) {
    do {
        VARIABLE *var = frame->Act->element.data.variable;
        debugPrintf("variable with name: %s and value: ", var->name);
        if (var->type == TYPE_INT) {
            debugPrintf("%d\n", var->value.intValue);
        } else if(var->type == TYPE_DOUBLE) {
            debugPrintf("%g\n", var->value.doubleValue);
        } else {
            debugPrintf("%s\n", var->value.stringValue);
        }
        ListSuccessor(frame);
    } while ( frame->Act != NULL );
}
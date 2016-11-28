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
#include "stack.h"
#include "interpret.h"
#include "basicstructures.h"
#include "debug.h"

int GLOBAL = 0;
int instrCounter = 0;

//TODO: solve execution of insertVar into ActualLocalFrame, and the new CopyToUpcomingFrame instruction

int Interpret( tDLList *InstructionList, tDLList *globalFrame, tStack *stackOfLocalFrames, char *returnValue ){
    if (InstructionList == NULL) return 99;
    int interpretRetVal;
    debugPrintf("\n----- Interpret call No.%d\n",GLOBAL++);

    //NewPtr - pointer to list element (allowing work with instructions inside InstructionList)
    LIST_ELEMENT *NewPtr = malloc(sizeof(struct LIST_ELEMENT));
    INSTRUCTION *Instr;

    tDLList *upcomingLocalFrame = NULL; // creating the pointer, yet not using it
    tDLList *actualLocalFrame = NULL;
    if (stackOfLocalFrames != NULL) {
        actualLocalFrame = getActualLocalFrame(stackOfLocalFrames);    // can be Null
    }

    // DON'T EVER FORGET THIS
    ListFirst(InstructionList);

    while ( 1 ) {   // The great cycle
        // Copy the actual instruction from the list
        ListElementCopy(InstructionList, NewPtr);
        Instr = NewPtr->data.instr;

        debugPrintf("Instruction number %d: ", instrCounter++);

        if (Instr->type == Instruction_End_Interpret) { debugPrintf("Instruction_End_Interpret\n");
            //TODO: return value, free all resources used by interpret (stack & globalframe)

            debugPrintf("----- I am ending!\n");
            return 0;
        }

        // Instruction to create both global frame & stack of local frames (the stack will be empty!)
        if (Instr->type == Instruction_Create_GlobalFrame_And_LocalStack) { debugPrintf("Instruction_Create_GlobalFrame_And_LocalStack\n");
            globalFrame = createFrame();
            stackOfLocalFrames = createFrameStack();
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        if (Instr->type == Instruction_Create_Local_Frame) { debugPrintf("Instruction_Create_Local_Frame\n");
            upcomingLocalFrame = createFrame();
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        // Inserting variable into global frame (with or without value)
        if (Instr->type == Instruction_Create_Global_Variable || Instr->type == Instruction_Push_Global_Variable) {
            debugPrintf("Instruction_Insert_Global_Variable: |%s|\n", (char *)Instr->address_dst);
            pushToFrame(globalFrame, Instr);
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }
        // Inserting variable into UPCOMING local frame (the frame is not in stack)
        if (Instr->type == Instruction_Create_Local_Variable || Instr->type == Instruction_Push_Local_Variable) {
            debugPrintf("Instruction_Insert_Local_Variable-UpcomingFrame: |%s|\n", (char *)Instr->address_dst);
            if (upcomingLocalFrame == NULL) {
                upcomingLocalFrame = createFrame();
            }

            pushToFrame(upcomingLocalFrame, Instr);
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        // Inserting variable into ACTUAL local frame (the frame is on top of stack)
        if (Instr->type == Instruction_Push_Actual_Local_Variable || Instr->type == Instruction_Create_Actual_Local_Variable ) {
            debugPrintf("Instruction_Insert_Local_Variable-ActualFrame: |%s|\n", (char *)Instr->address_dst);
            if (actualLocalFrame == NULL) {
                return 99;
            }

            pushToFrame(actualLocalFrame, Instr);
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        // Used in sequence before getting CallFunction
        // Finds variable of desired name in two frames: actual local & global.
        // Finds the other variable in upcoming frame, expecting it has no value
        // - and finally the content is copied "upcoming <- found"
        // Special version of Assignment
        if (Instr->type == Instruction_Copy_To_Upcoming_Frame) {
            debugPrintf("Instruction_Copy_To_Upcoming_Frame: DST=|%s|, SRC=|%s|\n", (char *)Instr->address_dst, (char *)Instr->address_src1);

            // expecting: dst = (char *) name of variable inside upcoming frame
            // expecting: src1 =(char *) name of variable inside actual local frame

            VARIABLE *upcomingFrameVariable = NULL;
            VARIABLE *seekVariable = NULL;

            upcomingFrameVariable = findFrameVariable(upcomingLocalFrame, Instr->address_dst);
            seekVariable = findFrameVariable(actualLocalFrame, Instr->address_src1);
            if(seekVariable == NULL) seekVariable = findFrameVariable(globalFrame, Instr->address_src1);
            if(seekVariable == NULL) return 99;


            switch (upcomingFrameVariable->type) {

                case TYPE_INT: upcomingFrameVariable->value.intValue = seekVariable->value.intValue; break;
                case TYPE_DOUBLE: upcomingFrameVariable->value.doubleValue = seekVariable->value.doubleValue; break;
                case TYPE_STRING:   ;
                    char *temp = malloc(sizeof(char) * (int)strlen(seekVariable->value.stringValue) + 1 );
                    strcpy(temp, seekVariable->value.stringValue);
                    upcomingFrameVariable->value.stringValue = temp;
                    break;
                default:break;
            }
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        // Pushing upcoming local frame into stack of local frames
        // THINGS TO KEEP: global frame, stack of local frames
        if (Instr->type == Instruction_CallFunction) { debugPrintf("Instruction_CallFunction\n");

            if(upcomingLocalFrame == NULL) upcomingLocalFrame = createFrame();
            pushFrameToStack(stackOfLocalFrames, upcomingLocalFrame);

            // HERE COMES THE FUCKING RECURSION
            interpretRetVal = Interpret( (tDLList *)Instr->address_dst, globalFrame, stackOfLocalFrames, Instr->address_src1 );
            if ( interpretRetVal != 0 ) {
                debugPrintf("Previous instance of interpret has failed. #CallFunctionError\n");
                return interpretRetVal;
            }

            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        if (Instr->type == Instruction_ReturnFunction) {    debugPrintf("Instruction_ReturnFunction\n");

            // This instruction gets one parameter - name of return value, in DST
            // let's find it!

            //but first, let me pop the stack, in order to work with 3 frames... i really wanna find it

            char *seekName = stringConcat("#", returnValue);
            // in this moment we have #function
            VARIABLE *variableFromPoppedFrame = findFrameVariable(stackOfLocalFrames->arr->data.localFrame, seekName);
            if (variableFromPoppedFrame == NULL) exit(42);

            int poppedIntValue = 0;
            double poppedDoubleValue = 0.0;
            char *poppedStringValue = NULL;

            switch (variableFromPoppedFrame->type){
                case TYPE_INT:  ;
                    poppedIntValue = variableFromPoppedFrame->value.intValue;
                    break;
                case TYPE_DOUBLE:   ;
                    poppedDoubleValue = variableFromPoppedFrame->value.doubleValue;
                    break;
                case TYPE_STRING:   ;
                    poppedStringValue = malloc(sizeof(char) * strlen(variableFromPoppedFrame->value.stringValue) +1);
                    strcpy(poppedStringValue, variableFromPoppedFrame->value.stringValue);
                    break;
                default:;
            }


            stackPop(stackOfLocalFrames);   // removal of top-local-frame

            // Executing returning value to a variable
            VARIABLE *variableFromNewTopFrame = findFrameVariable(stackOfLocalFrames->arr->data.localFrame, seekName);


            switch (variableFromNewTopFrame->type){
                case TYPE_INT:  ;
                    variableFromNewTopFrame->value.intValue = poppedIntValue;
                    break;
                case TYPE_DOUBLE:   ;
                    variableFromNewTopFrame->value.doubleValue = poppedDoubleValue;
                    break;
                case TYPE_STRING:   ;
                    variableFromNewTopFrame->value.stringValue = poppedStringValue;
                    break;
                default:;
            }


            debugPrintf("volam successora!\n");
            ListSuccessor(InstructionList);

            if (InstructionList->Act == NULL) debugPrintf("vykolajil som sa!\n");
            continue; // Jump to next instruction
        }

        // DST = BOOL, SRC1 = TRUE, SRC2 = FALSE
        if (Instr->type == Instruction_IF) {    debugPrintf("Instruction_IF\n");

            VARIABLE *booleanValue;
            if (actualLocalFrame != NULL)
                booleanValue = findFrameVariable(actualLocalFrame, Instr->address_dst);
            else
                booleanValue = findFrameVariable(globalFrame, Instr->address_dst);

            if (booleanValue->value.intValue == 1) {
                interpretRetVal = Interpret( (tDLList *)Instr->address_src1, globalFrame, stackOfLocalFrames, NULL );
                if ( interpretRetVal != 0 ) {
                    debugPrintf("Previous instance of interpret has failed. #IfTrueErr\n");
                    return interpretRetVal;
                }
            }
            else if (booleanValue->value.intValue == 0) {
                interpretRetVal = Interpret( (tDLList *)Instr->address_src2, globalFrame, stackOfLocalFrames, NULL );
                if ( interpretRetVal != 0 ) {
                    debugPrintf("Previous instance of interpret has failed. #IfFalseErr\n");
                    return interpretRetVal;
                }
            }
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        // DST = BOOL, SRC1 = EXPR, SRC2 = CYCLE
        if (Instr->type == Instruction_WHILE) { debugPrintf("Instruction_WHILE\n");

            //----- STEP 1: Calling recursion for ExpressionList
            interpretRetVal = Interpret( (tDLList *)Instr->address_src1, globalFrame, stackOfLocalFrames, NULL );
            if ( interpretRetVal != 0 ) {
                debugPrintf("Previous instance of interpret has failed. #WhileEvalErr_1st\n");
                return interpretRetVal;
            }

            //----- STEP 2: Getting value from bool Value (calculated by previous list)
            VARIABLE *booleanValue;
            if (actualLocalFrame != NULL)
                booleanValue = findFrameVariable(actualLocalFrame, Instr->address_dst);
            else {
                booleanValue = findFrameVariable(globalFrame, Instr->address_dst);
            }

            //----- STEP 3: This Monster
            while (booleanValue->value.intValue == 1) {

                // Execute Cycle tape
                interpretRetVal = Interpret( (tDLList *)Instr->address_src2, globalFrame, stackOfLocalFrames, NULL );
                if ( interpretRetVal != 0 ) {
                    debugPrintf("Previous instance of interpret has failed. #WhileCycleErr\n");
                    return interpretRetVal;
                }

                // Set new values
                interpretRetVal = Interpret( (tDLList *)Instr->address_src1, globalFrame, stackOfLocalFrames, NULL );
                if ( interpretRetVal != 0 ) {
                    debugPrintf("Previous instance of interpret has failed. #WhileEvalErr_n\n");
                    return interpretRetVal;
                }

                if (actualLocalFrame != NULL)
                    booleanValue = findFrameVariable(actualLocalFrame, Instr->address_dst);
                else
                    booleanValue = findFrameVariable(globalFrame, Instr->address_dst);
            }

            //----- STEP 4: Cycle is done
            ListSuccessor(InstructionList);
            continue; // Jump to next instruction
        }

        debugPrintf("OTHER: ");
        //--- Special instructions are captured, now we will execute the rest ---

        // EXECUTING OTHER INSTRUCTIONS, REPOINTING REQUIRED
        VARIABLE *dst = NULL;
        VARIABLE *src1 = NULL;
        VARIABLE *src2 = NULL;



        if ( actualLocalFrame!= NULL ) {

            if ( actualLocalFrame->First != NULL ) {    //existing non-empty local frame
                dst = findFrameVariable(actualLocalFrame, Instr->address_dst);
                src1 = findFrameVariable(actualLocalFrame, Instr->address_src1);
                src2 = findFrameVariable(actualLocalFrame, Instr->address_src2);

            }

            // TODO: check if this is required
            char *tempDstName = NULL, *tempSrc1Name = NULL, *tempSrc2Name = NULL;

            if (Instr->address_dst != NULL) {
                tempDstName = malloc(sizeof(char)*strlen((char *)Instr->address_dst)+1);
                tempDstName[0] = '\0';
                strcpy(tempDstName, (char *)Instr->address_dst);
            }
            if (Instr->address_src1 != NULL) {
                tempSrc1Name = malloc(sizeof(char)*strlen((char *)Instr->address_src1)+1);
                tempSrc1Name[0] = '\0';
                strcpy(tempSrc1Name, (char *)Instr->address_src1);
            }
            if (Instr->address_src2 != NULL) {
                tempSrc2Name = malloc(sizeof(char)*strlen((char *)Instr->address_src2)+1);
                tempSrc2Name[0] = '\0';
                strcpy(tempSrc2Name, (char *)Instr->address_src2);
            }

            // SEARCHING IN GLOBAL FRAME: PASS ONE MORE INFORMATION - NAME OF CLASS
            //if the varaible was not found and the pointer in the instruction is not null
            //without this check you would ty to find a variable even if you dont want to
            if (dst == NULL && Instr->address_dst != NULL) {
                if ( strcmp(returnValue, "Main.run") == 0 ) {
                    if (tempDstName != NULL)
                        Instr->address_dst = stringConcat("Main.", tempDstName);
                }
                dst = findFrameVariable(globalFrame, Instr->address_dst);
            }
            if (src1 == NULL && Instr->address_src1 != NULL) {
                if ( strcmp(returnValue, "Main.run") == 0 ) {
                    if (tempSrc1Name != NULL)
                        Instr->address_src1 = stringConcat("Main.", tempSrc1Name);
                }
                src1 = findFrameVariable(globalFrame, Instr->address_src1);
            }
            if (src2 == NULL && Instr->address_src2 != NULL) {
                if ( strcmp(returnValue, "Main.run") == 0 ) {
                    if (tempSrc2Name != NULL)
                        Instr->address_src2 = stringConcat("Main.", tempSrc2Name);
                }
                src2 = findFrameVariable(globalFrame, Instr->address_src2);
            }

        }
        else {  // not existing top-of-stack local frame
            if (globalFrame->First != NULL) {
                // SEARCHING IN GLOBAL FRAME: PASS ONE MORE INFORMATION - NAME OF CLASS
                dst = findFrameVariable(globalFrame, Instr->address_dst);
                src1 = findFrameVariable(globalFrame, Instr->address_src1);
                src2 = findFrameVariable(globalFrame, Instr->address_src2);
            }
        }

        //debugPrintf("wait a minute. do i have the function name?\n");
        //if (returnValue != NULL) debugPrintf("YES I DO! : |%s|\n", returnValue);

        //debugPrintf("Look what i found: |%s|", dst->name);

        debugPrintf("\n\n\n");

        /*
        if ( Instr->type == Instruction_Function_readString ) {
            debugPrintf("----------------FATAL ERROR-------------------");
            debugPrintf("\nTo make all things clear, the interpret tries to find a variable named 'a'\n");
            debugPrintf("Not knowing, that there is such variable. but 'Main.a' - full ID... \n");
            debugPrintf("TODO: remake the search function? \n");
            debugPrintf("TODO: remake the frame-filler function? What if only for global frame? (necessity for full ID)\n");
            exit(42);
        }
         */

        switch (Instr->type) {
            case Instruction_Assign:    // expecting DST & SRC variable name
                ;
                if ( dst ==NULL || src1 == NULL || src2 != NULL ){
                    //TODO: free?
                    return 99;
                }
                debugPrintf("Instruction_Assign\n");
                executeInstructionAssign(dst, src1);
                break;

            case Instruction_Addition:
            case Instruction_Subtraction:
            case Instruction_Multiply:
            case Instruction_Divide:
                ;
                if ( dst ==NULL || src1 == NULL || src2 == NULL ){
                    //TODO: dst, src1 or src2 not found
                    //free resources
                    return 99;
                }

                int mathRetValue = executeInstructionMathOperation(Instr->type, dst, src1, src2);
                if ( mathRetValue != 0 ){
                    debugPrintf("Math operation failed.\n");
                    return mathRetValue;
                }
                break;

            case Instruction_Bool_Equals:
            case Instruction_Bool_EqualsNot:
            case Instruction_Bool_More:
            case Instruction_Bool_Less:
            case Instruction_Bool_MoreEqual:
            case Instruction_Bool_LessEqual:

                if ( dst ==NULL || src1 == NULL || src2 == NULL ) return 99;
                int evalRetValue = executeInstructionExpressionEvaluation(Instr->type, dst, src1, src2);
                if ( evalRetValue != 0 ){
                    debugPrintf("Expression evaluation failed.\n");
                    return mathRetValue;
                }
                break;

            case Instruction_Function_readInt:
            case Instruction_Function_readDouble:
            case Instruction_Function_readString:
            case Instruction_Function_Print:
            case Instruction_Function_Length:
            case Instruction_Function_Compare:
            case Instruction_Function_Find:
            case Instruction_Function_Sort:
                ;

                debugPrintf("Executing built-in function: |%d|\n", Instr->type, dst, src1, src2);
                int funRetValue = executeInstructionBuiltInFunction(Instr->type, dst, src1, src2);
                if (funRetValue != 0){
                    debugPrintf("Built-in function failed.\n");
                    return mathRetValue;
                }
                break;

            default:
                ;
                debugPrintf("!!!!! !!!!! TRYING TO HANDLE INSTRUCTION WITHOUT SOLUTION !!!!! !!!!!\n");
                break;
        }

        // break condition
        if (InstructionList->Last->element.data.instr == Instr) break;
        ListSuccessor(InstructionList);
    } // end of the big cycle

    return 0; // I had no idea what have I done
}

//...

int executeInstructionMathOperation(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2) {

    switch ( instrType ) {

    case Instruction_Addition:  debugPrintf("Instruction_Addition\n");
        ;
        if (src1->type==TYPE_STRING && src2->type==TYPE_STRING) {
            // Concatenate strings, withtout casting

            //tell interpret the execution has failed
            if (dst->type!=TYPE_STRING) {
                debugPrintf("Wrong destination format.\n");
                return 99;
            }

            dst->value.stringValue = stringConcat(src1->value.stringValue, src2->value.stringValue);
            return 0; // end instruction
        }
        if ((src1->type==TYPE_STRING && src2->type!=TYPE_STRING) || (src1->type!=TYPE_STRING && src2->type==TYPE_STRING)) {
            // Concatenate strings, with casting

            //tell interpret the execution has failed
            if (dst->type!=TYPE_STRING) {
                debugPrintf("Wrong destination format.\n");
                return 99;
            }

            char tempString[100];
            char *typeCastSrc = NULL;

            if ( src1->type!=TYPE_STRING ){
                // Switch data type of src1 to string
                if ( src1->type==TYPE_INT ) {
                    sprintf(tempString,"%d", src1->value.intValue);
                    int len = (int)strlen(tempString);
                    typeCastSrc = malloc( sizeof(char)*len+1 );
                    strcpy(typeCastSrc, tempString);
                }
                else if ( src1->type==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", src1->value.doubleValue);
                    int len = (int)strlen(tempString) +1;
                    typeCastSrc = malloc( sizeof(char)*len );
                    strcpy(typeCastSrc, tempString);
                }
            }   //end of "type casting" of SRC1 into 'typeCastSrc'

            if ( src2->type!=TYPE_STRING ) {
                // Switch data type of src2 to string
                if ( src2->type==TYPE_INT ) {
                    sprintf(tempString,"%d", src2->value.intValue);
                    int len = (int)strlen(tempString) +1;
                    typeCastSrc = malloc( sizeof(char)*len );
                    strcpy(typeCastSrc, tempString);
                }
                else if ( src2->type==TYPE_DOUBLE ) {
                    sprintf(tempString,"%g", src2->value.doubleValue);
                    int len = (int)strlen(tempString) +1;
                    typeCastSrc = malloc( sizeof(char)*len );
                    strcpy(typeCastSrc, tempString);
                }
            }   //end of "type casting" of SRC2 into 'typeCastSrc'

            // Performing concatenation
            if ( src1->type!=TYPE_STRING )
                dst->value.stringValue = stringConcat(typeCastSrc, src2->value.stringValue);
            else
                dst->value.stringValue = stringConcat(src1->value.stringValue, typeCastSrc);

            return 0; // end instruction
        }
        switch ( src1->type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2->type==TYPE_INT) dst->value.intValue = src1->value.intValue + src2->value.intValue;
                else {  // SRC2 is double
                    if (dst->type==TYPE_INT){ // DST is int
                        dst->type = TYPE_DOUBLE;
                        dst->value.doubleValue = (double)dst->value.intValue;
                    }
                    dst->value.doubleValue = (double)src1->value.intValue + src2->value.doubleValue;
                }
            return 0; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dst->type==TYPE_INT) {
                    dst->type = TYPE_DOUBLE;  // remember the type cast
                    dst->value.doubleValue = (double)dst->value.intValue;
                }
                if (src2->type==TYPE_DOUBLE)
                    dst->value.doubleValue = src1->value.doubleValue + src2->value.doubleValue;
                else
                    dst->value.doubleValue = src1->value.doubleValue + (double)src2->value.intValue;

            return 0; // end instruction

            default: ;
        }
    break;

    case Instruction_Subtraction:   debugPrintf("Instruction_Subtraction\n");
        ;
        if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
            return 10; // cannot subtract strings
        switch ( src1->type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2->type==TYPE_INT) dst->value.intValue = src1->value.intValue - src2->value.intValue;
                else {  // SRC2 is double
                    if (dst->type==TYPE_INT){ // DST is int
                        dst->type = TYPE_DOUBLE;  // remember the type cast
                        dst->value.doubleValue = (double)dst->value.intValue;
                    }
                    dst->value.doubleValue = (double)src1->value.intValue - src2->value.doubleValue;
                }
            return 0; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dst->type==TYPE_INT) {
                    dst->type = TYPE_DOUBLE;  // remember the type cast
                    dst->value.doubleValue = (double)dst->value.intValue;
                }
                if (src2->type==TYPE_DOUBLE) dst->value.doubleValue = src1->value.doubleValue - src2->value.doubleValue;
                else dst->value.doubleValue = src1->value.doubleValue - (double)src2->value.intValue;
            return 0; // end instruction

            default: ;
        }
    break;

    case Instruction_Multiply:  debugPrintf("Instruction_Multiply\n");
        ;
        if ( src1->type == TYPE_STRING || src2->type == TYPE_STRING)
            return 10;  // cannot multiply strings
        switch ( src1->type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if (src2->type==TYPE_INT) dst->value.intValue = src1->value.intValue * src2->value.intValue;
                else {  // SRC2 is double
                    if (dst->type==TYPE_INT){ // DST is int
                        dst->type = TYPE_DOUBLE;
                        dst->value.doubleValue = (double)dst->value.intValue;
                    }
                    dst->value.doubleValue = (double)src1->value.intValue * src2->value.doubleValue;
                }
            return 0; // end instruction

            case TYPE_DOUBLE:
                ;
                if (dst->type==TYPE_INT) {
                    dst->type = TYPE_DOUBLE;
                    dst->value.doubleValue = (double)dst->value.intValue;
                }
                if (src2->type==TYPE_DOUBLE) dst->value.doubleValue = src1->value.doubleValue * src2->value.doubleValue;
                else dst->value.doubleValue = src1->value.doubleValue * (double)src2->value.intValue;
            return 0; // end instruction

            default: ;
        }
    break;

    case Instruction_Divide:    debugPrintf("Instruction_Divide\n");
        ;
        if ( src1->type == TYPE_STRING || src2->type == TYPE_STRING )
            return 10; // cannot divide strings

        switch ( src1->type ) {   // both are Double or Ints

            case TYPE_INT:  // SRC1 is int
                ; // SRC2 is int
                if ( src2->type == TYPE_INT ) {
                    if ( src2->value.intValue == 0 ) return 9; // dividing by zero
                    dst->value.intValue = src1->value.intValue / src2->value.intValue;
                }
                else {  // SRC2 is double
                    if ( dst->type == TYPE_INT ){ // DST is int
                        dst->type = TYPE_DOUBLE;
                        dst->value.doubleValue = (double)dst->value.intValue;
                    }
                    if ( src2->value.doubleValue == 0.0) return 9; // dividing by zero
                    dst->value.doubleValue = (double)src1->value.intValue / src2->value.doubleValue;
                }
            return 0; // end instruction

            case TYPE_DOUBLE:
                ;
                if ( dst->type == TYPE_INT ) {
                    dst->type = TYPE_DOUBLE;
                    dst->value.doubleValue = (double)dst->value.intValue;
                }
                if ( src2->type == TYPE_DOUBLE ) {
                    if ( src2->value.doubleValue == 0.0 ) return 9; // dividing by zero
                    dst->value.doubleValue = src1->value.doubleValue / src2->value.doubleValue;
                }
                else {
                    if ( src2->value.intValue == 0 ) return 9; // dividing by zero
                    dst->value.doubleValue = src1->value.doubleValue / (double)src2->value.intValue;
                }
            return 0; // end instruction

            default: ;
        }
    break;

    default: ;
    }   // end if Math Instructions switch

    return 0;
}   // end of Executing

//...

int executeInstructionExpressionEvaluation(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2) {

    switch ( instrType ) {
        case Instruction_Bool_Equals:   // if ( src1 == src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_Equals\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int == src2->int
                        if (src1->value.intValue == src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int == src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue == src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double == src2->double
                        if (src1->value.doubleValue == src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double == src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue == src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }

            break;
        case Instruction_Bool_EqualsNot:    // if ( src1 != src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_EqualsNot\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int != src2->int
                        if (src1->value.intValue != src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int != src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue != src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double != src2->double
                        if (src1->value.doubleValue == src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double != src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue == src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }

            break;
        case Instruction_Bool_More: // if ( src1 > src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_More\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int > src2->int
                        if (src1->value.intValue > src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int > src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue > src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double > src2->double
                        if (src1->value.doubleValue > src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double > src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue > src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }
            break;
        case Instruction_Bool_Less: // if ( src1 < src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_Less\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int < src2->int
                        if (src1->value.intValue < src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int < src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue < src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double < src2->double
                        if (src1->value.doubleValue < src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double < src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue < src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }
            break;
        case Instruction_Bool_MoreEqual:    // if ( src1 >= src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_MoreEqual\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int >= src2->int
                        if (src1->value.intValue >= src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int >= src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue >= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double >= src2->double
                        if (src1->value.doubleValue >= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double >= src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue >= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }
            break;
        case Instruction_Bool_LessEqual:    // if ( src1 <= src2 ) dst=TRUE else dst=FALSE;
            debugPrintf("Instruction_Bool_LessEqual\n");
            ;
            if (src1->type == TYPE_STRING || src2->type == TYPE_STRING)
                return 10;

            switch (src1->type){
                case TYPE_INT:
                    ;
                    if (src2->type==TYPE_INT){ // src1->int <= src2->int
                        if (src1->value.intValue <= src2->value.intValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_DOUBLE) { // src1->int <= src2->double
                        src1->value.doubleValue = (double) src1->value.intValue;
                        if (src1->value.doubleValue <= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                case TYPE_DOUBLE:
                    ;
                    if (src2->type==TYPE_DOUBLE){ // src1->double >= src2->double
                        if (src1->value.doubleValue <= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;

                    } else if (src2->type==TYPE_INT) { // src1->double >= src2->int
                        src2->value.doubleValue = (double) src2->value.intValue;
                        if (src1->value.doubleValue <= src2->value.doubleValue) dst->value.intValue = 1;
                        else dst->value.intValue = 0;
                    } else {
                        // error: unsupported format
                        return 10;
                    }
                    break;

                default: ;
            }
            break;

        default: ;
    }

    return 0;
}

//...

int executeInstructionAssign(VARIABLE *dst, VARIABLE *src) {
    int type_dst = dst->type;
    int type_src = src->type;

    switch (type_dst) {

        case TYPE_INT:
            if (type_src==TYPE_INT) dst->value.intValue = src->value.intValue;
            else return 6; // Incompatibile assign
            break;

        case TYPE_DOUBLE:
            if (type_src==TYPE_DOUBLE) { // Correct assignemt
                dst->value.doubleValue = src->value.doubleValue;
            }
            else if (type_src==TYPE_INT) { // Type cast INT->DOUBLE & assign
                dst->value.doubleValue = (double)src->value.intValue;
            }
            else return 6; // Incompatibile assign
            break;

        case TYPE_STRING:
            if (type_src==TYPE_STRING) dst->value.stringValue = src->value.stringValue;
            else return 6;
            break;

        default: ;
    }
    return 0;
}   // end of Assign instruction

//...

int executeInstructionBuiltInFunction(INSTRUCTION_TYPE instrType, VARIABLE *dst, VARIABLE *src1, VARIABLE *src2) {

    switch (instrType) {

        case Instruction_Function_readInt:  debugPrintf("Instruction_Function_readInt\n");
            ;
            if (dst == NULL || src1 != NULL || src2 != NULL) return 99;
            // int ifj16_readInt();
            dst->value.intValue = ifj16_readInt();
            break;
        case Instruction_Function_readDouble:   debugPrintf("Instruction_Function_readDouble\n");
            ;
            if (dst == NULL || src1 != NULL || src2 != NULL) return 99;
            // double ifj16_readDouble();
            dst->value.doubleValue = ifj16_readDouble();
            break;
        case Instruction_Function_readString:   debugPrintf("Instruction_Function_readString\n");
            ;
            if (dst == NULL || src1 != NULL || src2 != NULL) return 99;
            // char *ifj16_readString();
            dst->value.stringValue = ifj16_readString();
            break;
        case Instruction_Function_Print:    debugPrintf("Instruction_Function_Print\n");
            ;
            if (dst == NULL || src1 != NULL || src2 != NULL) return 99;
            // void ifj16_print(char *s);
            ifj16_print(dst->value.stringValue);
            break;
        case Instruction_Function_Length:   debugPrintf("Instruction_Function_Length\n");
            ;
            if (dst == NULL || src1 == NULL || src2 != NULL) return 99;
            // int ifj16_length(char *);
            dst->value.intValue = ifj16_length(src1->value.stringValue);
            break;
        case Instruction_Function_Compare:  debugPrintf("Instruction_Function_Compare\n");
            ;
            if (dst == NULL || src1 == NULL || src2 == NULL) return 99;
            // int ifj16_compare(char *, char *);
            dst->value.intValue = ifj16_compare(src1->value.stringValue, src2->value.stringValue);
            break;
        case Instruction_Function_Find: debugPrintf("Instruction_Function_Find\n");
            ;
            if (dst == NULL || src1 == NULL || src2 == NULL) return 99;
            // int ifj16_find(char *, char *);
            dst->value.intValue = ifj16_find(src1->value.stringValue, src2->value.stringValue);
            break;
        case Instruction_Function_Sort: debugPrintf("Instruction_Function_Sort\n");
            ;
            if (dst == NULL || src1 == NULL || src2 != NULL) return 99;
            // char *ifj16_sort(char *s);
            dst->value.stringValue = ifj16_sort(src1->value.stringValue);
            break;

        default:;   // char *ifj16_substr(char *, int, int); <---- YET TO HANDLE
    }
    return 0;
}

char* getClassNameWithDotFromFullIdentifier(char *fullIdentifier) {
    int find = ifj16_find(fullIdentifier, ".");

    if(find == -1) {
        //todo change
        exit(99);
    }


    char* className = ifj16_substr(fullIdentifier, 0, find);

    return stringConcat(className, ".");
}
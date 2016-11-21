//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_DOUBLELINKEDLIST_H
#define IFJ_DOUBLELINKEDLIST_H

#include "BasicStructures.h"
#include "LexicalAnalyzer.h"

typedef enum LIST_ELEMENT_TYPE {
    LIST_ELEMENT_TYPE_TOKEN,
    LIST_ELEMENT_TYPE_FUNCTION_PARAMETER,
    LIST_ELEMENT_TYPE_INSTRUCTION,
    LIST_ELEMENT_TYPE_FRAME_ELEMENT
} LIST_ELEMENT_TYPE;

typedef union LIST_ELEMENT_DATA {
    FUNCTION_PARAMETER *parameter;
    TOKEN *token;
    struct sINSTRUCTION *instr;
    VARIABLE *variable;
    FRAME_ELEMENT *framevar;
}LIST_ELEMENT_DATA;

typedef struct LIST_ELEMENT{
    enum LIST_ELEMENT_TYPE type;
    union LIST_ELEMENT_DATA data;
}LIST_ELEMENT;

typedef struct tDLElem {
    struct LIST_ELEMENT element;
    struct tDLElem *lptr;
    struct tDLElem *rptr;
} *tDLElemPtr;

typedef struct tDLListStruct{
    tDLElemPtr First;
    tDLElemPtr Act;
    tDLElemPtr Last;
} tDLList;

void ListInit (tDLList *);
void ListInsertLast (tDLList *, struct LIST_ELEMENT);
void ListFirst (tDLList *);
void ListElementCopy (tDLList *, struct LIST_ELEMENT *);
void ListActualize (tDLList *, struct LIST_ELEMENT);
void ListPredcessor (tDLList *);
void ListSuccessor (tDLList *);
void DisposeList (tDLList *);
int DLActive (tDLList *);

// Maybe useless
void InsertFirst (tDLList *, struct LIST_ELEMENT);
void DLLast (tDLList *);
void DLCopyFirst (tDLList *, struct LIST_ELEMENT *);
void DLCopyLast (tDLList *, struct LIST_ELEMENT *);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, struct LIST_ELEMENT);
void DLPreInsert (tDLList *, struct LIST_ELEMENT);

#endif //IFJ_DOUBLELINKEDLIST_H

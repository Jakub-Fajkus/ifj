//
// Created by Jakub Fajkus on 14.11.16.
//

#ifndef IFJ_DOUBLELINKEDLIST_H
#define IFJ_DOUBLELINKEDLIST_H

#include "BasicStructures.h"
#include "LexicalAnalyzer.h"
#include "Interpret.h"

typedef enum LIST_ELEMENT_TYPE {
    LIST_ELEMENT_TYPE_TOKEN,
    LIST_ELEMENT_TYPE_FUNCTION_PARAMETER,
    LIST_ELEMENT_TYPE_INSTRUCTION,  // for instructions
    LIST_ELEMENT_TYPE_FRAME_ELEMENT
} LIST_ELEMENT_TYPE;

typedef union LIST_ELEMENT_DATA {
    FUNCTION_PARAMETER *parameter;
    TOKEN *token;
    INSTRUCTION *instr;
    VARIABLE *var;
    FRAME_ELEMENT *framevar;
}LIST_ELEMENT_DATA;

typedef struct LIST_ELEMENT{
    enum LIST_ELEMENT_TYPE type;
    union LIST_ELEMENT_DATA data;
}LIST_ELEMENT;

typedef struct tDLElem {            /* prvek dvousměrně vázaného seznamu */
    struct LIST_ELEMENT element;   /* užitečná data */
    struct tDLElem *lptr;         /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, struct LIST_ELEMENT);
void DLInsertLast(tDLList *, struct LIST_ELEMENT);
void DLFirst (tDLList *);
void DLLast (tDLList *);
void DLCopyFirst (tDLList *, struct LIST_ELEMENT *);
void DLCopyLast (tDLList *, struct LIST_ELEMENT *);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, struct LIST_ELEMENT);
void DLPreInsert (tDLList *, struct LIST_ELEMENT);
void DLCopy (tDLList *, struct LIST_ELEMENT *);
void DLActualize (tDLList *, struct LIST_ELEMENT);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);

#endif //IFJ_DOUBLELINKEDLIST_H

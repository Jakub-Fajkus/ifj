#ifndef	_IAL_H_
#define	_IAL_H_

#include <stdbool.h>

#define TRUE 1
#define FALSE 0

//************** SYMBOL TABLE **************


typedef enum{
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
} DATA_TYPE;

typedef struct {
    char* name;
    DATA_TYPE type;
} FUNCTION_PARAMETER;

typedef struct tDLElem {          /* prvek dvousměrně vázaného seznamu */
    FUNCTION_PARAMETER data;      /* užitečná data */
    struct tDLElem *lptr;         /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

typedef enum{
    TREE_NODE_VARIABLE,
    TREE_NODE_FUNCTION,
    TREE_NODE_FUNCTION_CALL,
} TREE_NODE_DATA_TYPE;


typedef union {
    int intValue;
    double doubleValue;
    char *stringValue;
} VARIABLE_VALUE;

typedef struct {
    char *name; //name of the variable
    DATA_TYPE type; //type of the variable
    VARIABLE_VALUE value; //value of the variable
    unsigned int usages; //number of usages of the variable
    bool initialized; //whether the variable ahs been initialized(has value)
} SYMBOL_TABLE_VARIABLE;

typedef struct {
    char *name; //name of the function
    DATA_TYPE type; //return type of the function
    unsigned int usages; //number of usages of the function
    tDLList *parameters;
} SYMBOL_TABLE_FUNCTION;

typedef struct {
    SYMBOL_TABLE_FUNCTION *calledFunction; //pointer to the global symbol table
    VARIABLE_VALUE value; //value returned by the function
    struct SYMBOL_TABLE_NODE *symbolTable; //local symbol table for the function call
} SYMBOL_TABLE_FUNCTION_CALL;

//the symbol table can contain 3 types of nodes
typedef union{
    SYMBOL_TABLE_FUNCTION *function; //function node - for function definition in class
    SYMBOL_TABLE_VARIABLE *variable; //variable node - for variable definition in class or in a function
    SYMBOL_TABLE_FUNCTION_CALL *functionCall; //functionCall node - when there is a function called in an other function - it has a pointer to a new local symbol table
} SYMBOL_TABLE_ITEM;

typedef struct {
    SYMBOL_TABLE_ITEM *item;
    TREE_NODE_DATA_TYPE type;
} TREE_NODE_DATA;


/* prototypy jednotlivých funkcí */
void DLInitList (tDLList *);
void DLDisposeList (tDLList *);
void DLInsertFirst (tDLList *, FUNCTION_PARAMETER);
void DLInsertLast(tDLList *, FUNCTION_PARAMETER);
void DLFirst (tDLList *);
void DLLast (tDLList *);
void DLCopyFirst (tDLList *, FUNCTION_PARAMETER *);
void DLCopyLast (tDLList *, FUNCTION_PARAMETER *);
void DLDeleteFirst (tDLList *);
void DLDeleteLast (tDLList *);
void DLPostDelete (tDLList *);
void DLPreDelete (tDLList *);
void DLPostInsert (tDLList *, FUNCTION_PARAMETER);
void DLPreInsert (tDLList *, FUNCTION_PARAMETER);
void DLCopy (tDLList *, FUNCTION_PARAMETER *);
void DLActualize (tDLList *, FUNCTION_PARAMETER);
void DLSucc (tDLList *);
void DLPred (tDLList *);
int DLActive (tDLList *);


void make_delta(int *delta, unsigned char *pat, int patlen);

int boyer_moore (unsigned char *string, unsigned int stringlen, unsigned char *pat, unsigned int patlen);

char* quickSortWrapper(char *s);

void quickSort(char *arr, int left, int right);

char medianIndex(char *s);

void swap(char *arr, int left, int right);


typedef struct SYMBOL_TABLE_NODE {
    char* key;			                                         /* klíč */
    TREE_NODE_DATA * data;                                            /* užitečný obsah uzlu */
    struct SYMBOL_TABLE_NODE * lPtr;                                    /* levý podstrom */
    struct SYMBOL_TABLE_NODE * rPtr;                                   /* pravý podstrom */
} *SYMBOL_TABLE_NODEPtr;

void BSTInit   (SYMBOL_TABLE_NODEPtr *);
int BSTSearch  (SYMBOL_TABLE_NODEPtr, char*, TREE_NODE_DATA *);
void BSTInsert (SYMBOL_TABLE_NODEPtr *, char*, TREE_NODE_DATA);
void BSTDelete (SYMBOL_TABLE_NODEPtr *, char*);
void BSTDispose(SYMBOL_TABLE_NODEPtr *);

/* ADT zásobník implementovaný ve statickém poli */
typedef struct {
    SYMBOL_TABLE_FUNCTION_CALL* arr;              /* pole pro uložení hodnot */
    int top;                                /* index prvku na vrcholu zásobníku */
    int maxSize;
    int actualSize;
} tStack;

/* Hlavičky funkcí pro práci se zásobníkem. */
void stackInit ( tStack* s );
int stackEmpty ( const tStack* s );
int stackFull ( const tStack* s );
void stackTop ( const tStack* s, SYMBOL_TABLE_FUNCTION_CALL* functionCall );
void stackPop ( tStack* s );
void stackPush ( tStack* s, SYMBOL_TABLE_FUNCTION_CALL functionCall );

/**
 * Get variable from given symbol table
 *
 * @param symbolTable
 * @param name
 * @return SYMBOL_TABLE_VARIABLE|null
 */
SYMBOL_TABLE_VARIABLE* getVariableFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name);

/**
 * Get variable from local symbol table. If the variable does not exist in the local symbol table, get it from the global table.
 * If the variable is not in the global table, return null.
 * @param symbolTable
 * @param name
 * @return
 */
SYMBOL_TABLE_VARIABLE* getVariable(SYMBOL_TABLE_NODEPtr *localSymbolTable, SYMBOL_TABLE_NODEPtr *globalSymbolTable, SYMBOL_TABLE_FUNCTION *calledFunction, char* name);

void initializeSymbolTable(SYMBOL_TABLE_NODEPtr **symbolTable);

SYMBOL_TABLE_VARIABLE* createVariable(SYMBOL_TABLE_NODEPtr *symbolTable, char *name, DATA_TYPE type, VARIABLE_VALUE value1, bool initialize);

TREE_NODE_DATA createVariableData(SYMBOL_TABLE_VARIABLE *variable);
#endif
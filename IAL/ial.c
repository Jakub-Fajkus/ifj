#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ial.h"
#include "../IFJ16Lib/ifj16.h"

//************** BOYER-MOORE **************

#define ALPHABET_LEN 256

void make_delta(int *delta, unsigned char *pat, int patlen) {
    int i;
    for (i=0; i < ALPHABET_LEN; i++) {
        delta[i] = patlen; //not found
    }
    for (i=0; i < patlen-1; i++) {
        delta[pat[i]] = patlen-1 - i;
    }
}

int boyer_moore (unsigned char *string, unsigned int stringlen,unsigned char *pat, unsigned int patlen) {
    int i;
    int delta[ALPHABET_LEN];
    make_delta(delta, pat, patlen);

    // The empty pattern must be considered specially
    if (patlen == 0) {
        return 0;
    }

    i = patlen-1;
    while (i < stringlen) {
        int j = patlen-1;
        while (j >= 0 && (string[i] == pat[j])) {
            --i;
            --j;
        }
        if (j < 0) {
            return (string + i+1) - string;
        }

        i += delta[string[i]];
    }

    return -1;
}


//************** QUICK SORT **************

char* quickSortWrapper(char *s) {
    int right = (int)strlen(s);
    char *arr; /* our array for return as sorted string */
    arr = (char*) malloc(sizeof(char)*right);


    int k = 0;  /* copy elements into that */
    while(k <= right-1){
        arr[k] = s[k];
        k++;
    }

    quickSort(arr,0,right); /* call our quick sort */

    return arr;
}

void quickSort(char *arr, int left, int right){

    if(left < right){
        int border = left;
        for(int i = left+1; i < right; i++){
            if(arr[i] < arr[left]){
                swap(arr, i, ++border);
            }
        }
        swap(arr, left, border);
        quickSort(arr, left, border);
        quickSort(arr, border + 1, right);
    }
}

//todo: @DavidCzernin unused?
char medianIndex(char *s){
    char pole[3] = {s[0],s[ifj16_length(s)/2],s[ifj16_length(s)-1]};
    for (int i = 0; i < 3; ++i) {
        if(pole[i] > pole[i+1]){
            char tmp = pole[i];
            pole[i] = pole[i+1];
            pole[i+1] = tmp;
        }
    }

    return pole[1];
}

void swap(char *arr, int left, int right){
    char tmp = arr[right];
    arr[right] = arr[left];
    arr[left] = tmp;
}

//************** BINARY TREE **************


void BSTInit (SYMBOL_TABLE_NODEPtr *RootPtr) {
    *RootPtr = NULL;
}

int BSTSearch (SYMBOL_TABLE_NODEPtr RootPtr, char* K, TREE_NODE_DATA *Content)	{
    if (RootPtr == NULL) {
        return FALSE;
    }

    //the key is the root
    if (K == RootPtr->key) {
        *Content = *RootPtr->data;
        return TRUE;
    } else if (K < RootPtr->key && RootPtr->lPtr != NULL) {
        //the has lower value than the root
        return BSTSearch(RootPtr->lPtr, K, Content);
    } else if (RootPtr->rPtr != NULL){
        //the has higher value than the root
        return BSTSearch(RootPtr->rPtr, K, Content);
    } else {
        return FALSE;
    }
}

void BSTInsert (SYMBOL_TABLE_NODEPtr* RootPtr, char* K, TREE_NODE_DATA Content)	{
    if (NULL == *RootPtr) {
        SYMBOL_TABLE_NODEPtr newItem = malloc(sizeof(struct SYMBOL_TABLE_NODE));

        newItem->key = K;
        newItem->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        newItem->data->item = Content.item;
        newItem->data->type = Content.type;
        newItem->lPtr = NULL;
        newItem->rPtr = NULL;

        (*RootPtr) = newItem;
        return;
    }

    //the key is the root
    if (K == (*RootPtr)->key) {
        (*RootPtr)->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        (*RootPtr)->data->item = Content.item;
        (*RootPtr)->data->type = Content.type;
    } else if (K < (*RootPtr)->key) {
        //the has lower value than the root
        BSTInsert(&(*RootPtr)->lPtr, K, Content);
    } else {
        //the has higher value than the root
        BSTInsert(&(*RootPtr)->rPtr, K, Content);
    }
}

void ReplaceByRightmost (SYMBOL_TABLE_NODEPtr PtrReplaced, SYMBOL_TABLE_NODEPtr *RootPtr) {
    if (NULL == RootPtr) {
        return;
    }

    //if the current node has right subtree and the root node of the subtree has no right subtree - it is the rightmost
    if (NULL != (*RootPtr)->rPtr && NULL == (*RootPtr)->rPtr->rPtr) {
        PtrReplaced->data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        PtrReplaced->data->item = (*RootPtr)->rPtr->data->item;
        PtrReplaced->data->type = (*RootPtr)->rPtr->data->type;
        PtrReplaced->key = (*RootPtr)->rPtr->key;

        free((*RootPtr)->rPtr);
        (*RootPtr)->rPtr = NULL;

        //the current node has no right subtree - it it the rightmost
    } else if(NULL == (*RootPtr)->rPtr) {
        TREE_NODE_DATA *data = (TREE_NODE_DATA*)malloc(sizeof(TREE_NODE_DATA));
        data->type = (*RootPtr)->data->type;
        data->item = (*RootPtr)->data->item;

        char* key = (*RootPtr)->key;

        BSTDelete(&PtrReplaced, (*RootPtr)->key);

        PtrReplaced->data = data;
        PtrReplaced->key = key;
    } else {
        ReplaceByRightmost(PtrReplaced, &(*RootPtr)->rPtr);
    }

}

void BSTDelete (SYMBOL_TABLE_NODEPtr *RootPtr, char* K) {
    SYMBOL_TABLE_NODEPtr actualItem = *RootPtr;
    SYMBOL_TABLE_NODEPtr temp;

    //the key is the root
    if (K == (*RootPtr)->key) {
        //let the game begin

        //if the item has only right subtree
        if (NULL == (*RootPtr)->lPtr && NULL != (*RootPtr)->rPtr) {
            temp = (*RootPtr)->rPtr;
            free(*RootPtr);
            *RootPtr = temp;

            //if the item has only left subtree
        } else if (NULL != (*RootPtr)->lPtr && NULL == (*RootPtr)->rPtr) {
            temp = (*RootPtr)->lPtr;
            free(*RootPtr);
            *RootPtr = temp;

            //the item has two subtrees
        } else if(NULL != (*RootPtr)->lPtr && NULL != (*RootPtr)->rPtr){
            ReplaceByRightmost((*RootPtr), &(*RootPtr)->lPtr);
            //both subtrees are null
        } else {
            (*RootPtr) = NULL;
            free(actualItem);
        }

    } else if (K < (*RootPtr)->key && (*RootPtr)->lPtr != NULL) {
        //the has lower value than the root
        BSTDelete(&(*RootPtr)->lPtr, K);
    } else if ((*RootPtr)->rPtr != NULL){
        //the has higher value than the root
        BSTDelete(&(*RootPtr)->rPtr, K);
    }
}

void BSTDispose (SYMBOL_TABLE_NODEPtr *RootPtr) {

    if (NULL == *RootPtr) {
        return;
    }

    if (NULL != (*RootPtr)->lPtr) {
        BSTDispose(&(*RootPtr)->lPtr);
    }

    if (NULL != (*RootPtr)->rPtr) {
        BSTDispose(&(*RootPtr)->rPtr);
    }

    if(NULL == (*RootPtr)->lPtr && NULL == (*RootPtr)->rPtr) {
        free(*RootPtr);
        *RootPtr = NULL;
    }
}

//************** DOUBLE LINKED LIST **************

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->First = NULL;
    L->Last = NULL;
    L->Act = NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/

    //from the end to the beginning

    tDLElemPtr next;

    if (L->First == NULL) {
        return;
    }

    while (L->First != NULL) {
        next = L->First->rptr;
        free(L->First);
        L->First = next;
    }

    L->Act = NULL;
    L->First = NULL;
    L->Last = NULL;
}

void DLInsertFirst (tDLList *L, FUNCTION_PARAMETER parameter) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr newElem = malloc(sizeof(struct tDLElem));
    if (newElem == NULL) {
        DLError();
        return;
    }

    newElem->data = parameter;
    newElem->lptr = NULL;
    newElem->rptr = L->First;

    if (L->First != NULL) {
        L->First->lptr = newElem;
    }

    L->First = newElem;

    if (L->Last == NULL) {
        L->Last = newElem;
    }
}

void DLInsertLast(tDLList *L, FUNCTION_PARAMETER parameter) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr newElem = malloc(sizeof(struct tDLElem));
    if (newElem == NULL) {
        DLError();
        return;
    }

    newElem->data = parameter;
    newElem->rptr = NULL;
    newElem->lptr = L->Last;

    if (L->Last != NULL) {
        L->Last->rptr = newElem;
    }

    L->Last = newElem;

    if (L->First == NULL) {
        L->First = newElem;
    }
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->First;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->Last;
}

void DLCopyFirst (tDLList *L, FUNCTION_PARAMETER *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL || L->Last == NULL) {
        DLError();
        return;
    }

    *val = L->First->data;
}

void DLCopyLast (tDLList *L, FUNCTION_PARAMETER *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if (L->First == NULL || L->Last == NULL) {
        DLError();
        return;
    }

    *val = L->Last->data;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    tDLElemPtr toBeDeleted = L->First;

    if (L->First == NULL) {
        return;
    }

    if (L->Act == L->First) {
        L->Act = NULL;
    }

    //only one element in the list
    if (L->First == L->Last) {
        L->First = NULL;
        L->Last = NULL;
        // more elements
    } else {
        L->First = L->First->rptr;
        L->First->lptr = NULL;
    }

    free(toBeDeleted);
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    tDLElemPtr toBeDeleted = L->Last;

    if (L->Last == NULL) {
        return;
    }

    if (L->Act == L->Last) {
        L->Act = NULL;
    }

    //only one element in the list
    if (L->First == L->Last) {
        L->First = NULL;
        L->Last = NULL;
        // more elements
    } else {
        L->Last = L->Last->lptr;
        L->Last->rptr = NULL;
    }

    free(toBeDeleted);
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if(L->Act == NULL) {
        return;
    }

    if(L->Act->rptr == NULL) {
        return;
    }

    tDLElemPtr toBeDeleted = L->Act->rptr;
    L->Act->rptr = toBeDeleted->rptr;

    if (toBeDeleted == L->Last) {
        L->Last = L->Act;
    } else {
        toBeDeleted->rptr->lptr = L->Act;
    }

    free(toBeDeleted);
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if(L->Act == NULL) {
        return;
    }

    if(L->Act->lptr == NULL) {
        return;
    }

    tDLElemPtr toBeDeleted = L->Act->lptr;
    L->Act->lptr = toBeDeleted->lptr;

    if (toBeDeleted == L->First) {
        L->First = L->Act;
    } else {
        toBeDeleted->lptr->rptr = L->Act;
    }

    free(toBeDeleted);
}

void DLPostInsert (tDLList *L, FUNCTION_PARAMETER parameter) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act == NULL) {
        return;
    }

    tDLElemPtr newElem = malloc(sizeof(struct tDLElem));
    if (newElem == NULL) {
        DLError();
        return;
    }

    newElem->data = parameter;

    if (L->Act != L->Last) {
        L->Act->rptr->lptr = newElem;
    } else {
        L->Last = newElem;
    }

    newElem->rptr = L->Act->rptr;
    L->Act->rptr = newElem;
    newElem->lptr = L->Act;
}

void DLPreInsert (tDLList *L, FUNCTION_PARAMETER parameter) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act == NULL) {
        return;
    }

    tDLElemPtr newElem = malloc(sizeof(struct tDLElem));
    if (newElem == NULL) {
        DLError();
        return;
    }

    newElem->data = parameter;

    if (L->Act != L->First) {
        L->Act->lptr->rptr = newElem;
    } else {
        L->First = newElem;
    }

    newElem->lptr = L->Act->lptr;
    L->Act->lptr = newElem;
    newElem->rptr = L->Act;

    if(L->Act == L->First) {
    }
}

void DLCopy (tDLList *L, FUNCTION_PARAMETER *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if (L->Act == NULL) {
        DLError();
        return;
    }

    *val = L->Act->data;
}

void DLActualize (tDLList *L, FUNCTION_PARAMETER parameter) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act == NULL) {
        return;
    }

    L->Act->data = parameter;
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/

    if (L->Act == NULL) {
        return;
    }

    L->Act = L->Act->rptr;
}

void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/

    if (L->Act == NULL) {
        return;
    }

    L->Act = L->Act->lptr;

}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/

    return L->Act != NULL;
}

//************** STACK **************

void stackInit ( tStack* s ) {
    if (s == NULL) {
        printf("stack is null!");
        exit(111);
        return;
    }

    // stack is empty
    s->top = -1;

    s->maxSize = 100;
    s->actualSize = 0;
    s->arr = malloc(s->maxSize * sizeof(SYMBOL_TABLE_FUNCTION_CALL*));
}

void increaseStackSize(tStack* s) {
    s->maxSize += 100;
    s->arr = realloc(s->arr, s->maxSize * sizeof(SYMBOL_TABLE_FUNCTION_CALL*));
}

int stackEmpty ( const tStack* s ) {
    return s->top == -1;
}


void stackTop ( const tStack* s, SYMBOL_TABLE_FUNCTION_CALL* functionCall ) {
    if (s->top == -1) {
        printf("stack is empty!");
        return;
    }

    *functionCall = s->arr[s->top];
}


void stackPop ( tStack* s ) {
/*   --------
** Odstraní prvek z vrcholu zásobníku. Pro ověření, zda je zásobník prázdný,
** použijte dříve definovanou funkci stackEmpty.
**
** Vyvolání operace Pop při prázdném zásobníku je sice podezřelé a může
** signalizovat chybu v algoritmu, ve kterém je zásobník použit, ale funkci
** pro ošetření chyby zde nevolejte (můžeme zásobník ponechat prázdný).
** Spíše než volání chyby by se zde hodilo vypsat varování, což však pro
** jednoduchost neděláme.
**
*/
    if (stackEmpty(s)) {
        return;
    }

    s->top--;
    s->actualSize--;
}

int stackFull ( const tStack* s ) {
    return s->actualSize == s->maxSize;
}


void stackPush ( tStack* s, SYMBOL_TABLE_FUNCTION_CALL functionCall ) {

    if (stackFull(s)) {
        increaseStackSize(s);
    }

    //raise the index pointer
    s->top++;
    s->actualSize++;
    //save the value to the new index pointer
    s->arr[s->top] = functionCall;
}


//************** FUNCTIONS WORKING WITH SYMBOL TABLES **************

SYMBOL_TABLE_VARIABLE* getVariableFromTable(SYMBOL_TABLE_NODEPtr *symbolTable, char* name)
{
    TREE_NODE_DATA nodeData;

    if (TRUE == BSTSearch(*symbolTable, name, &nodeData)) {
        if (nodeData.type != TREE_NODE_VARIABLE) {
            printf("ERROR: getVariableFromTable: searched for variable %s, but it is not a variable", name);
            exit(1);
        }
        //found
        return nodeData.item->variable;
    } else {
//        not found
        return NULL;
    }
}

SYMBOL_TABLE_VARIABLE* getVariable(SYMBOL_TABLE_NODEPtr *localSymbolTable, SYMBOL_TABLE_NODEPtr *globalSymbolTable, SYMBOL_TABLE_FUNCTION *calledFunction, char* name)
{
    SYMBOL_TABLE_VARIABLE *variable;

    variable = getVariableFromTable(localSymbolTable, name);

    //the variable was found and is local
    if(variable != NULL) {
        return variable;
    }

    //when searching in the global table, there are 2 situations:
    //1. the searching variable is ordinary identifier(e.g. property) - in this case we want to prefix the name with class name(the class that contains the function definition!)
    //2. the searching variable is fully qualified name(e.g. class.property) - in this case we want to search for the name as it is

    //if it does not contain a dot
    if (-1 != ifj16_find(name, ".")) {
        //add class prefix to it
        char* className = ifj16_substr(calledFunction->name, 0, ifj16_find(calledFunction->name, "."));
        char *fullyQualifiedName = malloc(sizeof(char)*(strlen(name) + strlen(className) + 2)); //size of argument name + size of class + dot + \0
        strcat(fullyQualifiedName, className);
        strcat(fullyQualifiedName, ".");
        strcat(fullyQualifiedName, name);

        variable = getVariableFromTable(globalSymbolTable, fullyQualifiedName);

    } else {
        //the name is already fully qualified
        variable = getVariableFromTable(globalSymbolTable, name);
    }

    //the variable was either found(is global) or not found at all
    return variable;
}
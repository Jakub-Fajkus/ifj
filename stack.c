//
// Created by Jakub Fajkus on 14.11.16.
//

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "debug.h"

void stackInit ( struct STACK_STR* s) {
    if (s == NULL) {
        debugPrintf("stack is null!");
        exit(99);
    }

    // stack is empty
    s->top = -1;

    s->maxSize = 100;
    s->actualSize = 0;
    s->arr = malloc(s->maxSize * sizeof(struct STACK_ELEMENT));
}

void increaseStackSize(struct STACK_STR* s) {
    s->maxSize += 100;

    if (s->maxSize > STACK_SIZE_LIMIT) {
        exit(99);
    }

    s->arr = realloc(s->arr, s->maxSize * sizeof(struct STACK_ELEMENT));
}

int stackEmpty ( const struct STACK_STR* s ) {
    return s->top == -1;
}

void stackTop ( const struct STACK_STR* s, struct STACK_ELEMENT* element ) {
    if (s->top == -1) {
        //printf("stack is empty!");
        return;
    }

    *element = s->arr[s->top];
}


void stackPop ( struct STACK_STR* s ) {
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

int stackFull ( const struct STACK_STR* s ) {
    return s->actualSize == s->maxSize;
}


void stackPush ( struct STACK_STR* s, struct STACK_ELEMENT element ) {

    if (stackFull(s)) {
        increaseStackSize(s);
    }

    //raise the index pointer
    s->top++;
    s->actualSize++;
    //save the value to the new index pointer
    s->arr[s->top] = element;
}
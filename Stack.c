//
// Created by Jakub Fajkus on 14.11.16.
//

#include <ntsid.h>
#include <stdlib.h>
#include <stdio.h>
#include "Stack.h"

void stackInit ( tStack* s) {
    if (s == NULL) {
        printf("stack is null!");
        exit(99);
        return;
    }

    // stack is empty
    s->top = -1;

    s->maxSize = 100;
    s->actualSize = 0;
    s->arr = malloc(s->maxSize * sizeof(struct STACK_ELEMENT));
}

void increaseStackSize(tStack* s) {
    s->maxSize += 100;

    if (s->maxSize > STACK_SIZE_LIMIT) {
        exit(99);
    }

    s->arr = realloc(s->arr, s->maxSize * sizeof(struct STACK_ELEMENT));
}

int stackEmpty ( const tStack* s ) {
    return s->top == -1;
}

void stackTop ( const tStack* s, struct STACK_ELEMENT* element ) {
    if (s->top == -1) {
        printf("stack is empty!");
        return;
    }

    *element = s->arr[s->top];
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


void stackPush ( tStack* s, struct STACK_ELEMENT element ) {

    if (stackFull(s)) {
        increaseStackSize(s);
    }

    //raise the index pointer
    s->top++;
    s->actualSize++;
    //save the value to the new index pointer
    s->arr[s->top] = element;
}
//
// Created by Jakub Fajkus on 14.11.16.
//

#include <stdlib.h>
#include <stdio.h>
#include "DoubleLinkedList.h"

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    exit(99);
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

void DLInsertFirst (tDLList *L, struct LIST_ELEMENT parameter) {
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

    newElem->element = parameter;
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

void DLInsertLast(tDLList *L, struct LIST_ELEMENT parameter) {
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

    newElem->element = parameter;
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

void DLCopyFirst (tDLList *L, struct LIST_ELEMENT *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL || L->Last == NULL) {
        DLError();
        return;
    }

    *val = L->First->element;
}

void DLCopyLast (tDLList *L, struct LIST_ELEMENT *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

    if (L->First == NULL || L->Last == NULL) {
        DLError();
        return;
    }

    *val = L->Last->element;
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

void DLPostInsert (tDLList *L, struct LIST_ELEMENT parameter) {
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

    newElem->element = parameter;

    if (L->Act != L->Last) {
        L->Act->rptr->lptr = newElem;
    } else {
        L->Last = newElem;
    }

    newElem->rptr = L->Act->rptr;
    L->Act->rptr = newElem;
    newElem->lptr = L->Act;
}

void DLPreInsert (tDLList *L, struct LIST_ELEMENT parameter) {
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

    newElem->element = parameter;

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

void DLCopy (tDLList *L, struct LIST_ELEMENT *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if (L->Act == NULL) {
        DLError();
        return;
    }

    *val = L->Act->element;
}

void DLActualize (tDLList *L, struct LIST_ELEMENT parameter) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act == NULL) {
        return;
    }

    L->Act->element = parameter;
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
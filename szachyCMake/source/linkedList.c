#include <stdio.h>
#include <stdlib.h>
#include "../header/linkedList.h"

element_listy* lista_dodaj(element_listy* glowa, Move s) {
    if (glowa == NULL) {
        glowa = (element_listy*)calloc(1, sizeof(element_listy));
        if (glowa == NULL) {
            return NULL;
        }
        glowa->move = s;
        glowa->nastepny = NULL;
        return glowa;
    }
    element_listy* aktualny = glowa;
    while (aktualny->nastepny != NULL) {
        aktualny = aktualny->nastepny;
    }
    aktualny->nastepny = (element_listy*)calloc(1, sizeof(element_listy));
    if (aktualny->nastepny == NULL) {
        return glowa;
    }
    aktualny->nastepny->move = s;
    aktualny->nastepny->nastepny = NULL;
    return glowa;
}

element_listy* lista_usun_ostatniego(element_listy* glowa) {
    if (glowa == NULL) {
        return NULL;
    }
    if (glowa->nastepny == NULL) {
        free(glowa);
        return NULL;
    }
    element_listy* poprzedni = NULL;
    element_listy* aktualny = glowa;
    while (aktualny->nastepny != NULL) {
        poprzedni = aktualny;
        aktualny = aktualny->nastepny;
    }
    free(aktualny);
    poprzedni->nastepny = NULL;
    return glowa;
}

element_listy* lista_usun_ity(element_listy* glowa, int indeks) {
    if (glowa == NULL) {
        return NULL;
    }
    if (indeks == 0) {
        element_listy* nastepny = glowa->nastepny;
        free(glowa);
        return nastepny;
    }
    element_listy* poprzedni = NULL;
    element_listy* aktualny = glowa;
    while (indeks > 0 && aktualny != NULL) {
        poprzedni = aktualny;
        aktualny = aktualny->nastepny;
        indeks--;
    }
    if (aktualny != NULL) {
        poprzedni->nastepny = aktualny->nastepny;
        free(aktualny);
    }
    return glowa;
}

element_listy* lista_wstaw(element_listy* glowa, int miejsce, Move move) {
    if (miejsce == 0) {
        element_listy* nowaGlowa = (element_listy*)calloc(1, sizeof(element_listy));
        if (nowaGlowa == NULL) {
            return glowa;
        }
        nowaGlowa->move = glowa->move;
        nowaGlowa->nastepny = glowa->nastepny;
        glowa->move = move;
        glowa->nastepny = nowaGlowa;
        return glowa;
    }
    element_listy* aktualny = glowa;
    element_listy* poprzedni = NULL;
    while (miejsce > 0 && aktualny != NULL) {
        poprzedni = aktualny;
        aktualny = aktualny->nastepny;
        miejsce--;
    }
    if (aktualny != NULL) {
        element_listy* nowyElement = (element_listy*)calloc(1, sizeof(element_listy));
        if (nowyElement == NULL) {
            return glowa;
        }
        nowyElement->move = move;
        nowyElement->nastepny = aktualny;
        poprzedni->nastepny = nowyElement;
    }
    return glowa;
}

void lista_zwolnij(element_listy* glowa) {
    while (glowa != NULL) {
        element_listy* nastepny = glowa->nastepny;
        free(glowa);
        glowa = nastepny;
    }
}
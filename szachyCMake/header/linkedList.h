#include <stdio.h>
#include<stdlib.h>
#include"board.h"

typedef struct element_listy
{
    ChessBoard board;
	float evaluation;
    struct element_listy* nastepny;
}element_listy;


element_listy* dodaj(element_listy* glowa, ChessBoard s) {
    if (glowa == NULL) {
        return NULL;
    }
    element_listy* poprzedni;
    element_listy* aktualny = glowa;
    while ((aktualny != NULL) && (poprzedni = aktualny) && (aktualny = aktualny->nastepny)) { ; }
    poprzedni->nastepny = (element_listy*)calloc(1, sizeof(element_listy));
    poprzedni->nastepny->board= s;
    poprzedni->nastepny->nastepny = NULL;
    return glowa;
}
element_listy* usun_ostatniego(element_listy* glowa) {
    if (glowa->nastepny == NULL) {
        free(glowa);
        return NULL;
    }
    element_listy* poprzedni;
    element_listy* aktualny = glowa;
    while (aktualny->nastepny != NULL && (poprzedni = aktualny) && (aktualny = aktualny->nastepny)) { ; }
    free(aktualny);
    poprzedni->nastepny = NULL;
    return glowa;
}
element_listy* usun_ity(element_listy* glowa, int indeks) {
    element_listy* akutalny = glowa;
    if (indeks== 0) {
        if (glowa->nastepny == NULL) {
            free(glowa);
            return NULL;
        }
        akutalny = glowa->nastepny;
        glowa->board= akutalny->board;
        glowa->nastepny = akutalny->nastepny;
        free(akutalny);
        return glowa;
    }
    element_listy* poprzedni;
    while (indeks) {
        poprzedni = akutalny;
        akutalny = akutalny->nastepny;
        indeks--;
    }
    poprzedni->nastepny = akutalny->nastepny;
    free(akutalny);
    return glowa;
}
element_listy* wstaw(element_listy* glowa, int miejsce, ChessBoard board) {
    if (miejsce == 0) {
        element_listy* nowaGlowa =(element_listy *) calloc(1, sizeof(element_listy));
        nowaGlowa->board= glowa->board;
        nowaGlowa->nastepny = glowa->nastepny;
        glowa->board= board;
        glowa->nastepny = nowaGlowa;
        return glowa;
    }
    element_listy* akutalny = glowa;
    element_listy* poprzedni;
    while (miejsce) {
        poprzedni = akutalny;
        akutalny = akutalny->nastepny;
        miejsce--;
    }
    poprzedni->nastepny = (element_listy*)calloc(1, sizeof(element_listy));
    poprzedni->nastepny->board= board;
    poprzedni->nastepny->nastepny = akutalny;
    return glowa;
}


void zwolnij(element_listy* glowa) {
    if (glowa == NULL) return;
    element_listy* nastepny = glowa;
    while (glowa->nastepny != NULL)
    {
        nastepny = glowa->nastepny;
        free(glowa);
        glowa = nastepny;
    }
    free(glowa);
}
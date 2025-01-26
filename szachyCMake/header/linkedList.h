#pragma once
#include <stdio.h>
#include<stdlib.h>

typedef struct Move {
    char xFrom, yFrom, xTo, yTo,promotionPiece;
} Move;


typedef struct element_listy
{
    Move move;
    struct element_listy* nastepny;
}element_listy;


element_listy* lista_dodaj(element_listy* glowa, Move s);
element_listy* lista_usun_ostatniego(element_listy* glowa);
element_listy* lista_usun_ity(element_listy* glowa, int indeks);
element_listy* lista_wstaw(element_listy* glowa, int miejsce, Move move);
void lista_zwolnij(element_listy* glowa);
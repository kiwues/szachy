#pragma once
#include<stdint.h>
#include"../header/board.h"

typedef struct MagicBitboard {
	uint64_t* ptr;
	uint64_t magic;
	char shift;
} MagicBitboard;

extern const MagicBitboard MGRook[64];
extern const MagicBitboard MGBishop[64];

uint64_t GetMagicMaskForRook(int square, uint64_t* mask);
uint64_t GetMagicMaskForBishop(int square, uint64_t* mask);


void FindAndPrintMagicNumbers();
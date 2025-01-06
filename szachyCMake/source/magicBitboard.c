#include<stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include<wchar.h>
#include"../header/board.h"
#include"../magic_numbers.h"

//https://www.chessprogramming.org/Looking_for_Magics


uint64_t random_uint64t() {
	uint64_t r1, r2, r3, r4;
	r1 = ((uint64_t)rand()) & 0xFFFF;
	r2 = ((uint64_t)rand()) & 0xFFFF;
	r3 = ((uint64_t)rand()) & 0xFFFF;
	r4 = ((uint64_t)rand()) & 0xFFFF;
	return r1 | (r2 << 16) | (r3 << 32) | (r4 << 48);
}

uint64_t random_uint64t_fewerbits() {
	return random_uint64t() & random_uint64t() & random_uint64t();
}

int countBits(uint64_t number) {
	int count = 0;
	for (count = 0; number; count++, number &= number - 1);
	return count;
}

const int BitTable[64] = {
  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
  58, 20, 37, 17, 36, 8
};


int pop1stBit(uint64_t* nnumber) {
	uint64_t number = *nnumber ^ (*nnumber - 1);
	unsigned int fold = (unsigned)((number & 0xffffffff) ^ (number >> 32));
	*nnumber &= (*nnumber - 1);
	return BitTable[(fold * 0x783a9b23) >> 26];
}

uint64_t indexToUint64(int index, int bits, uint64_t mask) {
	int i, j;
	uint64_t result = 0ull;
	for (i = 0; i < bits; i++) {
		j = pop1stBit(&mask);
		if (index & (1 << i)) {
			result |= 1ull << j;
		}
	}
	return result;
}

uint64_t rookMask(int sq) {
	uint64_t result = 0ull;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 6; r++) result |= (1ull << (fl + r * 8));
	for (r = rk - 1; r >= 1; r--) result |= (1ull << (fl + r * 8));
	for (f = fl + 1; f <= 6; f++) result |= (1ull << (f + rk * 8));
	for (f = fl - 1; f >= 1; f--) result |= (1ull << (f + rk * 8));
	return result;
}

uint64_t bishopMask(int sq) {
	uint64_t result = 0ull;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 6 && f <= 6; r++, f++) result |= (1ull << (f + r * 8));
	for (r = rk + 1, f = fl - 1; r <= 6 && f >= 1; r++, f--) result |= (1ull << (f + r * 8));
	for (r = rk - 1, f = fl + 1; r >= 1 && f <= 6; r--, f++) result |= (1ull << (f + r * 8));
	for (r = rk - 1, f = fl - 1; r >= 1 && f >= 1; r--, f--) result |= (1ull << (f + r * 8));
	return result;
}

uint64_t rookMoveBlock(int sq, uint64_t block) {
	uint64_t result = 0ull;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1; r <= 7; r++) {
		result |= (1ull << (fl + r * 8));
		if (block & (1ull << (fl + r * 8))) break;
	}
	for (r = rk - 1; r >= 0; r--) {
		result |= (1ull << (fl + r * 8));
		if (block & (1ull << (fl + r * 8))) break;
	}
	for (f = fl + 1; f <= 7; f++) {
		result |= (1ull << (f + rk * 8));
		if (block & (1ull << (f + rk * 8))) break;
	}
	for (f = fl - 1; f >= 0; f--) {
		result |= (1ull << (f + rk * 8));
		if (block & (1ull << (f + rk * 8))) break;
	}
	return result;
}

uint64_t bishopMoveBlock(int sq, uint64_t block) {
	uint64_t result = 0ull;
	int rk = sq / 8, fl = sq % 8, r, f;
	for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; r++, f++) {
		result |= (1ull << (f + r * 8));
		if (block & (1ull << (f + r * 8))) break;
	}
	for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; r++, f--) {
		result |= (1ull << (f + r * 8));
		if (block & (1ull << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; r--, f++) {
		result |= (1ull << (f + r * 8));
		if (block & (1ull << (f + r * 8))) break;
	}
	for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; r--, f--) {
		result |= (1ull << (f + r * 8));
		if (block & (1ull << (f + r * 8))) break;
	}
	return result;
}

int RBits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};

int BBits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};


int transform(uint64_t number, uint64_t magic, int bits) {
	return (int)((number * magic) >> (64 - bits));
}

void printAttackTableForSquare(FILE* file,int sq,int bishop, unsigned int* attackTableIndex,uint64_t *magics) {
	uint64_t mask, b[4096], a[4096],used[4096];
	int i, j, n;


	mask = bishop ? bishopMask(sq) : rookMask(sq);
	n = countBits(mask);

	for (i = 0; i < (1 << n); i++) {
		b[i] = indexToUint64(i, n, mask);
		a[i] = bishop ? bishopMoveBlock(sq, b[i]) : rookMoveBlock(sq, b[i]);
	}

	for (i = 0; i < 4096; i++, used[i] = 0ull);



	for (i = 0; i < (1 << n); i++) {
		j = transform(b[i], magics[sq], bishop ? BBits[sq] : RBits[sq]);
		used[j] = a[i];
		(*attackTableIndex)++;
	}
	for (i = 0; i < (1 << n); i++)
		fprintf(file, "  0x%llxull,\n", used[i]);
}

uint64_t findMagic(int sq, int m,int bishop) {
	uint64_t mask, b[4096], a[4096], used[4096], magic;
	int i, j, k, n, fail;


	mask = bishop ? bishopMask(sq) : rookMask(sq);
	n = countBits(mask);

	for (i = 0; i < (1 << n); i++) {
		b[i] = indexToUint64(i, n, mask);
		a[i] = bishop ? bishopMoveBlock(sq, b[i]) : rookMoveBlock(sq, b[i]);
	}
	for (k = 0; k < 100000000; k++) {
		magic = random_uint64t_fewerbits();
		if (countBits((mask * magic) & 0xFF00000000000000ULL) < 6)
			continue;
		for (i = 0; i < 4096; i++) used[i] = 0ULL;
		for (i = 0, fail = 0; !fail && i < (1 << n); i++) {
			j = transform(b[i], magic, m);
			if (used[j] == 0ull) used[j] = a[i];
			else if (used[j] != a[i]) fail = 1;
		}
		if (!fail) {
			return magic;
		}
	}
	printf("Error!\n");
	return 0ull;

}

void FindAndPrintMagicNumbers() {
	wprintf(L"Writing magic numbers...");
	FILE* file = fopen("magic_numbers.txt", "w");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	int square;
	uint64_t rookMagic[64], bishopMagic[64];
	//fwprintf(file, L"const uint64_t RMagic[64] = {\n");

	for (square = 0; square < 64; square++) {
		rookMagic[square] = findMagic(square, RBits[square], 0);
		//fwprintf(file, L"  0x%llxull,\n", rookMagic[square]);
	}
	//fwprintf(file, L"};\n\n");

	//fwprintf(file, L"const uint64_t BMagic[64] = {\n");
	for (square = 0; square < 64; square++) {
		bishopMagic[square] = findMagic(square, BBits[square], 1);
		//fwprintf(file, L"  0x%llxull,\n", bishopMagic[square]);
	}
	//fwprintf(file, L"};\n\n");

	unsigned int attackTableIndex[128] = { 0u };
	unsigned int attackTableOffset = 0u;

	fwprintf(file, L"#pragma once\n");
	fwprintf(file, L"#include<stdint.h>\n");

	fwprintf(file, L"#include\"header/magicBitboard.h\"\n");

	fwprintf(file, L"const uint64_t attackTable[] = {\n");
	for (int bishop = 0; bishop < 2; bishop++)
		for (square = 0; square < 64; square++) {
			attackTableIndex[square + 64 * bishop] = attackTableOffset;
			printAttackTableForSquare(file, square, bishop, &attackTableOffset,bishop?&bishopMagic[0] : &rookMagic[0]);
		}
	fwprintf(file, L"};\n\n");

	fwprintf(file, L"const MagicBitboard MGRook[64] = {\n");
	for (square = 0; square < 64; square++) {
		fwprintf(file, L"{ &attackTable[%u], 0x%llxull, %d},",attackTableIndex[square], rookMagic[square],RBits[square]);
		//if (square % 2 == 0) fwprintf(file, L"\n");
	}
	fwprintf(file, L"\n};\n\n");

	fwprintf(file, L"const MagicBitboard MGBishop[64] = {\n");
	for (square = 0; square < 64; square++) {
		fwprintf(file, L"{ &attackTable[%u], 0x%llxull, %d},", attackTableIndex[square+64], bishopMagic[square], BBits[square]);
		//if (square % 2 == 0) fwprintf(file, L"\n");
	}
	fwprintf(file, L"\n};\n\n");

	fclose(file);
	wprintf(L"Magic numbers saved!\n");
}


uint64_t GetMagicMaskForRook(int square, uint64_t *mask) {
	if (square / 8 != 0) {
		*mask &= ~0xffull;
	} 
	if (square / 8 != 7) {
		*mask &= ~0xff00000000000000ull;
	}
	if (square % 8 != 7) {
		*mask &= ~0x8080808080808080ull;
	}
	if (square % 8 != 0) {
		*mask &= ~0x101010101010101ull;
	}
	*mask *= MGRook[square].magic;
	*mask >>= 64 - MGRook[square].shift;
	*mask = MGRook[square].ptr[*mask];
	return *mask;
}
uint64_t GetMagicMaskForBishop(int square, uint64_t* mask) 
{
	*mask &= 0x7e7e7e7e7e7e00ull;
	*mask *= MGBishop[square].magic;
	*mask >>= 64 - MGBishop[square].shift;
	*mask = MGBishop[square].ptr[*mask];
	return *mask;
}
#include<string.h>
#include<stdint.h>
#include"../header/board.h"
#include"../header/magicBitboard.h"

void getMaskOfPiece(char piece,char x, char y, uint64_t* moveBitmask,uint64_t* captureBitmask, ChessBoard* board) {
	switch (piece & 7)
	{
	case 1://KING
	{
		if (y == 0 || (y == 1 && x == 0)) {
			*moveBitmask = 460039ull >> ((1 - y) * 8 + (1 - x));
		}
		else {
			*moveBitmask = 460039ull << ((y - 1) * 8 + (x - 1));
		}
		if (x == 7) {
			*moveBitmask &= ~72340172838076673ull;
		}
		else if (x == 0) {
			*moveBitmask &= ~9259542123273814144ull;
		}
		*captureBitmask = *moveBitmask;
		if (board->check ||x!=4||!((board->castling >> ((piece & 8) ? 4 : 0)) & 1)||((piece&8)&&y!=0)||((!(piece&8))&&y!=7)) break;


		if (piece & 8) {
			if (((board->castling >> 5) & 1) && (96ull & (~board->allBitmap))&&(128ull&board->blackBitmaps.rook)) {//right
				if (!((board->whiteBitmaps.pawn & 0xf000ull) || (board->whiteBitmaps.knight & 0xf09800ull) || (board->whiteBitmaps.king & 0xc000ull))) {
					uint64_t tempMask;
					uint64_t tempMask2 = 0ull;
					getMaskOfPiece(12, x + 1, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					getMaskOfPiece(12, x + 2, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					if (!(tempMask2 & (board->whiteBitmaps.bishop | board->whiteBitmaps.queen))) {
						tempMask2 = 0ull;
						getMaskOfPiece(11, x + 1, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						getMaskOfPiece(11, x + 2, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						if (!(tempMask2 & (board->whiteBitmaps.rook | board->whiteBitmaps.queen))) {
							//all conditions met
							*moveBitmask |= 64ull;
						}
					}
				}
			}
			if (((board->castling >> 6)  & 1) && (14ull & (~board->allBitmap)) && (1ull & board->blackBitmaps.rook)) {//left
				if (!((board->whiteBitmaps.pawn & 0x1e0cull) || (board->whiteBitmaps.knight & 0x1e3300ull) || (board->whiteBitmaps.king & 0x60cull))) {
					uint64_t tempMask;
					uint64_t tempMask2 = 0ull;
					getMaskOfPiece(12, x - 1, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					getMaskOfPiece(12, x - 2, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					if (!(tempMask2 & (board->whiteBitmaps.bishop | board->whiteBitmaps.queen))) {
						tempMask2 = 0ull;
						getMaskOfPiece(11, x - 1, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						getMaskOfPiece(11, x - 2, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						if (!(tempMask2 & (board->whiteBitmaps.rook | board->whiteBitmaps.queen))) {
							//all conditions met
							*moveBitmask |= 4ull;
						}
					}
				}
			}
		}
		else {
			if (((board->castling >> 1) & 1) && (0x6000000000000000ull & (~board->allBitmap)) && (0x8000000000000000ull & board->whiteBitmaps.rook)) {//right
				if (!((board->blackBitmaps.pawn & 0xf0000000000000ull) || (board->blackBitmaps.knight & 0x98f00000000000ull) || (board->blackBitmaps.king & 0xc0000000000000ull))) {
					uint64_t tempMask;
					uint64_t tempMask2 = 0ull;
					getMaskOfPiece(4, x + 1, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					getMaskOfPiece(4, x + 2, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					if (!(tempMask2 & (board->blackBitmaps.bishop | board->blackBitmaps.queen))) {
						tempMask2 = 0ull;
						getMaskOfPiece(3, x + 1, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						getMaskOfPiece(3, x + 2, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						if (!(tempMask2 & (board->blackBitmaps.rook | board->blackBitmaps.queen))) {
							//all conditions met
							*moveBitmask |= 0x4000000000000000ull;
						}
					}
				}
			}
			if (((board->castling >>  2) & 1) && (0xe00000000000000ull & (~board->allBitmap))&&(0x100000000000000ull&board->whiteBitmaps.rook)) {
				if (!((board->blackBitmaps.pawn & 0xc16000000000000ull) || (board->blackBitmaps.knight & 0x331e0000000000ull) || (board->blackBitmaps.king & 0xc06000000000000ull))) {
					uint64_t tempMask;
					uint64_t tempMask2 = 0ull;
					getMaskOfPiece(4, x - 1, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					getMaskOfPiece(4, x - 2, y, &tempMask, &tempMask, board);
					tempMask2 |= tempMask;
					if (!(tempMask2 & (board->blackBitmaps.bishop | board->blackBitmaps.queen))) {
						tempMask2 = 0ull;
						getMaskOfPiece(3, x - 1, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						getMaskOfPiece(3, x - 2, y, &tempMask, &tempMask, board);
						tempMask2 |= tempMask;
						if (!(tempMask2 & (board->blackBitmaps.rook | board->blackBitmaps.queen))) {
							//all conditions met
							*moveBitmask |= 0x400000000000000ull;
						}
					}
				}
			}
		}

		/*if (x == 0) {
			maskArray[y] = 128 >> (x + 1);
			if (y > 0)
				maskArray[y - 1] = 192;
			if (y < 7) maskArray[y + 1] =192;
		}
		else {
		maskArray[y] = 160 >> (x - 1);
			if (y > 0)
				maskArray[y - 1] = 224 >> (x - 1);
			if (y < 7) maskArray[y + 1] = 224 >> (x - 1);
		}*/
	}
	break;
	case 2://QUEEN
	{
		uint64_t rookMask = 0ull;
		uint64_t bishopMask = 0ull;
		getMaskOfPiece(3, x, y, &rookMask, &rookMask, board);
		getMaskOfPiece(4, x, y, &bishopMask, &bishopMask, board);
		*moveBitmask = rookMask | bishopMask;
		*captureBitmask = *moveBitmask;
		/*if (y == 0 || (y == 1 && x == 0)) {
			*bitMask = 460039ull >> ((1 - y) * 8 + (1 - x));
		}
		else {
			*bitMask = 460039ull << ((y - 1) * 8 + (x - 1));
		}
		if (x == 7) {
			*bitMask &= ~72340172838076673ull;
		}
		else if (x == 0) {
			*bitMask &= ~9259542123273814144ull;
		}*/
		/*bitMask |= ((72340172838076673ull << x) | 255ull << (y * 8)) ^ (1ull << (y * 8 + x));

		uint64_t temp = 0;
		if (y > (7 - x)) {
			temp = 72624976668147840ull << (x - y);
			for (int i = 0; i < (x - y); i++) {
				temp &= 72340172838076673ull << i;
			}
			*bitMask |= temp;
		}
		else {
			temp = 72624976668147840ull >> (y - x);
			for (int i = 0; i < (y - x); i++) {
				temp &= 9259542123273814144ull >> i;
			}
			*bitMask |= temp;
		}
		if (y > x) {
			temp = 9241421688590303745ull >> (y - x);
			for (int i = 1; i < (y - x); i++) {
				temp &= 9259542123273814144ull >> i;
			}
			*bitMask |= temp;
		}
		else {
			temp = 9241421688590303745ull << (x - y);
			for (int i = 1; i < (x - y); i++) {
				temp &= 72340172838076673ull << i;
			}
			*bitMask |= temp;
		}
		*bitMask ^= (1ull << (y * 8 + x));
		/*for (int i = 0; i < y; i++) {
			if (x-y + i < 0) 
				maskArray[i] = 128 >> (x + y - i);
			else
				maskArray[i] = 128 >> x - y + i | (128>> (x + y - i));
		}
		for (int i = 0; i < 7-y; i++) {
			if (x-1-i < 0)
				maskArray[i + y + 1] =(128 >> (x + i + 1));
			else
			maskArray[i + y + 1] = (128 >> (x - i - 1)) | (128 >> (x + i + 1));
		}
		if (x == 0) {
			maskArray[y] |= 128 >> (x + 1);
			if (y > 0)
				maskArray[y - 1] |= 192;
			if (y < 7) maskArray[y + 1] |= 192;
		}
		else {
			maskArray[y] |= 160 >> (x - 1);
			if (y > 0)
				maskArray[y - 1] |= 224 >> (x - 1);
			if (y < 7) maskArray[y + 1] |= 224 >> (x - 1);
		}

		for (int i = 0; i < 8; i++) {
			if (i == y)continue;
			maskArray[i] |= 128 >> x;
		}
		for (int i = 0; i < 8; i++) {
			if (i == x)continue;
			maskArray[y] |= 128 >> i;
		}*/
	}
		break;
	case 3://ROOK
		//*moveBitmask = ((72340172838076673ull << x)| 255ull<<(y*8))^(1ull<<(y*8+x));
		*moveBitmask = MGRook[x + y * 8].ptr[0];
		* moveBitmask &= board->allBitmap;
		GetMagicMaskForRook(x + y * 8, moveBitmask);
		*captureBitmask = *moveBitmask;
		/*for (int i = 0; i < 8; i++) {
			if (i == y)continue;
			maskArray[i] |= 128 >> x;
		}
		for (int i = 0; i < 8; i++) {
			if (i == x)continue;
			maskArray[y] |= 128 >> i;
		}*/
		break;
	case 4://BISHOP
	{
		uint64_t temp = 0ull;
		/*if (7 - y > x) {
			temp = 72624976668147840ull << (7-y + x);
			for (int i = 0; i < (8-y + x); i++) {
				temp &= ~72340172838076673ull << i;
			}
			*moveBitmask |= temp;
		}
		else {
			temp = 72624976668147840ull >> (7-y - x);
			for (int i = 0; i < (7-y - x); i++) {
				temp &= ~9259542123273814144ull >> i;
			}
			*moveBitmask |= temp;
		}
		if (y > x) {
			temp = 9241421688590303745ull >> (y - x);
			for (int i = 0; i < (y - x); i++) {
				temp &= ~9259542123273814144ull >> i;
			}
			*moveBitmask |= temp;
		}
		else {
			temp = 9241421688590303745ull << (x - y);
			for (int i = 0; i < (x - y); i++) {
				temp &= ~72340172838076673ull << i;
			}
			*moveBitmask |= temp;
		}*/
		*moveBitmask = MGBishop[x + y * 8].ptr[0];
		//*moveBitmask ^= (1ull << (y * 8 + x));
		*moveBitmask &= board->allBitmap;
	    GetMagicMaskForBishop(x + y * 8, moveBitmask);
		*captureBitmask = *moveBitmask;

			/*for (int i = 0; i < y; i++) {
				if (x - y + i < 0)
					maskArray[i] = 128 >> (x + y - i);
				else
					maskArray[i] = 128 >> x - y + i | (128 >> (x + y - i));
			}
			for (int i = 0; i < 7 - y; i++) {
				if (x - 1 - i < 0)
					maskArray[i + y + 1] = (128 >> (x + i + 1));
				else
					maskArray[i + y + 1] = (128 >> (x - i - 1)) | (128 >> (x + i + 1));
			}*/
	}
		break;
	case 5://KNIGHT
		/*if (x < 1) {
			if(y<6)
				maskArray[y + 2] = 128 >> (x + 1);
			if(y>1)
			maskArray[y - 2] = 128 >> (x + 1);
		}
		else {
			if(y<6)
				maskArray[y + 2] = 160 >> (x - 1);
			if(y>1)
				maskArray[y - 2] = 160 >> (x - 1);
		}
		if (x < 2) {
			if(y<7)
				maskArray[y + 1] = 128 >> (x + 2);
			if(y>0)
				maskArray[y - 1] = 128 >> (x + 2);
		}
		else {
			if(y<7)
				maskArray[y + 1] = 136 >> (x - 2);
			if(y>0)
				maskArray[y - 1] = 136 >> (x - 2);
		}*/
		if (y*8+x>=18) {
			*moveBitmask = 43234889994ull << ((y - 2) * 8 + (x - 2));
		}
		else {
			*moveBitmask = 43234889994ull >> ((2 - y) * 8 + (2 - x));
		}
		if (x >= 6) {
			*moveBitmask &= ~217020518514230019ull;
		}
		else if (x <= 1) {
			*moveBitmask &= ~13889313184910721216ull;
		}
		*captureBitmask = *moveBitmask;
		break;
	case 6://PAWN
		if (!(piece & 8)) {
			if (y >0) {
				*moveBitmask = 1ull << ((y - 1) * 8 + x);
				if (y == 6)
					*moveBitmask |= 1ull << ((y - 2) * 8 + x);
			}
		}
		else {
			if (y < 7) {
				*moveBitmask = 1ull << ((y + 1) * 8 + x);
				if (y == 1)
					*moveBitmask |= 1ull << ((y + 2) * 8 + x);
			}
		}
		if (y == 0 && x == 0) {
			*captureBitmask = 2ull;
		}
		else {
			*captureBitmask = 5ull << (x - 1) + (y + ((piece & 8) ? 1 : -1)) * 8;
			if (x == 0) *captureBitmask &= 0x7f7f7f7f7f7f7f7full;
			else if (x == 7) *captureBitmask &= 0xfefefefefefefefeull;
		}
		break;
	default:
		break;
	}
}
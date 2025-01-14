#include"../header/chessPieces.h"
#include <string.h>
#include"../header/interface.h"
#include"../header/game.h"
#include"../header/board.h"
#include<stdint.h>
#ifndef __linux__
#include<intrin.h>
#endif
#include"../header/magicBitboard.h"
//#include"../header/magicBitboard.h"

//x = chessPiece2<<4|chessPiece1
/*const char startingBoard[32] =
{
	189,202,156,219,
	238,238,238,238,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	102,102,102,102,
	53,66,20,83
};*/
const char startingBoard[64] =
{
	BLACK | ROOK, BLACK | KNIGHT, BLACK | BISHOP, BLACK | QUEEN, BLACK | KING, BLACK | BISHOP, BLACK | KNIGHT, BLACK | ROOK,
	BLACK | PAWN, BLACK | PAWN  , BLACK | PAWN  , BLACK | PAWN , BLACK | PAWN, BLACK | PAWN  , BLACK | PAWN  , BLACK | PAWN,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	PAWN, PAWN  , PAWN  , PAWN , PAWN, PAWN  , PAWN  , PAWN,
	ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
};

char xPicked=-1, yPicked;


void InitBoard(ChessBoard* board) {
    //mainBoard = (ChessBoard){ .castling = 119,.enPassant = 0,.check = 0,.round = 0,.pawnPromotion = -1, .board = { 0 } };
	board->castling = 119;
	board->enPassant = 0;
	board->check = 0;
	board->round = 0;
	board->pawnPromotion = -1;
    memcpy(&board->board, startingBoard, 64);
	setBitmaps(board);
}
char getPieceFromBoard(char x, char y, ChessBoard* board) {
	return board->board[y * 8 + x ];
}

int getIndexOfFirstBit(uint64_t* bitmap) {
	unsigned long index;
#ifndef __linux__
	_BitScanForward64(&index, *bitmap);
#else 
	__builtin_ctzll(*bitmap);
#endif
	return index;
}

int MaskBitCount(uint64_t mask)
{
	int count;
	for (count = 0; mask; count++)
		mask &= mask - 1;
	return count;
}

char getSquareOfKing(char color, ChessBoard* board) {
	uint64_t* bitmap = color ? &board->blackBitmaps.king : &board->whiteBitmaps.king;
	//get index of king position from bitmap
	return getIndexOfFirstBit(bitmap);
}

char checkIfCheck(char color, ChessBoard* board);

/*void getLegalMovesWithoutCheck(char xPick, char yPick, char* moveBitMask, char* captureBitMask, ChessBoard* board) {
	char piece = getPieceFromBoard(xPick, yPick,board);
	char pieceColor = piece & BLACK;
    if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoard(x1, y1,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) ) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoard(x2, y2,board)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
				if (pieceOnBoard == 0)
					moveBitMask[y2] |= 128 >> (x2);
				else
					captureBitMask[y2] |= 128 >> (x2);
			}
		}
		return;
	}
	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? 1 : -1;
		if (!((!pieceColor && yPick < 1) || (pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck=getPieceFromBoard(xPick - 1, yPick + colorSide,board))&&(pieceCkeck&BLACK)!=pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide,board)) && (pieceCkeck & BLACK) != pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
			if (board->enPassant && ((board->enPassant & 15) == yPick) && abs((board->enPassant >> 4) - xPick) == 1) {
				captureBitMask[yPick + colorSide] |= 128 >> (board->enPassant >> 4);
			}
		}
	}
	uint64_t mask = 0;
	getMaskOfPiece(piece, xPick, yPick, &mask);//directions: up-1, down-2, right-3, left-4, up-right-5, up-left-6, down-right-7, down-left-8
	char direction = 8;
	char x=xPick, y=yPick;
	while (direction) {
		switch (direction)
		{
		case 1:
			y--;
			break;
		case 2:
			y++;
			break;
		case 3:
			x++;
			break;
		case 4:
			x--;
			break;
		case 5:
			x++;
			y--;
			break;
		case 6:
			x--;
			y--;
			break;
		case 7:
			x++;
			y++;
			break;
		case 8:
			x--;
			y++;
			break;
		default:
			break;
		}
		if (x < 0 || x>7 || y < 0 || y>7) {
			direction--;
			x = xPick;
			y = yPick;
			continue;
		}
		if (mask[y]&(128>>x)) {
			char pieceOnBoard = getPieceFromBoard(x, y,board);
			if (pieceOnBoard == 0) {
				moveBitMask[y] |= 128 >> x;
				continue;
			}else
			if ((pieceOnBoard&BLACK)!=pieceColor && (piece & CHESSMASK) != PAWN) {
				captureBitMask[y] |= 128 >> x;
			}
		}
		direction--;
		x = xPick;
		y = yPick;
	}
	return;
}*/

void getLegalMoves(char xPick, char yPick, uint64_t* moveBitmask, uint64_t* captureBitmask, ChessBoard* board) {
	if ((board->round ? board->blackBitmaps.lockedPieces : board->whiteBitmaps.lockedPieces) & (1ull << (xPick + yPick * 8))) return;
	char piece = getPieceFromBoard(xPick, yPick, board);

	getMaskOfPiece(piece, xPick, yPick, moveBitmask, captureBitmask, board);

	*moveBitmask = (*moveBitmask & ~board->allBitmap);
	*captureBitmask = *captureBitmask & (piece & BLACK ? board->whiteBitmaps.all : board->blackBitmaps.all);
	if ((piece & CHESSMASK) == PAWN && board->enPassant && abs(board->enPassant - (xPick + yPick * 8)) == 1) {
		*captureBitmask |= 1ull << (board->enPassant + ((piece & BLACK) ? 8 : -8));
	}

	if ((piece & CHESSMASK) == KING) {
		uint64_t _moveBitmask = 0ull;
		uint64_t _captureBitmask = 0ull;
		uint64_t* _allMapPtr = (piece & BLACK) ? &board->blackBitmaps.all : &board->whiteBitmaps.all;
		uint64_t _allMap = *_allMapPtr;

		*_allMapPtr |= *moveBitmask;
		for (char i = 0; i < 64; i++) {
			if (board->board[i] && (board->board[i] & BLACK) != (piece & BLACK) && ((i % 8) != xPick && (i / 8) != yPick)) {
				if ((board->board[i] & CHESSMASK) == KING) {
					getMaskOfPiece(board->board[i], i % 8, i / 8, &_moveBitmask, &_captureBitmask, board);
				}else
				getLegalMoves(i % 8, i / 8, &_moveBitmask, &_captureBitmask, board);
				*moveBitmask &= ~_captureBitmask;
				*captureBitmask &= ~_captureBitmask;
			}
		}
		*_allMapPtr = _allMap;
	}

	if (board->check) {
		if (board->round) {
			if ((piece & CHESSMASK) != KING)
				*moveBitmask &= board->blackBitmaps.squaresToDefend;
			else
				*moveBitmask &= ~board->blackBitmaps.squaresToDefend;
			*captureBitmask &= board->blackBitmaps.squaresToDefend;
		}
		else {
			if ((piece & CHESSMASK) != KING)
				*moveBitmask &= board->whiteBitmaps.squaresToDefend;
			else if(~board->whiteBitmaps.squaresToDefend!=UINT64_MAX){
				*moveBitmask &= ~board->whiteBitmaps.squaresToDefend;
			}
			*captureBitmask &= board->whiteBitmaps.squaresToDefend;

		}
	}

	/*if ((piece & CHESSMASK) == KING && !(board->check || !(board->castling >> ((piece & BLACK) ? 4 : 0) & 1))) {//castling
		if ((board->castling >> ((piece & BLACK) ? 1 : 5) & 1) && (((piece & BLACK) ? 96ull : 0x6000000000000000ull) & (~board->allBitmap))) { //right
			if (!checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 3, yPick, board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 2, yPick, board))
			{
				pieceMask |= 1ull << (xPick + 2) + yPick * 8;
			}
		}
		if ((board->castling >> ((piece & BLACK) ? 2 : 4) & 1) && (((piece & BLACK) ? 14ull : 0xe00000000000000ull) & (~board->allBitmap))) { //left
			if (!checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 5, yPick, board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 6, yPick, board))
			{
				pieceMask |= 1ull << (xPick - 2) + yPick * 8;
			}
		}
	}*/
	/*char piece = getPieceFromBoard(xPick, yPick, board);
	char pieceColor = piece & BLACK;
	//char counterPieceColor = ~pieceColor&BLACK;
	if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoard(x1, y1,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x1, y1,board)) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoard(x2, y2,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x2, y2,board)) {
				if (pieceOnBoard == 0)
					moveBitMask[y2] |= 128 >> (x2);
				else
					captureBitMask[y2] |= 128 >> (x2);
			}
		}
		return;
	}

	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? 1 : -1;
		if (!((!pieceColor && yPick < 1) || (pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck = getPieceFromBoard(xPick - 1, yPick + colorSide,board)) && (pieceCkeck & BLACK) != pieceColor&&!checkIfCheckOnChangedBoard(pieceColor,xPick,yPick,xPick-1,yPick+colorSide,board))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide, board)) && (pieceCkeck & BLACK) != pieceColor && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick + colorSide,board))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
			if (board->enPassant && ((board->enPassant & 15) == yPick) && abs((board->enPassant >> 4) - xPick) == 1) {
				captureBitMask[yPick + colorSide] |= 128 >> (board->enPassant >> 4);
			}
		}
	}
	char mask[8] = { 0 };
	getMaskOfPiece(piece, xPick, yPick, &mask);//directions: up-1, down-2, right-3, left-4, up-right-5, up-left-6, down-right-7, down-left-8
	char direction = 8;
	char x = xPick, y = yPick;
	while (direction) {
		switch (direction)
		{
		case 1:
			y--;
			break;
		case 2:
			y++;
			break;
		case 3:
			x++;
			break;
		case 4:
			x--;
			break;
		case 5:
			x++;
			y--;
			break;
		case 6:
			x--;
			y--;
			break;
		case 7:
			x++;
			y++;
			break;
		case 8:
			x--;
			y++;
			break;
		default:
			break;
		}
		if (x < 0 || x>7 || y < 0 || y>7) {
			direction--;
			x = xPick;
			y = yPick;
			continue;
		}
		if (mask[y] & (128 >> x)) {
			char pieceOnBoard = getPieceFromBoard(x, y,board);
			if (!checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x, y,board)) {
				if (pieceOnBoard == 0) {
					moveBitMask[y] |= 128 >> x;
					continue;
				}
				else if ((pieceOnBoard & BLACK) != pieceColor && (piece & CHESSMASK) != PAWN) {
					captureBitMask[y] |= 128 >> x;
				}
			}
		}
		direction--;
		x = xPick;
		y = yPick;
	}
	if ((piece & CHESSMASK) == KING&&!board->check) {
		if ((128 >> ((piece & BLACK) ? 5 : 1)) & board->castling) { //left rook
			if (!getPieceFromBoard(xPick - 1, yPick,board) && !getPieceFromBoard(xPick - 2, yPick,board) && !getPieceFromBoard(xPick - 3, yPick,board)&&!checkIfCheckOnChangedBoard(piece&BLACK,xPick,yPick,3,yPick,board)&& !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 2, yPick,board))
			{
				moveBitMask[yPick] |= 128 >> 2;
			}
		}
		if ((128 >> ((piece & BLACK) ? 6 : 2)) & board->castling) { //right rook
            if (!getPieceFromBoard(xPick + 1, yPick,board) && !getPieceFromBoard(xPick + 2, yPick,board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 5, yPick, board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 6, yPick, board))
			{
				moveBitMask[yPick] |= 2;
			}
		}
	}*/
	return;
}

char isLegalMove(char xPick, char yPick, char xDest, char yDest,ChessBoard* board) {
	uint64_t moveMask = 0ull;
	uint64_t captureMask =  0ull;
	getLegalMoves(xPick, yPick, &moveMask,&captureMask,board);
	return !!((moveMask | captureMask) & (1ull << (xDest + yDest * 8)));
}

void setPieceOnBoard(char x, char y, char piece,ChessBoard* board) {
	//boardRepresentation[(x / 2)*8+y] = (boardRepresentation[(x / 2)*8+y] & (240 >> (!(x % 2) * 4)))|(piece<<!(x%2)*4);
	board->board[y * 8 + x ] = piece;
}

void ChangeBitboard(char piecePicked, char x, char y, char withAllBitboardsUpate, ChessBoard* board);

void movePieceOnBoard(char sourceX, char sourceY, char destinationX, char destinationY, ChessBoard* board) {
	char piece = getPieceFromBoard(sourceX, sourceY,board);
	setPieceOnBoard(sourceX, sourceY, 0,board);
	ChangeBitboard(piece, sourceX, sourceY, 0,board);
	setPieceOnBoard(destinationX, destinationY, piece,board);
	ChangeBitboard(piece, destinationX, destinationY,1, board);
}

uint64_t getLineBetween(char xFrom,char yFrom,char xTo, char yTo) {
	//jest lepszy sposob na to
	uint64_t mask = 0ull;
	char xDelta = (xTo - xFrom);
	if(xDelta!=0)
	xDelta /= abs(xDelta);
	char yDelta = yTo - yFrom;
	if (yDelta != 0)
	yDelta /= abs(yDelta);
	for (char x = xFrom+xDelta, y = yFrom+yDelta; x != xTo+xDelta && y != yTo+yDelta; mask |= 1ull << (y * 8 + x),x += xDelta, y += yDelta);//&&(!getPieceFromBoard(x,y,board))
	return mask;
}

uint64_t createAttackersBitboard(char kingIndex,uint64_t sliderAttackers){
	uint64_t mask=0ull;
	while (sliderAttackers) {
		char index = getIndexOfFirstBit(&sliderAttackers);
		mask |= getLineBetween(kingIndex % 8, kingIndex / 8, index % 8, index / 8);
		sliderAttackers &= (sliderAttackers - 1);
	}
	return mask;
}


char checkIfCheck(char color,ChessBoard* board) {
	char kingPos = getSquareOfKing(color,board);
	char kingPosX = kingPos %8;
	char kingPosY = kingPos /8;
	uint64_t mask[4] = { 0ull };
	uint64_t temp = 0ull;
	getMaskOfPiece(PAWN | (color ? BLACK : 0), kingPosX, kingPosY, &temp, &mask[0], board);
	getMaskOfPiece(KNIGHT, kingPosX, kingPosY, &temp, &mask[1],board);
	getMaskOfPiece(BISHOP, kingPosX, kingPosY, &temp, &mask[2],board);
	getMaskOfPiece(ROOK , kingPosX, kingPosY, &temp, &mask[3],board);
	mask[0] &= (color ? board->whiteBitmaps.pawn : board->blackBitmaps.pawn);
	mask[1] &= (color ? board->whiteBitmaps.knight : board->blackBitmaps.knight);
	mask[2] &= (color ? (board->whiteBitmaps.bishop | board->whiteBitmaps.queen) : (board->blackBitmaps.bishop | board->blackBitmaps.queen));
	mask[3] &= (color ? (board->whiteBitmaps.rook | board->whiteBitmaps.queen) : (board->blackBitmaps.rook | board->blackBitmaps.queen));
	uint64_t attackers =mask[0]|mask[1]|mask[2]|mask[3];

	uint64_t attackLine=createAttackersBitboard(kingPos,mask[2]|mask[3]);//squares to block

	board->allBitmap = color ? board->whiteBitmaps.all : board->blackBitmaps.all;
	getMaskOfPiece(BISHOP, kingPosX, kingPosY, &temp, &mask[2], board);
	getMaskOfPiece(ROOK, kingPosX, kingPosY, &temp, &mask[3], board);
	attackers = mask[2] & (color?board->whiteBitmaps.bishop:board->blackBitmaps.bishop);//attackers but blocked
	uint64_t lockedPieces = 0ull;
	if (MaskBitCount(attackers) == 1)
		lockedPieces |= attackers;

	attackers = mask[3] & (color ? board->whiteBitmaps.rook : board->blackBitmaps.rook);//attackers but blocked
	if (MaskBitCount(attackers) == 1)
		lockedPieces |= attackers;

	attackers = (mask[3]|mask[2]) & (color ? board->whiteBitmaps.queen : board->blackBitmaps.queen);//attackers but blocked
	if (MaskBitCount(attackers) == 1)
		lockedPieces |= attackers;

	board->allBitmap = board->blackBitmaps.all | board->whiteBitmaps.all;
	//lockedPieces &= color ? board->blackBitmaps.all : board->whiteBitmaps.all;//locked pieces


	if(color){
		board->blackBitmaps.lockedPieces = lockedPieces;
		board->blackBitmaps.squaresToDefend = attackLine ? attackLine : UINT64_MAX;
	}
	else {
		board->whiteBitmaps.lockedPieces = lockedPieces;
		board->whiteBitmaps.squaresToDefend = attackLine ? attackLine : UINT64_MAX;
	}
	return !!attackers;

	/*char moveBitmask[8] = {0};
	char captureBitmask[8] = { 0 };
	for(char x=0;x<8;x++)
		for (char y=0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y,board);
			if ((piece & BLACK)!=color)
			{
				getLegalMovesWithoutCheck(x, y, &moveBitmask, &captureBitmask,board);
				if ((captureBitmask[kingPosY] & (128 >> kingPosX)))
					return 1;
			}
		}
	return 0;*/
}

//0-none 1-checkmate 2-stalemate 
char checkIfCheckmateOrStalemateWhileChecked(char color, ChessBoard* board) {
	uint64_t moveBitmask = 0ull;
	uint64_t captureBitmask = 0ull;
	for(char x=0;x<8;x++)
		for (char y = 0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y,board);
			if ((piece & BLACK) == color)
			{
				getLegalMoves(x, y, &moveBitmask, &captureBitmask,board);
				if ((moveBitmask | captureBitmask)) return 0;
			}
		}

	return !!!board->check + 1;
}



void pickupPiece(char x, char y, ChessBoard* board) {
	if (board->pawnPromotion!=-1) {
		if ((y != -1 && y  != 8)) return;
		setPieceOnBoard(board->pawnPromotion >> 4, board->pawnPromotion & 15, ((board->pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0),board);
		ChangeBitboard(((board->pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0), board->pawnPromotion >> 4, board->pawnPromotion & 15, 1, board);
		ChangeBitboard(PAWN| (y == 8 ? BLACK : 0), board->pawnPromotion >> 4, board->pawnPromotion & 15, 1, board);
		interface_clearPawnPromotion();
		//interface_renderPiece(pawnPromotion >> 4, pawnPromotion & 15);
		NextRound();
	}else
	if (xPicked != -1&& isLegalMove(xPicked, yPicked, x, y,board)) {
		char piece = getPieceFromBoard(xPicked, yPicked,board);
		movePieceOnBoard(xPicked, yPicked, x, y,board);
		interface_renderMovePiece(xPicked, yPicked, x, y, piece);
		if (board->enPassant) {
			if ((piece & CHESSMASK) == PAWN&&(getPieceFromBoard(x,y + ((piece & BLACK) ? -1 : 1),board)&CHESSMASK)==PAWN) {
				setPieceOnBoard(x, y + ((piece & BLACK) ? -1 : 1), 0, board);
				ChangeBitboard(piece^BLACK,board->enPassant%8,board->enPassant/8,1,board);
				interface_renderPiece(x, y + ((piece & BLACK) ? -1 : 1));
			}
			board->enPassant = 0;
		}
		if ((piece & CHESSMASK) == KING) {
			if (x - xPicked == 2) { //castle wtith right rook
				movePieceOnBoard(7, yPicked, 5, yPicked,board);
			}
			else if (xPicked - x == 2) {//castle wtith left rook
				movePieceOnBoard(0, yPicked, 3, yPicked,board);
			}
		}
		interface_clearBitmask();
		if ((y == 0 || y == 7)&&(piece & CHESSMASK) == PAWN) {
			xPicked = -1;
			interface_showPawnPromotion(x, y);
		}
		else {
			NextRound();
			if ((piece&CHESSMASK) == ROOK) {
				if (xPicked == 0) {
					board->castling &= ~(1 << ((piece & BLACK) ? 6 : 2));
				}
				else if (xPicked == 7) {
					board->castling &= ~(1 << ((piece & BLACK) ? 5 : 1));
				}
			}
			else if ((piece & CHESSMASK) == KING) {
				board->castling &= 7 << ((piece & BLACK) ? 0 : 4);
			}
			else if ((piece & CHESSMASK) == PAWN&&abs(yPicked-y)==2) {
				board->enPassant = x + y * 8;
			}
		}
		xPicked = -1;

	} else{
		char piece = getPieceFromBoard(x, y,board);
		if (!piece || (piece >> 3) != board->round) {
			return;
		}
		interface_clearBitmask();
		interface_showLegalMovesOfCurrentPiece(x,y);
		xPicked = x;
		yPicked = y;
	}
}
void pickupAndPlacePiece(char xSrc, char ySrc, char xDst, char yDst, ChessBoard* board) {
	
	pickupPiece(xSrc, ySrc, board);
	pickupPiece(xDst, yDst, board);
}
void simulatePieceMoveOnBoard(char xSrc, char ySrc, char xDst, char yDst, ChessBoard* board) {
	char piece = getPieceFromBoard(xSrc, ySrc, board);
	movePieceOnBoard(xSrc, ySrc, xDst, yDst, board);
	if (board->enPassant) {
		if ((piece & CHESSMASK) == PAWN && (getPieceFromBoard(xDst, yDst + ((piece & BLACK) ? -1 : 1), board) & CHESSMASK) == PAWN) {
			setPieceOnBoard(xDst, yDst + ((piece & BLACK) ? -1 : 1), 0, board);
			ChangeBitboard(piece ^ BLACK, board->enPassant % 8, board->enPassant / 8, 1, board);
		}
		board->enPassant = 0;
	}
	if ((piece & CHESSMASK) == KING) {
		if (xDst - xSrc == 2) { //castle wtith right rook
			movePieceOnBoard(7, ySrc, 5, ySrc, board);
		}
		else if (xSrc - xDst == 2) {//castle wtith left rook
			movePieceOnBoard(0, ySrc, 3, ySrc, board);
		}
	}
	if ((piece & CHESSMASK) == ROOK) {
		if (xSrc == 0) {
			board->castling &= ~(1 << ((piece & BLACK) ? 6 : 2));
		}
		else if (xSrc == 7) {
			board->castling &= ~(1 << ((piece & BLACK) ? 5 : 1));
		}
	}
	else if ((piece & CHESSMASK) == KING) {
		board->castling &= 7 << ((piece & BLACK) ? 0 : 4);
	}
	else if ((piece & CHESSMASK) == PAWN && abs(ySrc - yDst) == 2) {
		board->enPassant = xDst + yDst * 8;
	}
}

unsigned char BitCount(unsigned char byte) {
	static const unsigned char NIBBLE_LOOKUP[16] =
	{
	  0, 1, 1, 2, 1, 2, 2, 3,
	  1, 2, 2, 3, 2, 3, 3, 4
	};
	return NIBBLE_LOOKUP[byte & 0x0F] + NIBBLE_LOOKUP[byte >> 4];
}

int getAmountOfLegalMoves(char x, char y,char *moveMask,char *captureMask) 
{
	getLegalMoves(x, y, moveMask, captureMask,&displayBoard);
	int amount = 0;
	for (char i = 0; i < 8; i++) {
		amount += BitCount(moveMask[i]) ;//+ BitCount(captureMask[i])
	}
	return amount;
}

int getAmountOfLegalMovesOnBoard(char x, char y, uint64_t* moveMask, char* captureMask,ChessBoard* board)
{
	getLegalMoves(x, y, moveMask, captureMask,board);
	int amount = 0;
	for (char i = 0; i < 8; i++) {
		amount += BitCount(moveMask[i]);//+ BitCount(captureMask[i])
	}
	return amount;
}

void setBitmaps(ChessBoard* board) {
    for (char x = 0; x < 8; x++) {
        for (char y = 0; y < 8; y++) {
            char piece = getPieceFromBoard(x, y, board);
            switch (piece & CHESSMASK) {
                case KING:
                    if (piece & BLACK)
                        board->blackBitmaps.king |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.king |= 1ull << (y * 8 + x);
                    break;
                case QUEEN:
                    if (piece & BLACK)
						board->blackBitmaps.queen |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.queen |= 1ull << (y * 8 + x);
                    break;
                case ROOK:
                    if (piece & BLACK)
						board->blackBitmaps.rook |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.rook |= 1ull << (y * 8 + x);
                    break;
                case BISHOP:
                    if (piece & BLACK)
						board->blackBitmaps.bishop |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.bishop |= 1ull << (y * 8 + x);
                    break;
                case KNIGHT:
                    if (piece & BLACK)
						board->blackBitmaps.knight |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.knight |= 1ull << (y * 8 + x);
                    break;
                case PAWN:
                    if (piece & BLACK)
						board->blackBitmaps.pawn |= 1ull << (y * 8 + x);
                    else
						board->whiteBitmaps.pawn |= 1ull << (y * 8 + x);
                    break;
                default:
                    break;
            }
        }
    }
	board->whiteBitmaps.all = board->whiteBitmaps.king | board->whiteBitmaps.queen | board->whiteBitmaps.rook | board->whiteBitmaps.bishop | board->whiteBitmaps.knight | board->whiteBitmaps.pawn;
	board->blackBitmaps.all = board->blackBitmaps.king | board->blackBitmaps.queen | board->blackBitmaps.rook | board->blackBitmaps.bishop | board->blackBitmaps.knight | board->blackBitmaps.pawn;
    board->allBitmap = board->whiteBitmaps.all | board->blackBitmaps.all;
	board->whiteBitmaps.lockedPieces = 0ull;
	board->blackBitmaps.lockedPieces = 0ull;
	board->whiteBitmaps.squaresToDefend = UINT64_MAX;
	board->blackBitmaps.squaresToDefend = UINT64_MAX;
}

void ChangeBitboard(char piecePicked, char x, char y,char withAllBitboardsUpate,ChessBoard* board) {
    uint64_t* bitBoardPicked = NULL;
    switch (piecePicked & CHESSMASK) {
        case PAWN:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.pawn : &board->whiteBitmaps.pawn;
            break;
        case KNIGHT:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.knight : &board->whiteBitmaps.knight;
            break;
        case BISHOP:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.bishop : &board->whiteBitmaps.bishop;
            break;
        case ROOK:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.rook : &board->whiteBitmaps.rook;
            break;
        case QUEEN:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.queen : &board->whiteBitmaps.queen;
            break;
        case KING:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.king : &board->whiteBitmaps.king;
            break;
        default:
            return; 
    }
    *bitBoardPicked ^= 1ull << (y * 8 + x);
	if (piecePicked & BLACK)
		board->blackBitmaps.all ^= 1ull << (y * 8 + x);
	else
		board->whiteBitmaps.all ^= 1ull << (y * 8 + x);
	if (!withAllBitboardsUpate) return;
	board->allBitmap =board->blackBitmaps.all|board->whiteBitmaps.all;
}

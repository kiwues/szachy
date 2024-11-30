#include"../header/chessPieces.h"
#include <string.h>
#include"../header/interface.h"
#include"../header/game.h"

char boardRepresentation[32] = { 0 };

//x = chessPiece2<<4|chessPiece1
const char startingBoard[32] =
{
	189,238,0,0,0,0,102,53, 
	202,238,0,0,0,0,102,66,
	156,238,0,0,0,0,102,20,
	219,238,0,0,0,0,102,83
};
/*const char startingBoard[32] =
{
	189,238,0,0,0,0,102,53,
	202,238,0,0,0,0,102,66,
	156,238,0,0,0,0,102,20,
	219,238,0,0,0,0,102,83
};*/

char xPicked=-1, yPicked;

char castling = 119;// 0001-king didnt move, 0110-rooks didnt move start: 01110111 4bit-white 4bit-black
char enPassant = 0;//0-none or pawn position

void setDefaultChessBoard() {
	memcpy(boardRepresentation, startingBoard, 4 * 8);
}
char getPieceFromBoard(char x, char y) {
	return (boardRepresentation[(x / 2)*8+y] >> !(x % 2) * 4) & 15;
}

char getPositionOfKing(char color) {
	char searchingPiece = KING | color;
	for(char x=0;x<8;x++)
		for (char y = 0; y < 8; y++) {
			if (getPieceFromBoard(x, y) == searchingPiece)
				return (x << 4) | y;
		}
}

char checkIfCheck(char color);
char checkIfCheckOnChangedBoard(char color, char xPicked, char yPicked, char xDest, char yDest);

void getLegalMovesWithoutCheck(char xPick, char yPick,char* moveBitMask,char* captureBitMask) {
	char piece = getPieceFromBoard(xPick, yPick);
	char pieceColor = piece & BLACK;
    if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoard(x1, y1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) ) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoard(x2, y2)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
				if (pieceOnBoard == 0)
					moveBitMask[y2] |= 128 >> (x2);
				else
					captureBitMask[y2] |= 128 >> (x2);
			}
		}
		/*if (xPick  < 6 && yPick  < 6 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick + 1)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 1] |= 128 >> (xPick + 2);
			else
				captureBitMask[yPick + 1] |= 128 >> (xPick + 2);
		}
		if (xPick  < 68 && yPick  > 0 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick - 1)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 1] |= 128 >> (xPick + 2);
			else
				captureBitMask[yPick - 1] |= 128 >> (xPick + 2);
		}
		if (xPick  > 1 && yPick  < 6 && ((pieceOnBoard = getPieceFromBoard(xPick - 2, yPick + 1)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 1] |= 128 >> (xPick - 2);
			else
				captureBitMask[yPick + 1] |= 128 >> (xPick - 2);
		}
		if (xPick  > 1 && yPick  > 0 && ((pieceOnBoard = getPieceFromBoard(xPick - 2, yPick - 1)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 1] |= 128 >> (xPick - 2);
			else
				captureBitMask[yPick - 1] |= 128 >> (xPick - 2);
		}
		if (xPick  < 7 && yPick  < 5 && ((pieceOnBoard = getPieceFromBoard(xPick + 1, yPick + 2)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 2] |= 128 >> (xPick + 1);
			else
				captureBitMask[yPick + 2] |= 128 >> (xPick + 1);
		}
		if (xPick  < 7 && yPick  > 1 && ((pieceOnBoard = getPieceFromBoard(xPick + 1, yPick - 2)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 2] |= 128 >> (xPick + 1);
			else
				captureBitMask[yPick - 2] |= 128 >> (xPick + 1);
		}
		if (xPick  > 0 && yPick  < 5 && ((pieceOnBoard = getPieceFromBoard(xPick - 1, yPick + 2)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 2] |= 128 >> (xPick - 1);
			else
				captureBitMask[yPick + 2] |= 128 >> (xPick - 1);
		}
		if (xPick  > 0 && yPick  > 1 && ((pieceOnBoard = getPieceFromBoard(xPick - 1, yPick - 2)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 2] |= 128 >> (xPick - 1);
			else
				captureBitMask[yPick - 2] |= 128 >> (xPick - 1);
		}*/
		return;
    }
	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? 1 : -1;
		if (!((!pieceColor && yPick < 1) || (pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck=getPieceFromBoard(xPick - 1, yPick + colorSide))&&(pieceCkeck&BLACK)!=pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
			if (enPassant && ((enPassant & 15) == yPick) && abs((enPassant >> 4) - xPick) == 1) {
				captureBitMask[yPick + colorSide] |= 128 >> (enPassant >> 4);
			}
		}
	}
	char mask[8] = { 0 };
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
			char pieceOnBoard = getPieceFromBoard(x, y);
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
}

void getLegalMoves(char xPick, char yPick, char* moveBitMask, char* captureBitMask) {
	char piece = getPieceFromBoard(xPick, yPick);
	char pieceColor = piece & BLACK;
	//char counterPieceColor = ~pieceColor&BLACK;
	if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoard(x1, y1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x1, y1)) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoard(x2, y2)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x2, y2)) {
				if (pieceOnBoard == 0)
					moveBitMask[y2] |= 128 >> (x2);
				else
					captureBitMask[y2] |= 128 >> (x2);
			}
		}
		/*if (xPick < 6 && yPick < 6 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick + 1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 2, yPick + 1)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 1] |= 128 >> (xPick + 2);
			else
				captureBitMask[yPick + 1] |= 128 >> (xPick + 2);
		}
		if (xPick < 6 && yPick > 0 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick - 1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 2, yPick - 1)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 1] |= 128 >> (xPick + 2);
			else
				captureBitMask[yPick - 1] |= 128 >> (xPick + 2);
		}
		if (xPick > 1 && yPick < 6 && ((pieceOnBoard = getPieceFromBoard(xPick - 2, yPick + 1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick - 2, yPick + 1)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 1] |= 128 >> (xPick - 2);
			else
				captureBitMask[yPick + 1] |= 128 >> (xPick - 2);
		}
		if (xPick > 1 && yPick > 0 && ((pieceOnBoard = getPieceFromBoard(xPick - 2, yPick - 1)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick - 2, yPick - 1)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 1] |= 128 >> (xPick - 2);
			else
				captureBitMask[yPick - 1] |= 128 >> (xPick - 2);
		}
		if (xPick < 7 && yPick < 5 && ((pieceOnBoard = getPieceFromBoard(xPick + 1, yPick + 2)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick + 2)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 2] |= 128 >> (xPick + 1);
			else
				captureBitMask[yPick + 2] |= 128 >> (xPick + 1);
		}
		if (xPick < 7 && yPick > 1 && ((pieceOnBoard = getPieceFromBoard(xPick + 1, yPick - 2)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick - 2)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 2] |= 128 >> (xPick + 1);
			else
				captureBitMask[yPick - 2] |= 128 >> (xPick + 1);
		}
		if (xPick > 0 && yPick < 5 && ((pieceOnBoard = getPieceFromBoard(xPick - 1, yPick + 2)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick - 1, yPick + 2)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick + 2] |= 128 >> (xPick - 1);
			else
				captureBitMask[yPick + 2] |= 128 >> (xPick - 1);
		}
		if (xPick > 0 && yPick > 1 && ((pieceOnBoard = getPieceFromBoard(xPick - 1, yPick - 2)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick - 1, yPick - 2)) {
			if (pieceOnBoard == 0)
				moveBitMask[yPick - 2] |= 128 >> (xPick - 1);
			else
				captureBitMask[yPick - 2] |= 128 >> (xPick - 1);
		}*/
		return;
	}

	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? 1 : -1;
		if (!((!pieceColor && yPick < 1) || (pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck = getPieceFromBoard(xPick - 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor&&!checkIfCheckOnChangedBoard(pieceColor,xPick,yPick,xPick-1,yPick+colorSide))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick + colorSide))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
			if (enPassant && ((enPassant & 15) == yPick) && abs((enPassant >> 4) - xPick) == 1) {
				captureBitMask[yPick + colorSide] |= 128 >> (enPassant >> 4);
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
			char pieceOnBoard = getPieceFromBoard(x, y);
			if (!checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x, y)) {
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
	if ((piece & CHESSMASK) == KING&&!check) {
		if ((128 >> ((piece & BLACK) ? 5 : 1)) & castling) { //left rook
			if (!getPieceFromBoard(xPick - 1, yPick) && !getPieceFromBoard(xPick - 2, yPick) && !getPieceFromBoard(xPick - 3, yPick)&&!checkIfCheckOnChangedBoard(piece&BLACK,xPick,yPick,3,yPick)&& !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 2, yPick))
			{
				moveBitMask[yPick] |= 128 >> 2;
			}
		}
		if ((128 >> ((piece & BLACK) ? 6 : 2)) & castling) { //right rook
			if (!getPieceFromBoard(xPick + 1, yPick) && !getPieceFromBoard(xPick + 2, yPick) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 5, yPick) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 6, yPick))
			{
				moveBitMask[yPick] |= 2;
			}
		}
	}
	return;
}

char isLegalMove(char xPick, char yPick, char xDest, char yDest) {
	char moveMask[8] = { 0 };
	char captureMask[8] = { 0 };
	getLegalMoves(xPick, yPick, &moveMask,&captureMask);
	return (moveMask[yDest] & (128 >> xDest)|| captureMask[yDest] & (128 >> xDest));
}



void setPiece(char x, char y, char piece) {
	boardRepresentation[(x / 2)*8+y] = (boardRepresentation[(x / 2)*8+y] & (240 >> (!(x % 2) * 4)))|(piece<<!(x%2)*4);
}

void movePiece(char sourceX, char sourceY, char destinationX, char destinationY) {
	char piece = getPieceFromBoard(sourceX, sourceY);
	setPiece(sourceX, sourceY, 0);
	setPiece(destinationX, destinationY, piece);
	interface_renderMovePiece(sourceX, sourceY, destinationX, destinationY, piece);
}

char* getBoardPtr() {
	return &boardRepresentation;
}

char checkIfCheck(char color) {
	char kingPos = getPositionOfKing(color);
	char kingPosX = kingPos >> 4;
	char kingPosY = kingPos & 15;
	char moveBitmask[8] = { 0 };
	char captureBitmask[8] = { 0 };
	for(char x=0;x<8;x++)
		for (char y=0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y);
			if ((piece & BLACK)!=color)
			{
				getLegalMovesWithoutCheck(x, y, &moveBitmask, &captureBitmask);
				if ((captureBitmask[kingPosY] & (128 >> kingPosX)))
					return 1;
			}
		}
	return 0;
}

char checkIfCheckOnChangedBoard(char color, char xPicked, char yPicked, char xDest, char yDest) {
	char picked = getPieceFromBoard(xPicked, yPicked);
	char dest = getPieceFromBoard(xDest, yDest);
	setPiece(xPicked, yPicked, 0);
	setPiece(xDest, yDest, picked);
	char ret = checkIfCheck(color);
	setPiece(xPicked, yPicked, picked);
	setPiece(xDest, yDest, dest);
	return ret;
}

//0-none 1-checkmate 2-stalemate 
char checkIfCheckmateOrStalemateWhileChecked(char color) {
	char moveBitmask[8] = { 0 };
	char captureBitmask[8] = { 0 };
	for(char x=0;x<8;x++)
		for (char y = 0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y);
			if ((piece & BLACK) == color)
			{
				getLegalMoves(x, y, &moveBitmask, &captureBitmask);
				for (int i = 0; i < 8; i++) {
					if (moveBitmask[i] != 0 || captureBitmask[i] != 0) 
						return 0;
				}
			}
		}
	return !!!check + 1;
}



void pickupPiece(char x, char y) {
	if (pawnPromotion!=-1) {
		if ((y != -1 && y  != 8)) return;
		setPiece(pawnPromotion >> 4, pawnPromotion & 15, ((pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0));
		interface_clearPawnPromotion();
		//interface_renderPiece(pawnPromotion >> 4, pawnPromotion & 15);
		NextRound();
	}else
	if (xPicked != -1&& isLegalMove(xPicked, yPicked, x, y)) {
		char piece = getPieceFromBoard(xPicked, yPicked);
		movePiece(xPicked, yPicked, x, y);
		if (enPassant) {
			if ((piece & CHESSMASK) == PAWN && (getPieceFromBoard(x, y + ((piece & BLACK) ? -1 : 1))&CHESSMASK)==PAWN) {
				setPiece(x, y + ((piece & BLACK) ? -1 : 1), 0);
				interface_renderPiece(x, y + ((piece & BLACK) ? -1 : 1));
			}
			enPassant = 0;
		}
		if ((piece & CHESSMASK) == KING) {
			if (x - xPicked == 2) { //castle wtith right rook
				movePiece(7, yPicked, 5, yPicked);
			}
			else if (xPicked - x == 2) {
				movePiece(0, yPicked, 3, yPicked);
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
					castling &= ~((64)>> (piece & BLACK ? 4 : 0));
				}
				else if (xPicked == 7) {
					castling &= ~((32) >> (piece & BLACK ? 4 : 0));
				}
			}
			else if ((piece & CHESSMASK) == KING) {
				castling &= 240 >> (piece & BLACK ? 0 : 4);
			}
			else if ((piece & CHESSMASK) == PAWN&&abs(yPicked-y)==2) {
				enPassant = (x << 4) | y;
			}
		}
		xPicked = -1;

	} else{
		char piece = getPieceFromBoard(x, y);
		if (!piece || (piece >> 3) != round) {
			return;
		}
		interface_clearBitmask();
		interface_showLegalMovesOfCurrentPiece();
		xPicked = x;
		yPicked = y;
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
	getLegalMoves(x, y, moveMask, captureMask);
	int amount = 0;
	for (char i = 0; i < 8; i++) {
		amount += BitCount(moveMask[i]) ;//+ BitCount(captureMask[i])
	}
	return amount;
}
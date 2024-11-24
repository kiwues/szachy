#include"../header/chessPieces.h"
#include <string.h>
#include"../header/interface.h"
#include"../header/game.h"

char boardRepresentation[4][8] = { 0 };

//x = chessPiece2<<4|chessPiece1
const char startingBoard[4][8] =
{
	{53,102,0,0,0,0,238,189}, 
	{66,102,0,0,0,0,238,202},
	{20,102,0,0,0,0,238,156},
	{83,102,0,0,0,0,238,219}
};
/*const char startingBoard[4][8] =
{
	{189,238,0,0,0,0,102,53}, 
	{202,238,0,0,0,0,102,66},
	{156,238,0,0,0,0,102,20},
	{219,238,0,0,0,0,102,83}
};*/

char xPicked=-1, yPicked;

void setDefaultChessBoard() {
	memcpy(boardRepresentation, startingBoard, 4 * 8);
}
char getPieceFromBoard(char x, char y) {
	return (boardRepresentation[x / 2][y] >> !(x % 2) * 4) & 15;
}

char getPositionOfKing(char color) {
	char searchingPiece = KING | color;
	for(char x=0;x<84;x++)
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
		if (xPick  < 6 && yPick  < 6 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick + 1)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
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
		}
		return;
    }
	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? -1 : 1;
		if (!((pieceColor && yPick < 1) || (!pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck=getPieceFromBoard(xPick - 1, yPick + colorSide))&&(pieceCkeck&BLACK)!=pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
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
		if (xPick < 6 && yPick < 6 && ((pieceOnBoard = getPieceFromBoard(xPick + 2, yPick + 1)) == 0 ||  pieceColor!=(pieceOnBoard&BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 2, yPick + 1)) {
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
		}
		return;
	}

	if ((piece & CHESSMASK) == PAWN) {
		char colorSide = pieceColor ? -1 : 1;
		if (!((pieceColor && yPick < 1) || (!pieceColor && yPick > 6))) {
			char pieceCkeck;
			if ((pieceCkeck = getPieceFromBoard(xPick - 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor&&!checkIfCheckOnChangedBoard(pieceColor,xPick,yPick,xPick-1,yPick+colorSide))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick + colorSide))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
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
			if (checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x, y))
				continue;
			if (pieceOnBoard == 0) {
				moveBitMask[y] |= 128 >> x;
				continue;
			}
			else
				if ((pieceOnBoard & BLACK) != pieceColor && (piece & CHESSMASK) != PAWN) {
					captureBitMask[y] |= 128 >> x;
				}
		}
		direction--;
		x = xPick;
		y = yPick;
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
	boardRepresentation[x / 2][y] = (boardRepresentation[x / 2][y] & (240 >> (!(x % 2) * 4)))|(piece<<!(x%2)*4);
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

// :/ 
char checkIfCheck(char color) {
	char kingPos = getPositionOfKing(color);
	char kingPosX = kingPos >> 4;
	char kingPosY = kingPos & 12;
	char moveBitmask[8] = { 0 };
	char captureBitmask[8] = { 0 };
	for(char x=0;x<8;x++)
		for (char y=0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y);
			if ((piece & BLACK)!=color)
			{
				getLegalMovesWithoutCheck(x, y, &moveBitmask, &captureBitmask);
				if (captureBitmask[kingPosY] & (128 >> kingPosX))
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
					if (moveBitmask[i] != 0 || captureBitmask[i] != 0) return 0;
				}
			}
		}
	return !!!check + 1;
}

void pickupPiece(char x, char y) {
	if (xPicked != -1&& isLegalMove(xPicked, yPicked, x, y)) {
		movePiece(xPicked, yPicked, x, y);
		interface_clearBitmask();
		xPicked = -1;
		NextRound();
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

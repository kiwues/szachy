#include"../header/chessPieces.h"
#include <string.h>
#include"../header/interface.h"
#include"../header/game.h"
#include"../header/board.h"

//x = chessPiece2<<4|chessPiece1
/*const char startingBoard[32] =
{
	189,238,0,0,0,0,102,53, 
	202,238,0,0,0,0,102,66,
	156,238,0,0,0,0,102,20,
	219,238,0,0,0,0,102,83
};*/
const char startingBoard[32] =
{
	189,202,156,219,
	238,238,238,238,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	0,0,0,0,
	102,102,102,102,
	53,66,20,83
};

static ChessBoard mainBoard = { .castling=119,.enPassant=0,.check=0,.round=0,.pawnPromotion =-1, .board={ 0 } };

char xPicked=-1, yPicked;


void setPiece(char x, char y, char piece);

void setDefaultChessBoard() {
    //mainBoard = (ChessBoard){ .castling = 119,.enPassant = 0,.check = 0,.round = 0,.pawnPromotion = -1, .board = { 0 } };
    memcpy(&mainBoard.board, startingBoard, 4 * 8);
}

char getPieceFromBoard(char x, char y) {
	return (mainBoard.board[y *4+(x/2)] >> (!(x % 2) * 4)) & 15;
	//return (boardRepresentation[(x / 2)*8+y] >> !(x % 2) * 4) & 15;
}
char getPieceFromBoardPtr(char x, char y, ChessBoard* board) {
	return (board->board[y * 4 + (x / 2)] >> (!(x % 2) * 4)) & 15;
}

char getPositionOfKing(char color, ChessBoard* board) {
	char searchingPiece = KING | color;
	for(char x=0;x<8;x++)
		for (char y = 0; y < 8; y++) {
			if (getPieceFromBoardPtr(x, y,board) == searchingPiece)
				return (x << 4) | y;
		}
}

char checkIfCheck(char color, ChessBoard* board);
char checkIfCheckOnChangedBoard(char color, char xPicked, char yPicked, char xDest, char yDest, ChessBoard* board);

void getLegalMovesWithoutCheck(char xPick, char yPick,char* moveBitMask,char* captureBitMask,ChessBoard*board) {
	char piece = getPieceFromBoardPtr(xPick, yPick,board);
	char pieceColor = piece & BLACK;
    if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoardPtr(x1, y1,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) ) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoardPtr(x2, y2,board)) == 0 || pieceColor != (pieceOnBoard & BLACK))) {
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
			if ((pieceCkeck=getPieceFromBoardPtr(xPick - 1, yPick + colorSide,board))&&(pieceCkeck&BLACK)!=pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoardPtr(xPick + 1, yPick + colorSide,board)) && (pieceCkeck & BLACK) != pieceColor)
				captureBitMask[yPick + colorSide] |= 128 >> (xPick + 1);
			if (board->enPassant && ((board->enPassant & 15) == yPick) && abs((board->enPassant >> 4) - xPick) == 1) {
				captureBitMask[yPick + colorSide] |= 128 >> (board->enPassant >> 4);
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
			char pieceOnBoard = getPieceFromBoardPtr(x, y,board);
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

void getLegalMoves(char xPick, char yPick, char* moveBitMask, char* captureBitMask,ChessBoard* board) {
	char piece = getPieceFromBoardPtr(xPick, yPick,board);
	char pieceColor = piece & BLACK;
	//char counterPieceColor = ~pieceColor&BLACK;
	if ((piece & CHESSMASK) == KNIGHT) {
		char pieceOnBoard;
		for (char i = 0; i < 4; i++) {
			char x1 = xPick + (i % 2 ? -1 : 1);
			char x2 = xPick + (i % 2 ? -2 : 2);
			char y1 = yPick + (i > 1 ? -2 : 2);
			char y2 = yPick + (i > 1 ? -1 : 1);
			if (!(x1 < 0 || x1>7 || y1 < 0 || y1>7) && ((pieceOnBoard = getPieceFromBoardPtr(x1, y1,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x1, y1,board)) {
				if (pieceOnBoard == 0)
					moveBitMask[y1] |= 128 >> (x1);
				else
					captureBitMask[y1] |= 128 >> (x1);
			}
			if (!(x2 < 0 || x2>7 || y2 < 0 || y2>7) && ((pieceOnBoard = getPieceFromBoardPtr(x2, y2,board)) == 0 || pieceColor != (pieceOnBoard & BLACK)) && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, x2, y2,board)) {
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
			if ((pieceCkeck = getPieceFromBoard(xPick - 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor&&!checkIfCheckOnChangedBoard(pieceColor,xPick,yPick,xPick-1,yPick+colorSide,board))
				captureBitMask[yPick + colorSide] |= 128 >> (xPick - 1);
			if ((pieceCkeck = getPieceFromBoard(xPick + 1, yPick + colorSide)) && (pieceCkeck & BLACK) != pieceColor && !checkIfCheckOnChangedBoard(pieceColor, xPick, yPick, xPick + 1, yPick + colorSide,board))
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
			char pieceOnBoard = getPieceFromBoardPtr(x, y,board);
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
			if (!getPieceFromBoardPtr(xPick - 1, yPick,board) && !getPieceFromBoardPtr(xPick - 2, yPick,board) && !getPieceFromBoardPtr(xPick - 3, yPick,board)&&!checkIfCheckOnChangedBoard(piece&BLACK,xPick,yPick,3,yPick,board)&& !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 2, yPick,board))
			{
				moveBitMask[yPick] |= 128 >> 2;
			}
		}
		if ((128 >> ((piece & BLACK) ? 6 : 2)) & board->castling) { //right rook
            if (!getPieceFromBoardPtr(xPick + 1, yPick,board) && !getPieceFromBoardPtr(xPick + 2, yPick,board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 5, yPick, board) && !checkIfCheckOnChangedBoard(piece & BLACK, xPick, yPick, 6, yPick, board))
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
	getLegalMoves(xPick, yPick, &moveMask,&captureMask,&mainBoard);
	return (moveMask[yDest] & (128 >> xDest)|| captureMask[yDest] & (128 >> xDest));
}



void setPiece(char x, char y, char piece) {
	//boardRepresentation[(x / 2)*8+y] = (boardRepresentation[(x / 2)*8+y] & (240 >> (!(x % 2) * 4)))|(piece<<!(x%2)*4);
	mainBoard.board[y * 4 + (x / 2)] &=(240 >> (!(x % 2) * 4));
	mainBoard.board[y * 4 + (x / 2)] |= (piece << !(x % 2) * 4);
}

void setPieceOnBoardPtr(char x, char y, char piece,ChessBoard* board) {
	//boardRepresentation[(x / 2)*8+y] = (boardRepresentation[(x / 2)*8+y] & (240 >> (!(x % 2) * 4)))|(piece<<!(x%2)*4);
	board->board[y * 4 + (x / 2)] &= (240 >> (!(x % 2) * 4));
	board->board[y * 4 + (x / 2)] |= (piece << !(x % 2) * 4);
}

void movePiece(char sourceX, char sourceY, char destinationX, char destinationY) {
	char piece = getPieceFromBoard(sourceX, sourceY);
	setPiece(sourceX, sourceY, 0);
	setPiece(destinationX, destinationY, piece);
	interface_renderMovePiece(sourceX, sourceY, destinationX, destinationY, piece);
}

void movePieceOnBoardPtr(char sourceX, char sourceY, char destinationX, char destinationY, ChessBoard* board) {
	char piece = getPieceFromBoardPtr(sourceX, sourceY,board);
	setPieceOnBoardPtr(sourceX, sourceY, 0,board);
	setPieceOnBoardPtr(destinationX, destinationY, piece,board);
}

ChessBoard* getBoardPtr() {
	return &mainBoard;
}

char checkIfCheck(char color,ChessBoard* board) {
	char kingPos = getPositionOfKing(color,board);
	char kingPosX = kingPos >> 4;
	char kingPosY = kingPos & 15;
	char moveBitmask[8] = { 0 };
	char captureBitmask[8] = { 0 };
	for(char x=0;x<8;x++)
		for (char y=0; y < 8; y++) {
			char piece = getPieceFromBoardPtr(x, y,board);
			if ((piece & BLACK)!=color)
			{
				getLegalMovesWithoutCheck(x, y, &moveBitmask, &captureBitmask,board);
				if ((captureBitmask[kingPosY] & (128 >> kingPosX)))
					return 1;
			}
		}
	return 0;
}

char checkIfCheckOnChangedBoard(char color, char xPicked, char yPicked, char xDest, char yDest,ChessBoard *board) {
	char picked = getPieceFromBoardPtr(xPicked, yPicked,board);
	char dest = getPieceFromBoardPtr(xDest, yDest,board);
	setPieceOnBoardPtr(xPicked, yPicked, 0,board);
	setPieceOnBoardPtr(xDest, yDest, picked, board);
	char ret = checkIfCheck(color,board);
	setPieceOnBoardPtr(xPicked, yPicked, picked, board);
	setPieceOnBoardPtr(xDest, yDest, dest, board);
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
				getLegalMoves(x, y, &moveBitmask, &captureBitmask,&mainBoard);
				for (int i = 0; i < 8; i++) {
					if (moveBitmask[i] != 0 || captureBitmask[i] != 0) 
						return 0;
				}
			}
		}
	return !!!mainBoard.check + 1;
}



void pickupPiece(char x, char y) {
	if (mainBoard.pawnPromotion!=-1) {
		if ((y != -1 && y  != 8)) return;
		setPiece(mainBoard.pawnPromotion >> 4, mainBoard.pawnPromotion & 15, ((mainBoard.pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0));
		interface_clearPawnPromotion();
		//interface_renderPiece(pawnPromotion >> 4, pawnPromotion & 15);
		NextRound();
	}else
	if (xPicked != -1&& isLegalMove(xPicked, yPicked, x, y)) {
		char piece = getPieceFromBoard(xPicked, yPicked);
		movePiece(xPicked, yPicked, x, y);
		if (mainBoard.enPassant) {
			if ((piece & CHESSMASK) == PAWN && (getPieceFromBoard(x, y + ((piece & BLACK) ? -1 : 1))&CHESSMASK)==PAWN) {
				setPiece(x, y + ((piece & BLACK) ? -1 : 1), 0);
				interface_renderPiece(x, y + ((piece & BLACK) ? -1 : 1));
			}
			mainBoard.enPassant = 0;
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
					mainBoard.castling &= ~((64)>> (piece & BLACK ? 4 : 0));
				}
				else if (xPicked == 7) {
					mainBoard.castling &= ~((32) >> (piece & BLACK ? 4 : 0));
				}
			}
			else if ((piece & CHESSMASK) == KING) {
				mainBoard.castling &= 240 >> (piece & BLACK ? 0 : 4);
			}
			else if ((piece & CHESSMASK) == PAWN&&abs(yPicked-y)==2) {
				mainBoard.enPassant = (x << 4) | y;
			}
		}
		xPicked = -1;

	} else{
		char piece = getPieceFromBoard(x, y);
		if (!piece || (piece >> 3) != mainBoard.round) {
			return;
		}
		interface_clearBitmask();
		interface_showLegalMovesOfCurrentPiece(x,y);
		xPicked = x;
		yPicked = y;
	}
}
void pickupAndPlacePiece(char xSrc, char ySrc, char xDst, char yDst) {
	
	pickupPiece(xSrc, ySrc);
	pickupPiece(xDst, yDst);
}
void simulatePieceMoveOnBoardPtr(char xSrc, char ySrc, char xDst, char yDst, ChessBoard* board) {
	char piece = getPieceFromBoardPtr(xSrc, ySrc,board);
	movePieceOnBoardPtr(xSrc, ySrc, xDst, yDst,board);
	if (board->enPassant) {
		if ((piece & CHESSMASK) == PAWN && (getPieceFromBoardPtr(xDst, yDst + ((piece & BLACK) ? -1 : 1),board) & CHESSMASK) == PAWN) {
			setPieceOnBoardPtr(xDst, yDst + ((piece & BLACK) ? -1 : 1), 0,board);
		}
		board->enPassant = 0;
	}
	if ((piece & CHESSMASK) == KING) {
		if (xDst - xSrc == 2) { //castle wtith right rook
			movePieceOnBoardPtr(7, ySrc, 5, ySrc,board);
		}
		else if (xSrc - xDst == 2) {
			movePieceOnBoardPtr(0, ySrc, 3, ySrc,board);
		}
	}
	if ((yDst == 0 || yDst == 7) && (piece & CHESSMASK) == PAWN) {
		setPieceOnBoardPtr(xDst, yDst, QUEEN | (yDst == 7 ? BLACK : 0), board);
	}
	else {
		if ((piece & CHESSMASK) == ROOK) {
			if (xSrc == 0) {
				board->castling &= ~((64) >> (piece & BLACK ? 4 : 0));
			}
			else if (xSrc == 7) {
				board->castling &= ~((32) >> (piece & BLACK ? 4 : 0));
			}
		}
		else if ((piece & CHESSMASK) == KING) {
			board->castling &= 240 >> (piece & BLACK ? 0 : 4);
		}
		else if ((piece & CHESSMASK) == PAWN && abs(ySrc - yDst) == 2) {
			board->enPassant = (xDst << 4) | yDst;
		}
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
	getLegalMoves(x, y, moveMask, captureMask,&mainBoard);
	int amount = 0;
	for (char i = 0; i < 8; i++) {
		amount += BitCount(moveMask[i]) ;//+ BitCount(captureMask[i])
	}
	return amount;
}

int getAmountOfLegalMovesOnBoardPtr(char x, char y, char* moveMask, char* captureMask,ChessBoard* board)
{
	getLegalMoves(x, y, moveMask, captureMask,board);
	int amount = 0;
	for (char i = 0; i < 8; i++) {
		amount += BitCount(moveMask[i]);//+ BitCount(captureMask[i])
	}
	return amount;
}
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
	board->castling = 119;
	board->enPassant = 0;
	board->check = normal;
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
	index = __builtin_ctzll(*bitmap);
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

GameState checkIfCheck(char color, ChessBoard* board);

void getLegalMoves(char xPick, char yPick, uint64_t* moveBitmask, uint64_t* captureBitmask, ChessBoard* board) {
	char piece = getPieceFromBoard(xPick, yPick, board);

	getMaskOfPiece(piece, xPick, yPick, moveBitmask, captureBitmask, board);

	*moveBitmask = (*moveBitmask & ~board->allBitmap);
	*captureBitmask = *captureBitmask & (piece & BLACK ? board->whiteBitmaps.all : board->blackBitmaps.all);
	if ((piece & CHESSMASK) == PAWN && board->enPassant && abs(board->enPassant - (xPick + yPick * 8)) == 1) {
		*captureBitmask |= 1ull << (board->enPassant + ((piece & BLACK) ? 8 : -8));
	}

	if ((board->round ? board->blackBitmaps.lockedPieces : board->whiteBitmaps.lockedPieces) & (1ull << (xPick + yPick * 8))) {
		*moveBitmask &= ((piece&BLACK) ? board->blackBitmaps.lockedPieces_move : board->whiteBitmaps.lockedPieces_move);
		*captureBitmask &= ((piece&BLACK) ? board->blackBitmaps.lockedPieces_move : board->whiteBitmaps.lockedPieces_move);
	}


	if ((piece & CHESSMASK) == KING) {
		uint64_t _moveBitmask = 0ull;
		uint64_t _captureBitmask = 0ull;
		uint64_t* _allMapPtr = (piece & BLACK) ? &board->blackBitmaps.all : &board->whiteBitmaps.all;
		uint64_t _allMap = *_allMapPtr;

		*_allMapPtr ^= 1ull << (xPick + yPick * 8);
		uint64_t attackMask = 0ull;
		for (char i = 0; i < 64; i++) {
			if (board->board[i] && (board->board[i] & BLACK) != (piece & BLACK) && ((i % 8) != xPick && (i / 8) != yPick)) {
				if ((board->board[i] & CHESSMASK) == KING) {
					getMaskOfPiece(board->board[i], i % 8, i / 8, &_moveBitmask, &_captureBitmask, board);
				}else
				getLegalMoves(i % 8, i / 8, &_moveBitmask, &_captureBitmask, board);
				attackMask|= _captureBitmask;
				attackMask |= _moveBitmask;
			}
		}
		*moveBitmask &= ~attackMask;
		*captureBitmask &= ~attackMask;
		*_allMapPtr ^= 1ull << (xPick + yPick * 8);
	}

	if (board->check!=normal) {
		if ((piece&BLACK)) {
			if ((piece & CHESSMASK) != KING)
				*moveBitmask &= board->blackBitmaps.squaresToDefend;
			else if(board->blackBitmaps.squaresToDefend != UINT64_MAX)
				*moveBitmask &= ~board->blackBitmaps.squaresToDefend;
			*captureBitmask &= board->blackBitmaps.squaresToDefend;
		}
		else {
			if ((piece & CHESSMASK) != KING)
				*moveBitmask &= board->whiteBitmaps.squaresToDefend;
			else if(board->whiteBitmaps.squaresToDefend!=UINT64_MAX){
				*moveBitmask &= ~board->whiteBitmaps.squaresToDefend;
			}
			*captureBitmask &= board->whiteBitmaps.squaresToDefend;

		}
	}
	return;
}

char isLegalMove(char xPick, char yPick, char xDest, char yDest,ChessBoard* board) {
	uint64_t moveMask = 0ull;
	uint64_t captureMask =  0ull;
	getLegalMoves(xPick, yPick, &moveMask,&captureMask,board);
	return !!((moveMask | captureMask) & (1ull << (xDest + yDest * 8)));
}

void setPieceOnBoard(char x, char y, char piece,ChessBoard* board) {
	board->board[y * 8 + x ] = piece;
}

void ChangeBitboard(char piecePicked, char x, char y, char withAllBitboardsUpate, ChessBoard* board);

void movePieceOnBoard(char sourceX, char sourceY, char destinationX, char destinationY, ChessBoard* board) {
	char piece = getPieceFromBoard(sourceX, sourceY,board);
	char piece2 = getPieceFromBoard(destinationX, destinationY, board);
	setPieceOnBoard(sourceX, sourceY, 0,board);
	ChangeBitboard(piece, sourceX, sourceY ,0,board);
	if (piece2 != 0) ChangeBitboard(piece2, destinationX, destinationY, 0, board);
	setPieceOnBoard(destinationX, destinationY, piece,board);
	ChangeBitboard(piece, destinationX, destinationY,1, board);
}

uint64_t getLineBetween(char xFrom,char yFrom,char xTo, char yTo) {// 4,4 to 4,0
	//jest lepszy sposob na to
	uint64_t mask = 0ull;
	int8_t xDelta = (xTo - xFrom);
	if(xDelta!=0)
	xDelta /= abs(xDelta);
	int8_t yDelta = yTo - yFrom;
	if (yDelta != 0)
	yDelta /= abs(yDelta);
	for (char x = xFrom, y = yFrom; (x != xTo || y != yTo); mask |= 1ull << (y * 8 + x),x += xDelta, y += yDelta);//&&(!getPieceFromBoard(x,y,board))
	return mask;
}

uint64_t createAttackersBitboard(char kingIndex,uint64_t sliderAttackers,uint64_t* defenderAllPieces){
	uint64_t mask=0ull;
	uint64_t temp=0ull;
	while (sliderAttackers) {
		char index = getIndexOfFirstBit(&sliderAttackers);
		temp = getLineBetween(index % 8, index / 8,kingIndex % 8, kingIndex / 8);
		if (*defenderAllPieces != 0 ) {
			if (MaskBitCount(temp & *defenderAllPieces)==1) mask |= temp;
		}
		else {
			mask |= temp;
		}
		
		sliderAttackers &= (sliderAttackers - 1);
	}
	return mask;
}



GameState checkIfCheck(char color,ChessBoard* board) {
	unsigned char kingPos = getSquareOfKing(color,board);
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

	uint64_t attackers2 = 0ull;
	uint64_t attackLine=createAttackersBitboard(kingPos,mask[2]|mask[3],&attackers2);//squares to block
	attackLine |= attackers;

	board->allBitmap = color ? board->whiteBitmaps.all : board->blackBitmaps.all;
	uint64_t lockedPieces = 0ull;
	getMaskOfPiece(BISHOP, kingPosX, kingPosY, &temp, &mask[2], board);
	getMaskOfPiece(ROOK, kingPosX, kingPosY, &temp, &mask[3], board);
	attackers2 = (mask[2] & (color?board->whiteBitmaps.bishop| board->whiteBitmaps.queen :board->blackBitmaps.bishop| board->blackBitmaps.queen))|(mask[3] & (color ? board->whiteBitmaps.rook| board->whiteBitmaps.queen : board->blackBitmaps.rook| board->blackBitmaps.queen));//attackers but blocked

	lockedPieces = createAttackersBitboard(kingPos,attackers2,(color?&board->blackBitmaps.all:&board->whiteBitmaps.all));

	board->allBitmap = board->blackBitmaps.all | board->whiteBitmaps.all;


	if(color){
		board->blackBitmaps.lockedPieces = lockedPieces & board->blackBitmaps.all;
		board->blackBitmaps.lockedPieces_move = lockedPieces ;
		board->blackBitmaps.squaresToDefend = (attackLine ) ? (attackLine ) : UINT64_MAX;
	}
	else {
		board->whiteBitmaps.lockedPieces = lockedPieces &  board->whiteBitmaps.all;
		board->whiteBitmaps.lockedPieces_move = lockedPieces;
		board->whiteBitmaps.squaresToDefend = ((attackLine) ? (attackLine) : UINT64_MAX);
	}
	board->check = color?(attackers?black_check:normal):(attackers?white_check:normal);
	board->check = checkIfCheckmateOrStalemate(color, !!attackers,board);
	return board->check;
}

GameState checkIfCheckmateOrStalemate(char color, char check,ChessBoard* board) {
	uint64_t moveBitmask = 0ull;
	uint64_t captureBitmask = 0ull;
	for(char x=0;x<8;x++)
		for (char y = 0; y < 8; y++) {
			char piece = getPieceFromBoard(x, y,board);
			if (!!(piece & BLACK) == color)
			{
				getLegalMoves(x, y, &moveBitmask, &captureBitmask,board);
				if ((moveBitmask | captureBitmask)) return color?(check?black_check:normal):(check?white_check:normal);
			}
		}

	return color?(check?black_checkmate:black_stalemate):(check?white_checkmate:white_stalemate);
}

void PromotePawn(char x, char y, char piece, ChessBoard* board) {
	setPieceOnBoard(x, y, piece, board);
	ChangeBitboard(piece, x, y, 0, board);
	ChangeBitboard(PAWN | (y == 7 ? BLACK : 0), x, y, 1, board);
}



void pickupPiece(char x, char y, ChessBoard* board) {
	if (board->pawnPromotion!=-1) {
		if ((y != -1 && y  != 8)) return;
		PromotePawn(board->pawnPromotion >> 4, board->pawnPromotion & 15, ((board->pawnPromotion >> 4) + 3 - x) | (y == 8 ? BLACK : 0), board);
		interface_clearPawnPromotion();
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
			NextRound();
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
void simulatePieceMoveOnBoard(Move* move, ChessBoard* board) {
	char piece = getPieceFromBoard(move->xFrom, move->yFrom, board);
	movePieceOnBoard(move->xFrom, move->yFrom, move->xTo, move->yTo, board);
	if (move->promotionPiece!=20) {
		PromotePawn(move->xTo, move->yTo, move->promotionPiece, board);
		return;
	}
	if (board->enPassant) {
		if ((piece & CHESSMASK) == PAWN && (getPieceFromBoard(move->xTo, move->yTo + ((piece & BLACK) ? -1 : 1), board) & CHESSMASK) == PAWN) {
			setPieceOnBoard(move->xTo, move->yTo + ((piece & BLACK) ? -1 : 1), 0, board);
			ChangeBitboard(piece ^ BLACK, board->enPassant % 8, board->enPassant / 8, 1, board);
		}
		board->enPassant = 0;
	}
	if ((piece & CHESSMASK) == KING) {
		if (move->xTo - move->xFrom == 2) { //castle wtith right rook
			movePieceOnBoard(7, move->yFrom, 5, move->yFrom, board);
		}
		else if (move->xFrom - move->xTo == 2) {//castle wtith left rook
			movePieceOnBoard(0, move->yFrom, 3, move->yFrom, board);
		}
	}
	if ((piece & CHESSMASK) == ROOK) {
		if (move->xFrom == 0) {
			board->castling &= ~(1 << ((piece & BLACK) ? 6 : 2));
		}
		else if (move->xFrom == 7) {
			board->castling &= ~(1 << ((piece & BLACK) ? 5 : 1));
		}
	}
	else if ((piece & CHESSMASK) == KING) {
		board->castling &= 7 << ((piece & BLACK) ? 0 : 4);
	}
	else if ((piece & CHESSMASK) == PAWN && abs(move->yFrom - move->yTo) == 2) {
		board->enPassant = move->xTo + move->yTo * 8;
	}
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
            bitBoardPicked = (piecePicked & BLACK) ? &board->blackBitmaps.pawn : &board->whiteBitmaps.pawn;
            break;
        case KNIGHT:
            bitBoardPicked = (piecePicked & BLACK) ? &board->blackBitmaps.knight : &board->whiteBitmaps.knight;
            break;
        case BISHOP:
            bitBoardPicked = (piecePicked & BLACK) ? &board->blackBitmaps.bishop : &board->whiteBitmaps.bishop;
            break;
        case ROOK:
            bitBoardPicked = (piecePicked & BLACK) ? &board->blackBitmaps.rook : &board->whiteBitmaps.rook;
            break;
        case QUEEN:
            bitBoardPicked = piecePicked & BLACK ? &board->blackBitmaps.queen : &board->whiteBitmaps.queen;
            break;
        case KING:
            bitBoardPicked = (piecePicked & BLACK) ? &board->blackBitmaps.king : &board->whiteBitmaps.king;
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

element_listy* GetAllMovesFor(char color, ChessBoard* board) {
	element_listy* head = (element_listy*)calloc(1, sizeof(element_listy));
	uint64_t moveBitmask = 0ull;
	uint64_t captureBitmask = 0ull;
	char piece = 0;
	for (int i = 0; i < 64; i++) {
		piece = board->board[i];
		if ((!!(piece & BLACK)) == color) {
			getLegalMoves(i % 8, i / 8, &moveBitmask, &captureBitmask, board);
			while (captureBitmask) {
				int moveIndex = getIndexOfFirstBit(&captureBitmask);
				captureBitmask &= (captureBitmask - 1);
				Move move = { .xFrom = i % 8,.yFrom = i / 8,.xTo = moveIndex % 8,.yTo = moveIndex / 8 ,.promotionPiece=20};
				lista_dodaj(head, move);
			}
			while (moveBitmask) {
				int moveIndex = getIndexOfFirstBit(&moveBitmask);
				moveBitmask &= (moveBitmask - 1);
				Move move = { .xFrom = i % 8,.yFrom = i / 8,.xTo = moveIndex % 8,.yTo = moveIndex / 8,.promotionPiece=20 };
				lista_dodaj(head, move);
			}
			captureBitmask = 0;
			moveBitmask = 0;
		}
	}
	head = lista_usun_ity(head, 0);
	return head;
}
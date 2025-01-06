#include<string.h>

void getMaskOfPiece(char piece,char x, char y,char* maskArray) {
	switch (piece&7)
	{
	case 1://KING
	{
		if (x == 0) {
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
		}
	}
		break;
	case 2://QUEEN
	{
		for (int i = 0; i < y; i++) {
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
		}
	}
		break;
	case 3://ROOK
		for (int i = 0; i < 8; i++) {
			if (i == y)continue;
			maskArray[i] |= 128 >> x;
		}
		for (int i = 0; i < 8; i++) {
			if (i == x)continue;
			maskArray[y] |= 128 >> i;
		}
		break;
	case 4://BISHOP
		for (int i = 0; i < y; i++) {
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
		}
		break;
	case 5://KNIGHT
		if (x < 1) {
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
		}
		break;
	case 6://PAWN
		if (!(piece & 8)) {
			if (y > 0) {
				maskArray[y - 1] = 128 >> x;
				if (y == 6)
					maskArray[y - 2] = 128 >> x;
			}
		}
		else {
			if (y < 7) {
				maskArray[y + 1] = 128 >> x;
				if (y == 1)
					maskArray[y + 2] = 128 >> x;
			}
		}
		break;
	default:
		break;
	}
}
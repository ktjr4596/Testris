#include "gameboard.h"
#include "gotoxy.h"
#include "inputclass.h"

int gameboard::id = 0;
const char * tile[] = { " ","□","■" } ;

extern Point shape[][4][4] = {
	{ { 0,0,1,0,2,0,-1,0 },{ 0,0,0,1,0,-1,0,-2 },{ 0,0,1,0,2,0,-1,0 },{ 0,0,0,1,0,-1,0,-2 } },

{ { 0,0,1,0,0,1,1,1 },{ 0,0,1,0,0,1,1,1 },{ 0,0,1,0,0,1,1,1 },{ 0,0,1,0,0,1,1,1 } },

{ { 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,1,-1,0,-1,-1 },{ 0,0,-1,0,0,-1,1,-1 },{ 0,0,0,1,-1,0,-1,-1 } },

{ { 0,0,-1,-1,0,-1,1,0 },{ 0,0,-1,0,-1,1,0,-1 },{ 0,0,-1,-1,0,-1,1,0 },{ 0,0,-1,0,-1,1,0,-1 } },

{ { 0,0,-1,0,1,0,-1,-1 },{ 0,0,0,-1,0,1,-1,1 },{ 0,0,-1,0,1,0,1,1 },{ 0,0,0,-1,0,1,1,-1 } },

{ { 0,0,1,0,-1,0,1,-1 },{ 0,0,0,1,0,-1,-1,-1 },{ 0,0,1,0,-1,0,-1,1 },{ 0,0,0,-1,0,1,1,1 } },

{ { 0,0,-1,0,1,0,0,-1 },{ 0,0,-1,0,0,-1,0,1 },{ 0,0,-1,0,1,0,0,1 },{ 0,0,0,-1,0,1,1,0 } },

};

gameboard::gameboard() {
	id++;

	for (int x = 0; x<BW + 2; ++x) {

		for (int y = 0; y<BH + 2; ++y) {

			board[x][y].block = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
			board[x][y].color = gtxy::Black;
		}

	}
	if (id == 1) {
		nx = (BX + BW) / 2;
		ny = 3;
		ax = BW / 2;
		ay = 3;
		DrawNextBoard();
	}
	else if (id > 1) {
		//((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2
		//nx = BW + BX + 8+((BW+BX+2)*(id-1)/2);
		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ny = 3;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ay = 3;
	}
	score = 0;
	DrawScreen();

}
//gameboard::gameboard(int pid) {
//	id++;
//
//	playerid = pid;
//	for (int x = 0; x<BW + 2; ++x) {
//
//		for (int y = 0; y<BH + 2; ++y) {
//
//			board[x][y].block = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
//			board[x][y].color = gtxy::Black;
//
//		}
//
//	}
//	nx = id*BW / 2;
//	ny = 3;
//	score = 0;
//	DrawScreen();
//}
gameboard::gameboard(gameboard&& rhs) noexcept {
;
	
	nx = rhs.nx;
	ny = rhs.ny;
	ax = rhs.ax;
	ay = rhs.ay;
	memcpy(board, rhs.board, sizeof(boardinfo)*((BH + 2)*(BW + 2)));

	//printf("%d\n", id);
	DrawScreen();
}
gameboard& gameboard::operator=(gameboard && rhs)noexcept {
	if (this == &rhs) {
		return *this;
	}

	id = rhs.id;
	nx = rhs.nx;
	ny = rhs.ny;
	ax = rhs.ax;
	ay = rhs.ay;
	memcpy(board, rhs.board, sizeof(boardinfo)*(BH + 2)*(BW + 2));
	printf("operator=");
	return *this;
}
void gameboard::DrawScreen() {
	for (int x = 0; x < BW + 2; ++x) {
		for (int y = 0; y < BH + 2; ++y) {
			if (id == 1)
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX * (id)+x * 2, BY + y);
			else if (id>1)
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2, BY + y);
			puts(tile[board[x][y].block]);
		}
	}
	gtxy::gotoxy(BX, BY + BH + 5);
	puts("score : ");
}

void gameboard::Printbrick(bool show, short brick, short rot) {

	std::unique_lock<std::mutex> l(printm);
	for (int i = 0; i < 4; ++i) {
		gtxy::gotoxy(BX + (shape[brick][rot][i].x + nx) * 2, BY + shape[brick][rot][i].y + ny);
		puts(tile[show ? BRICK : EMPTY]);
	}
	l.unlock();
}
void gameboard::Printbricka(bool show, short brick, short rot) {

	for (int i = 0; i < 4; ++i) {
		gtxy::gotoxy(BX + (shape[brick][rot][i].x + ax) * 2, BY + shape[brick][rot][i].y + ay);
		puts(tile[show ? WALL : EMPTY]);
	}

}
void gameboard::DrawBoard() {

	int x, y;



	for (x = 1; x<BW + 1; x++) {

		for (y = 1; y<BH + 1; y++) {

			gtxy::gotoxy(BX + x * 2, BY + y);
			gtxy::ClearConsoleToColors(board[x][y].color, gtxy::Black);
			puts(tile[board[x][y].block]);

		}

	}


}

void gameboard::DrawNextBoard() {
	for (short x = BW + BX; x < BW + BX + 8; ++x) {
		for (short y = BY; y < BY + 8; ++y) {
			if (x == BW + BX || x == BW + BX + 7 || y == BY || y == BY + 7) {
				gtxy::gotoxy(x * 2, y);
				puts(tile[2]);
			}
		}
	}
}

void gameboard::DrawNextBrick(bool show, short brick) {
	for (int i = 0; i < 4; ++i) {
		gtxy::gotoxy((shape[brick][0][i].x + BX + BW + 3) * 2, BY + shape[brick][0][i].y + 4);
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		puts(tile[show ? BRICK : EMPTY]);
	}
}

void gameboard::Printscore()
{
	gtxy::gotoxy(BX + 10, BY + BH + 5);
	puts("                      ");
	gtxy::gotoxy(BX + 10, BY + BH + 5);
	gtxy::ClearConsoleToColors(gtxy::White, gtxy::Black);
	printf("%d", score);
}

void gameboard::setxy(const short id) {
	if (id == 0) {
		nx = (BW) / 2;
		ny = 3;
		ax = (BW) / 2;
		ay = 3;
	}
	else if (id > 0) {
		//((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2
		//nx = BW + BX + 8+((BW+BX+2)*(id-1)/2);
		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ny = 3;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ay = 3;
	}
}
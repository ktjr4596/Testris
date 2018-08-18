#include "gameboard.h"
#include "gotoxy.h"
#include "inputclass.h"
#include <conio.h>

int gameboard::playerid = 0;
const char * tile[] = { " ","□","■"," "," "," "," " } ;

extern Point shape[][4][4] = {
	{ { 0,0,1,0,2,0,-1,0 },{ 0,-1,0,0,0,-2,0,-3 },{ 0,0,1,0,2,0,-1,0 },{ 0,-1,0,0,0,-2,0,-3 } },//-

{ { 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 } },//ㅁ

{ { 0,0,-1,0,0,-1,1,-1 },{ 0,-1,0,0,-1,-1,-1,-2 },{ 0,0,-1,0,0,-1,1,-1 },{ 0,-1,0,0,-1,-1,-1,-2 } },//ㄴㄱ
//0,0,0,1,-1,0,-1,-1 
{ { 0,0,-1,-1,0,-1,1,0 },{ 0,-1,-1,-1,-1,0,0,-2 },{ 0,0,-1,-1,0,-1,1,0 },{ 0,-1,-1,-1,-1,0,0,-2 } },//ㄱㄴ

{ { 0,0,-1,0,1,0,-1,-1 },{ 0,-1,0,-2,0,0,-1,0 },{ 0,-1,-1,-1,1,-1,1,0 },{ 0,-1,0,-2,0,0,1,-2 } },//ㄴreverse

{ { 0,0,1,0,-1,0,1,-1 },{ 0,-1,0,0,0,-2,-1,-2 },{ 0,-1,1,-1,-1,-1,-1,0 },{ 0,-1,0,-2,0,0,1,0 } },//ㄴ

{ { 0,0,-1,0,1,0,0,-1 },{ 0,-1,-1, -1,0,-2,0,0 },{ 0,-1,-1,-1,1,-1,0,0 },{ 0,-1,0,-2,0,0,1,-1 } },//ㅗ

};

gameboard::gameboard() {
	playerid++;
	id = playerid;
	
	for (int x = 0; x<BW + 2; ++x) {

		for (int y = 0; y<BH + 2; ++y) {

			board[x][y].block = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
			board[x][y].color = gtxy::Black;
		}

	}
	if (id == 1) {
		nx = (BX + BW) / 2;
		ny = 4;
		ax = BW / 2;
		ay = 4;
		brick = 0;
		rot = 0;
		mtrig = false;
		DrawNextBoard();
	}
	else if (id > 1) {
		//((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2
		//nx = BW + BX + 8+((BW+BX+2)*(id-1)/2);
		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ny = 4;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ay = 4;
		brick = 0;
		rot = 0;
		mtrig = false;
	}
	for (int i = 0; i < 4; ++i) {
		p[i] = { 0, 0, 3 };
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

	id = rhs.id;
	nx = rhs.nx;
	ny = rhs.ny;
	ax = rhs.ax;
	ay = rhs.ay;
	memcpy(board, rhs.board, sizeof(boardinfo)*((BH + 2)*(BW + 2)));
	brick = 0;
	rot = 0;
	mtrig = false;
	
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
	brick = rhs.brick;
	rot = rhs.rot;
	mtrig = rhs.mtrig;
	return *this;
}
void gameboard::DrawScreen() {
	for (int x = 0; x < BW + 2; ++x) {
		for (int y = 0; y < BH + 2; ++y) {
			if (board[x][y].block==WALL&&id == 1)
			{
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX * (id)+x * 2, BY + y);
				puts(tile[2]);
			}
			else if (board[x][y].block == WALL&&id > 1)
			{
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2, BY + y);
				puts(tile[2]);
			}//puts(tile[board[x][y].block]);
		}
	}
	gtxy::gotoxy(BX, BY + BH + 5);
	puts("score : ");
}
void gameboard::printnextbrick(const short brick,const short rot)
{
	short cnt = 0;
	short cntflag = 0;

	int j = 1;
	short tnx = 0;
	std::unique_lock<std::mutex> l(printm, std::defer_lock);
	for (int i = 0; i < 4; ++i) {
		if (id == 1) {
			//l.lock();
			board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1;
			//l.unlock();
		}//n[i] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y  ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
		else if (id > 1) {
			board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny +
				shape[brick][rot][i].y].block += 1;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (id == 1) {
			if (p[i].state == board[p[i].x][p[i].y].block) {
				l.lock();
				board[p[i].x][p[i].y].block = EMPTY;
				
				gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
				puts(tile[0]);
				l.unlock();
			}
			else {
				p[i].state = board[p[i].x][p[i].y].block;
				n[cnt++] = p[i];
			}
		}
		else if (id > 1) {
			if (p[i].state == board[p[i].x][p[i].y].block) {
				board[p[i].x][p[i].y].block = EMPTY;
				l.lock();
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + ((p[i].x) + static_cast<short>(28 + 22 * (id - 2))) * 2, BY + p[i].y);
				puts(tile[0]);
				l.unlock();
			}
			else {
				p[i].state = board[p[i].x][p[i].y].block;
				n[cnt++] = p[i];
			}
		}
	}
	for (int i = 0; i < cnt; ++i) {
		p[i] = n[i];
	}
	for (int i = 0; i < 4; ++i) {
		if (tnx != cnt) {
			for (int k = 0; k < cnt; ++k) {
				if (id == 1) {
					if ((nx + shape[brick][rot][i].x) == p[k].x && (ny + shape[brick][rot][i].y) == p[k].y) {
						cntflag++;
						tnx++;
						break;
					}
				}
				else if (id > 1) {

					if ((nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x) == p[k].x && (ny + shape[brick][rot][i].y) == p[k].y) {
						cntflag++;
						tnx++;
						break;
					}
				}

			}
		}
		if (cntflag > 0) {
			cntflag = 0; continue;
		}
		if (id == 1) {
			p[cnt + j - 1] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			l.lock();
			gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
			gtxy::gotoxy(BX + (p[cnt + j - 1].x) * 2, BY + p[cnt + j - 1].y);
			puts(tile[1]);
			l.unlock();
		}
		else if (id > 1) {
			p[cnt + j - 1] = { nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			l.lock();
			gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
			gtxy::gotoxy(BX + ((p[cnt + j - 1].x) + static_cast<short>(28 + 22 * (id - 2))) * 2, BY + p[cnt + j - 1].y);
			puts(tile[1]);
			l.unlock();
		}
		j++;
	}
}
void gameboard::Printbrick(bool show, short brick, short rot) {
	

	std::unique_lock<std::mutex> l(printm, std::defer_lock);
	if (show) {
		for (int i = 0; i < 4; ++i) {

			if (id == 1) {
				board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1;
				p[i] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			}
			else if (id > 1) {
				board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1;
				p[i] = { nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };

			}
		}
		for (int i = 0; i < 4; ++i) {

			if (id == 1) {
				l.lock();
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
				std::puts(tile[1]);
				l.unlock();
			}
			else if (id > 1) {
				l.lock();
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + ((p[i].x) + static_cast<short>(28 + 22 * (id - 2))) * 2, BY + p[i].y);
				std::puts(tile[1]);
				l.unlock();
			}


		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			board[p[i].x][p[i].y].block = EMPTY;
			gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
			std::puts(tile[0]);
		}
		for (int i = 0; i < 4; ++i) {
			board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1;
			//n[i] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			p[i] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
			std::puts(tile[1]);
		}
	}
	//sm.unlock_shared();
	
}
void gameboard::Printbricka(bool show, short brick, short rot) {
	std::unique_lock<std::mutex> l(printm,std::defer_lock);
	l.lock();
	short cntflag = 0;
	if (show) {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (((shape[brick][rot][i].x + ax) == (shape[brick][rot][i].x + nx)) && ((shape[brick][rot][i].y + ay) == (shape[brick][rot][i].x + ny)))
				{
					cntflag++;
					break;
				}

			}
			if (cntflag == 0)
			{
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + (shape[brick][rot][i].x + ax) * 2, BY + shape[brick][rot][i].y + ay);
				puts(tile[show ? WALL : EMPTY]);
			}
		}
	}
	else {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				if (((shape[brick][rot][i].x + ax) == (shape[brick][rot + 1][i].x + nx)) && ((shape[brick][rot ][i].y + ay) == (shape[brick][rot+1][i].x + ny)))
				{
					cntflag++;
					break;
				}

			}
			//for (int j = 0; j < 4; ++j) {
			//	if ((shape[brick][rot ][i].x + ax) == p[j].x && (shape[brick][rot ][i].y + ay) == p[j].y)
			//	{
			//		cntflag++;
			//		break;
			//	}

			//}
			if (cntflag == 0)
			{
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + (shape[brick][rot][i].x + ax) * 2, BY + shape[brick][rot][i].y + ay);
				puts(tile[show ? WALL : EMPTY]);
			}
		}
	}
	l.unlock();
	

}
void gameboard::DrawBoard() {

	int x, y;

	std::unique_lock<std::mutex> l(printm,std::defer_lock);
	
	for (x = 1; x<BW + 1; x++) {

		for (y = 1; y<BH + 1; y++) {
			//if (board[x][y].block == EMPTY && board[x][y + 1].block == EMPTY) continue;
			l.lock();
			gtxy::gotoxy(BX + x * 2, BY + y);
			gtxy::ClearConsoleToColors(board[x][y].color, gtxy::Black);
			puts(tile[board[x][y].block]);
			l.unlock();
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
	std::unique_lock<std::mutex> l(printm, std::defer_lock);
	l.lock();
	for (int i = 0; i < 4; ++i) {
		
		gtxy::gotoxy((shape[brick][0][i].x + BX + BW + 3) * 2, BY + shape[brick][0][i].y + 4);
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		puts(tile[show ? BRICK : EMPTY]);
		
	}
	l.unlock();
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
		ny = 4;
		ax = (BW) / 2;
		ay = 4;
	}
	else if (id > 0) {
		//((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2
		//nx = BW + BX + 8+((BW+BX+2)*(id-1)/2);
		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ny = 4;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ay = 4;
	}
}
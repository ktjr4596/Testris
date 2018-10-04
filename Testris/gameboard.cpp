#include "gameboard.h"
#include "gotoxy.h"
#include "inputclass.h"
#include <conio.h>

int gameboard::playerid = 0;
const char * tile[] = { "▨","■","▣"," "," "," "," "," ", }; // 공격받아 쌓이는 블럭, 놓아진 블럭 및 현재내려오는 블럭, 벽, 빈공간...
std::mutex m[4];
extern Point shape[][4][4] = {
	{ { 0,0,1,0,2,0,-1,0 },{ 0,-1,0,0,0,-2,0,-3 },{ 0,0,1,0,2,0,-1,0 },{ 0,-1,0,0,0,-2,0,-3 } },//-

{ { 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 },{ 0,-1,1,-1,0,0,1,0 } },//ㅁ

{ { 0,0,-1,0,0,-1,1,-1 },{ 0,-1,0,0,-1,-1,-1,-2 },{ 0,0,-1,0,0,-1,1,-1 },{ 0,-1,0,0,-1,-1,-1,-2 } },//ㄴㄱ

{ { 0,0,-1,-1,0,-1,1,0 },{ 0,-1,-1,-1,-1,0,0,-2 },{ 0,0,-1,-1,0,-1,1,0 },{ 0,-1,-1,-1,-1,0,0,-2 } },//ㄱㄴ

{ { 0,0,-1,0,1,0,-1,-1 },{ 0,-1,0,-2,0,0,-1,0 },{ 0,-1,-1,-1,1,-1,1,0 },{ 0,-1,0,-2,0,0,1,-2 } },//ㄴreverse

{ { 0,0,1,0,-1,0,1,-1 },{ 0,-1,0,0,0,-2,-1,-2 },{ 0,-1,1,-1,-1,-1,-1,0 },{ 0,-1,0,-2,0,0,1,0 } },//ㄴ

{ { 0,0,-1,0,1,0,0,-1 },{ 0,-1,-1, -1,0,-2,0,0 },{ 0,-1,-1,-1,1,-1,0,0 },{ 0,-1,0,-2,0,0,1,-1 } },//ㅗ

};

gameboard::gameboard() {
	playerid++;
	id = playerid;

	for (int x = 0; x < BW + 2; ++x) {

		for (int y = 0; y < BH + 2; ++y) {

			board[x][y].block = (y == 0 || y == BH + 1 || x == 0 || x == BW + 1) ? WALL : EMPTY;
			board[x][y].color = gtxy::Black;
		}

	}
	if (id == 1) { // 1번 플레이어의 정보 설정
		nx = (BX + BW) / 2 + 1;
		ny = 4;
		ax = BW / 2 + 1;
		ay = 4;
		brick = 0;
		rot = 0;
		mtrig = false;
		DrawNextBoard();
	}
	else if (id > 1) { //1번 외 플레이어의 정보 설정

		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ny = 4;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 2) + (BW / 2);
		ay = 4;
		brick = 0;
		rot = 0;
		mtrig = false;
	}
	for (int i = 0; i < 4; ++i) { // 현재 내려오고 있는 블럭 정보 변수 초기화
		p[i] = { 0, 0, 3 };
	}
	score = 0;
	DrawScreen();


}

gameboard::gameboard(gameboard&& rhs) noexcept {

	id = rhs.id;
	nx = rhs.nx;
	ny = rhs.ny;
	ax = rhs.ax;
	ay = rhs.ay;
	
	memcpy(board, rhs.board, sizeof(boardinfo)*((BH + 2)*(BW + 2)));
	brick = rhs.brick;
	rot = rhs.rot;
	mtrig = rhs.mtrig;
	score = rhs.score;
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
	score = rhs.score;
	mtrig = rhs.mtrig;
	return *this;
}

//화면에 테트리스 틀 출력 함수
void gameboard::DrawScreen() {
	for (int x = 0; x < BW + 2; ++x) {
		for (int y = 0; y < BH + 2; ++y) {
			if (board[x][y].block == WALL && id == 1)
			{
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX * (id)+x * 2, BY + y);
				puts(tile[2]);
			}
			else if (board[x][y].block == WALL && id > 1)
			{
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2, BY + y);
				puts(tile[2]);
			}
		}
	}
	gtxy::gotoxy(BX, BY + BH + 5);
	puts("score : ");
}
//내려오는 블럭을 그려주는 함수 한칸 내려올 때 내려오는 블럭과 겹치지 않는 부분만 그려줌
void gameboard::printnextbrick(const short brick, const short rot)
{
	short cnt = 0;
	short cntflag = 0;

	int j = 1;
	short tnx = 0; 
	std::lock_guard<std::mutex> l(m[id - 1]);

	for (int i = 0; i < 4; ++i) {
		if (id == 1) { // 1플레이어
			board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1; // 블럭이 있는 위치의 상태값을 1증가시킴
		}
		else if (id > 1) { // 나머지 플레이어
			board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny +
				shape[brick][rot][i].y].block += 1;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (id == 1) { // 1플레이어
			if (p[i].state == board[p[i].x][p[i].y].block) {	// 현재 내려오는 블럭 정보와 게임보드에 저장된 블럭 정보 비교해서
																// 같으면 해당 좌표에 블럭을 표시하지 않아도 됨

				board[p[i].x][p[i].y].block = EMPTY;
				std::lock_guard<std::mutex> l(printm);
				gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
				puts(tile[4]);
			}
			else {
				p[i].state = board[p[i].x][p[i].y].block;		// 같지 않은 부분, 즉 계속 표시를 유지해야할 위치를 저장
				n[cnt++] = p[i];	// 블럭 4칸의 정보 중 겹치지 않는 부분만 따로 저장함(나중에 비교횟수를 줄이기 위함)
			}
		}
		else if (id > 1) { // 그외 플레이어
			if (p[i].state == board[p[i].x][p[i].y].block) {


				board[p[i].x][p[i].y].block = EMPTY;
				std::lock_guard<std::mutex> l(printm);
				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + ((p[i].x) + static_cast<short>(28 + 22 * (id - 2))-1) * 2, BY + p[i].y);
				puts(tile[4]);

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

	// 한칸 내려와서 표시해야 할 블럭의 위치와 겹치지 않은 부분에 대한 정보 p[]와 비교해서 같지 않은 부분만, 즉 새로 그려야할 부분만 그려준다.
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
			std::lock_guard<std::mutex> l(printm);
			gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
			gtxy::gotoxy(BX + (p[cnt + j - 1].x) * 2, BY + p[cnt + j - 1].y);
			puts(tile[1]);

		}
		else if (id > 1) {
			p[cnt + j - 1] = { nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx - static_cast<short>(28 + 22 * (id - 2)) + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			std::lock_guard<std::mutex> l(printm);
			gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
			gtxy::gotoxy(BX + ((p[cnt + j - 1].x) + static_cast<short>(28 + 22 * (id - 2))-1) * 2, BY + p[cnt + j - 1].y);
			puts(tile[1]);

		}
		j++;
	}
}

//처음 나올 블럭을 그리는 함수 
void gameboard::Printbrick(bool show, short brick, short rot) {


	std::unique_lock<std::mutex> l(printm, std::defer_lock);
	if (show) { // 화면에 그려야 할 때 
		for (int i = 0; i < 4; ++i) {
			std::lock_guard<std::mutex> lg(m[id - 1]);
			if (id == 1) {
				// 보드내 해당 블럭 위치 좌표의 상태값을 1 증가시킨후 현재 블럭 정보 p[]에 저장한다.
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
				gtxy::gotoxy(BX + ((p[i].x) + static_cast<short>(28 + 22 * (id - 2))-1) * 2, BY + p[i].y);
				std::puts(tile[1]);
				l.unlock();
			}


		}
	}
	else { // 화면에서 지울 때
		for (int i = 0; i < 4; ++i) {
			l.lock();
			board[p[i].x][p[i].y].block = EMPTY;
			gtxy::gotoxy(BX + (p[i].x) * 2, BY + p[i].y);
			std::puts(tile[0]);
			l.unlock();
		}
		for (int i = 0; i < 4; ++i) {
			l.lock();
			board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block += 1;
			p[i] = { nx + shape[brick][rot][i].x ,ny + shape[brick][rot][i].y ,board[nx + shape[brick][rot][i].x][ny + shape[brick][rot][i].y].block };
			gtxy::gotoxy(BX + (p[i].x) * 2-1, BY + p[i].y);
			std::puts(tile[1]);
			l.unlock();
		}
	}

}

//내려올 지점을 미리 보여주는 함수
void gameboard::Printbricka(bool nshow, short nbrick, short nrot, bool isup) {


	short cntflag = 0;

	if (nshow) { //화면에 그려야할 때
		for (int i = 0; i < 4; ++i) {
			cntflag = 0;
			for (int j = 0; j < 4; ++j) {// 현재 내려오는 블럭과 내려올 지점을 미리보여주는 블럭이 겹치는 부분은 그려주지 않는다.
				if (((shape[nbrick][nrot][i].x + ax) == (shape[nbrick][nrot][j].x + nx)) && ((shape[nbrick][nrot][i].y + ay) == (shape[nbrick][nrot][j].y + ny)))
				{
					cntflag++;
					break;
				}

			}
			if (cntflag == 0)
			{
				std::lock_guard<std::mutex> l(printm);

				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + (shape[nbrick][nrot][i].x + ax) * 2, BY + shape[nbrick][nrot][i].y + ay);
				puts("□");

			}
		}
	}
	else { // 화면에서 지울 때

		for (int i = 0; i < 4; ++i) {
			cntflag = 0;
			if (isup) {// 위 방향키(회전) 시 예상 블럭 처리
				for (int j = 0; j < 4; ++j) {
					// 한칸 회전시 겹치는 미리 보여주는 블럭이 겹치는 부분은 지우지 않는다.
					if (((shape[nbrick][nrot][i].x + ax) == (shape[nbrick][(nrot + 1) % 4][j].x + nx)) && ((shape[nbrick][nrot][i].y + ay) == (shape[nbrick][(nrot + 1) % 4][j].y + ny)))
					{
						cntflag++;
						break;
					}

				}
			}
			else { // 다른 방향키 입력시
				for (int j = 0; j < 4; ++j) { // 회전시 겹치는 예상 블럭과 겹치는 부분은 지우지 않는다.
					if (((shape[nbrick][nrot][i].x + ax) == (shape[nbrick][nrot][j].x + nx)) && ((shape[nbrick][nrot][i].y + ay) == (shape[nbrick][nrot][j].y + ny)))
					{
						cntflag++;
						break;
					}

				}
			}


			if (cntflag == 0)
			{
				std::lock_guard<std::mutex> l(printm);

				gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
				gtxy::gotoxy(BX + (shape[nbrick][nrot][i].x + ax) * 2, BY + shape[nbrick][nrot][i].y + ay);
				puts(" ");

			}
		}
	}

}

// 보드 안의 블럭을 그려주는 함수
void gameboard::DrawBoard() {

	int x, y;
	std::lock_guard<std::mutex> l(printm);
	
	for (x = 1; x < BW+1 ; x++) {

		for (y = 1; y < BH + 1; y++) {
			if(id==1)
				gtxy::gotoxy(BX + x * 2, BY + y);
			else
				gtxy::gotoxy(((id - 1)*(BW + 2) * 2) + BX + 10 * (id)+x * 2, BY + y);
			
			gtxy::ClearConsoleToColors(board[x][y].color, gtxy::Black);
			puts(tile[board[x][y].block]);

		}

	}
	

}
// 다음 나올 블럭의 정보를 보여줄 틀
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


//다음 나올 블럭을 그려줌
void gameboard::DrawNextBrick(bool show, short brick) {
	std::unique_lock<std::mutex> l(printm, std::defer_lock);

	for (int i = 0; i < 4; ++i) {
		l.lock();
		gtxy::gotoxy((shape[brick][0][i].x + BX + BW + 3) * 2, BY + shape[brick][0][i].y + 4);
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		puts(tile[show ? BRICK : EMPTY]);
		l.unlock();
	}

}

void gameboard::Printscore()
{
	std::lock_guard<std::mutex> l(printm);
	gtxy::gotoxy(BX + 10, BY + BH + 5);
	puts("                      ");
	gtxy::gotoxy(BX + 10, BY + BH + 5);
	gtxy::ClearConsoleToColors(gtxy::White, gtxy::Black);
	printf("%d", score);
}



void gameboard::setxy(const short id) {
	if (id == 0) {
		nx = (BW) / 2 + 1;
		ny = 4;
		ax = (BW) / 2 + 1;
		ay = 4;
	}
	else if (id > 0) {
	
		nx = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ny = 4;
		ax = (BW + BX + 8) + (BW + BX + 2)*(id - 1) + (BW / 2);
		ay = 4;
	}
}
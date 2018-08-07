#include"inputclass.h"
#include "gameboard.h"
#include "gotoxy.h"
#include <conio.h>
#include <iostream>
#include <time.h>

bool trig[4] = { false,false,false,false };
bool ttrig[4]= { false,false,false,false };
short cnt = 0;
std::vector<gameboard> player;
void inputclass::operator()(std::queue<int>& q) {
	gtxy::SetCursor(false);
	while (true) {
		ch = _getch();
		if (ch == 224) {
			ch = _getch();
			if (ch == UP || ch == LEFT || ch == RIGHT || ch == DOWN)

				q.emplace(ch);

		}
		else if (ch == 1 || ch == 2 || ch == 3 || ch == 4 || ch == 13) {
			q.emplace(ch);
		}
		else
		{

			if (ch == SPACE)
			{
				q.emplace(ch);
				continue;
			}
			else if (ch == ESC) {
				q.emplace(ch);
				break;
			}
		}
	}
}

controller::controller() :brick(0), trot(0), rot(0), playernum(0), timetrig(false) {

	//player.reserve(4);

}
controller::controller(controller &&rhs) {
	brick = std::move(rhs.brick);
	rot = std::move(rhs.rot);
	//player = std::move(rhs.player);
	playernum = rhs.playernum;
}


void moveFunc(const short pnum,bool & timetrig) {
	
	short brick, nextbrick, rot;
	nextbrick = rand() % 7; //ttrig[pnum] = false;

	while (true) {

		//printf("%d\n", pnum);
		player[pnum].setxy(pnum);
		ttrig[pnum] = false;
		brick = nextbrick;
		player[pnum].setbrick(brick);
		nextbrick = rand() % 7;
		rot = 0;
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		//while (controller::getAround(pnum,player[pnum].getax(), player[pnum].getay()+1, brick, rot) == EMPTY) {
		//	//player[playernum].ay += 1;
		//	player[pnum].setay(player[pnum].getay() + 1);
		//}
		//player[pnum].Printbricka(true, brick, rot);
		//player[pnum].Printbricka(true, brick, rot);
		//player[pnum].Printbrick(true, brick, rot);
		player[pnum].Printbrick(true, brick, rot);
		//player[0].DrawNextBrick(true, nextbrick);
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		if (controller::getAround(pnum,player[pnum].getnx(), player[pnum].getny(), brick, rot) != EMPTY) break;

		player[pnum].setmtrig(false);
		while (!player[pnum].getmtrig())
		{
			
			if (timetrig) {
				std::unique_lock<std::mutex> l(m[pnum]);
				if (!ttrig[pnum])
				{
					if (controller::moveDown(pnum, brick, player[pnum].getrot())) {
						ttrig[pnum] = true;
						l.unlock();
						break;
					}
				}
				ttrig[pnum] = true;
				l.unlock();
				while (!ttrig[0]&&!ttrig[1]&&!ttrig[2]&&!ttrig[3])
				{
					;
				}ttrig[pnum] = false;
				
				
			}

		}
		//player[0].DrawNextBrick(false, nextbrick);
	}
	trig[cnt++] = true;
}
void controller::operator()(std::queue<int> & q) {
	srand(time(NULL));
	Timer t;
	std::thread timert(t, std::ref(timetrig));
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();
	timert.detach();
	//nextbrick = rand() % 7;
	player[0].Printscore();
	std::thread p1(moveFunc,0,std::ref(timetrig));
	std::thread p2(moveFunc, 1, std::ref(timetrig));
	std::thread p3(moveFunc, 2, std::ref(timetrig));
	std::thread p4(moveFunc, 3, std::ref(timetrig));
	while (cnt<4) {

		while (true) {
			if (!q.empty()) {

				if (ProcessKey(q.front()) == true) {
					q.pop();
					break;
				}
				q.pop();
			}
		}
		//player[0].nx = BW / 2;

		//player[0].ny = 3;
		//player[0].ax = BW / 2;
		//player[0].ay = 3;

		//brick = nextbrick;
		//nextbrick = rand() % 7;
		//rot = 0;
		//gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		//while (getAround(player[playernum].ax, player[playernum].ay + 1, brick, rot) == EMPTY) {
		//	player[playernum].ay += 1;
		//}
		//player[0].Printbricka(true, brick, rot);
		//player[0].Printbrick(true, brick, rot);
		//player[1].Printbrick(true, brick, rot);
		//player[2].Printbrick(true, brick, rot);
		//player[3].Printbrick(true, brick, rot);
		//player[0].DrawNextBrick(true, nextbrick);
		//gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		//if (getAround(player[0].nx, player[0].ny, brick, rot) != EMPTY) break;
		//while (true)
		//{
		//	if (timetrig) {
		//		if (moveDownAll())
		//			break;
		//	}
		//	if (!q.empty()) {

		//		if (ProcessKey(q.front()) == true) {
		//			q.pop();
		//			break;
		//		}
		//		q.pop();
		//	}
		//}
		//player[0].DrawNextBrick(false, nextbrick);
	}
}
short controller::getAround(const short pnum,const short  x, const short  y, const short  b, const short  r) {
	short k = EMPTY;
	for (int i = 0; i< 4; ++i) {
		if(pnum==0)
		k = std::max(k, player[pnum].board[x  + shape[b][r][i].x][y + shape[b][r][i].y].block);
		else if(pnum!=0)
		k = std::max(k, player[pnum].board[x-(28+22*(pnum-1)) + shape[b][r][i].x][y + shape[b][r][i].y].block);
		if (k >0)
		{
			if (x + shape[b][r][i].x < x) {
				return 3;
			}
			else if (x + shape[b][r][i].x > x) {
				return 4;
			}
		}
	}
	return k;
}
void controller::addboard() {
	player.emplace_back();

}
void controller::moveDowna(const short  pnum) {

	while (getAround(pnum,player[pnum].ax, player[pnum].ay + 1, player[pnum].brick, rot) == EMPTY) {
		player[pnum].ay += 1;
	}
}
bool controller::moveDown(const short pnum,const short brick,const short rot) {
	if (getAround(pnum,player[pnum].nx, player[pnum].ny + 1, brick, rot) != EMPTY) {
		TestFull(pnum,brick,rot);
		return true;
	}
	
		player[pnum].Printbrick(false, brick, rot);
		player[pnum].ny += 1;
		player[pnum].Printbrick(true, brick, rot);
	return false;
}
//bool controller::moveDownAll() {
//	for (short i = 0; i < 4; ++i) {
//		moveDown(i);
//	}
//	return false;
//}
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT:
		if (getAround(0,player[0].nx - 1, player[0].ny, player[0].brick, rot) == EMPTY) {
			std::unique_lock<std::mutex> l(m[0]);

			player[0].Printbrick(false, player[0].brick, rot);
			player[0].Printbricka(false, player[0].brick, rot);
			player[0].ay = player[0].ny;
			player[0].nx -= 1;
			player[0].ax -= 1;
			moveDowna(0);
			player[0].Printbricka(true, player[0].brick, rot);
			player[0].Printbrick(true, player[0].brick, rot);
			l.unlock();
		}
		break;
	case RIGHT:
		if (getAround(0,player[0].nx + 1, player[0].ny, player[0].brick, rot) == EMPTY) {
			std::unique_lock<std::mutex> l(m[0]);
			player[0].Printbrick(false, player[0].brick, rot);
			player[0].Printbricka(false, player[0].brick, rot);
			player[0].ay = player[0].ny;
			player[0].nx += 1;
			player[0].ax += 1;
			moveDowna(0);
			player[0].Printbricka(true, player[0].brick, rot);
			player[0].Printbrick(true, player[0].brick, rot);
			l.unlock();

		}

		break;
	case UP:
		trot = (rot == 3 ? 0 : rot + 1);
		rotnum = getAround(0,player[0].nx, player[0].ny, player[0].brick, trot);
		if (rotnum == EMPTY) {
			std::unique_lock<std::mutex> l(m[0]);
			player[0].Printbrick(false, player[0].brick, rot);
			player[0].Printbricka(false, player[0].brick, rot);
			player[0].ay = player[0].ny;
			rot = trot;
			player[0].setrot(rot);
			moveDowna(0);
			player[0].Printbricka(true, player[0].brick, rot);
			player[0].Printbrick(true, player[0].brick, rot);
			l.unlock();

		}
		else if (rotnum == 3) {
			rotnum = getAround(0,player[0].nx + 1, player[0].ny, brick, trot);
			if (rotnum == EMPTY) {
				std::unique_lock<std::mutex> l(m[0]);
				player[0].Printbrick(false, player[0].brick, rot);
				player[0].Printbricka(false, player[0].brick, rot);
				player[0].ay = player[0].ny;
				rot = trot;
				player[0].setrot(rot);
				player[0].nx++;
				player[0].ax++;
				moveDowna(0);
				player[0].Printbricka(true, player[0].brick, rot);
				player[0].Printbrick(true, player[0].brick, rot);
				l.unlock();

			}
		}
		else if (rotnum == 4) {
			rotnum = getAround(0,player[0].nx - 1, player[0].ny, player[0].brick, trot);
			if (rotnum == EMPTY) {
				std::unique_lock<std::mutex> l(m[0]);
				player[0].Printbrick(false, player[0].brick, rot);
				player[0].Printbricka(false, player[0].brick, rot);
				player[0].ay = player[0].ny;
				rot = trot;
				player[0].setrot(rot);
				player[0].nx--;
				player[0].ax--;
				moveDowna(0);
				player[0].Printbricka(true, player[0].brick, rot);
				player[0].Printbrick(true, player[0].brick, rot);
				l.unlock();

			}
		}

		break;
	case DOWN:
		if (moveDown(0,brick,rot))
			return true;

		break;
	case SPACE:
		while (!moveDown(0,player[0].brick,rot)) {}
		return true;
	}
	return false;
}

void controller::TestFull(const short pnum,const short brick,const short rot) {
	using namespace std::chrono_literals;

	int x, y, ty;
	for (int i = 0; i < 4; ++i) {
		if (pnum == 0) {
			player[pnum].board[player[pnum].nx  + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].nx  + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].color = brick + 1;
			
		}
		if (pnum > 0) {
			player[pnum].board[player[pnum].nx - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].nx - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].color = brick + 1;
			
		}

	}
	for (y = 1; y < BH + 1; ++y) {
		for (x = 1; x < BW + 1; ++x) {
			if (player[pnum].board[x][y].block != BRICK)
				break;
		}
		if (x == BW + 1) {
			if (pnum == 0)
				player[pnum].score++;
			for (ty = y; ty > 1; --ty) {
				for (x = 1; x < BW + 1; ++x) {
					player[pnum].board[x][ty] = player[pnum].board[x][ty - 1];
				}
			}
			player[pnum].DrawBoard();
			if (pnum == 0)
				player[pnum].Printscore();
			std::this_thread::sleep_for(50ms);
		}
	}
	player[pnum].mtrig = true;
}

void Timer::operator()(bool &trig) {
	while (true) {
		if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() > 1)
		{
			start = std::chrono::high_resolution_clock::now();
			trig = true;
		}
		else {
			trig = false;
		}
	}
}

void MoveFunc::operator()(gameboard & p) {

	
}
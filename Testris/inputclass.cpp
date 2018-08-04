#include"inputclass.h"
#include "gameboard.h"
#include "gotoxy.h"
#include <conio.h>
#include <iostream>
#include <time.h>

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

	player.reserve(4);

}
controller::controller(controller &&rhs) {
	brick = std::move(rhs.brick);
	rot = std::move(rhs.rot);
	player = std::move(rhs.player);
	playernum = rhs.playernum;
}
void controller::operator()(std::queue<int> & q) {
	srand(time(NULL));
	Timer t;
	std::thread timert(t, std::ref(timetrig));
	timert.detach();
	nextbrick = rand() % 7;
	player[0].Printscore();
	while (true) {


		player[0].nx = BW / 2;

		player[0].ny = 3;
		player[0].ax = BW / 2;
		player[0].ay = 3;

		brick = nextbrick;
		nextbrick = rand() % 7;
		rot = 0;
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		while (getAround(player[playernum].ax, player[playernum].ay + 1, brick, rot) == EMPTY) {
			player[playernum].ay += 1;
		}
		player[0].Printbricka(true, brick, rot);
		player[0].Printbrick(true, brick, rot);
		player[1].Printbrick(true, brick, rot);
		player[2].Printbrick(true, brick, rot);
		player[3].Printbrick(true, brick, rot);
		player[0].DrawNextBrick(true, nextbrick);
		gtxy::ClearConsoleToColors(brick + 1, gtxy::Black);
		if (getAround(player[0].nx, player[0].ny, brick, rot) != EMPTY) break;
		while (true)
		{
			if (timetrig) {
				if (moveDownAll())
					break;
			}
			if (!q.empty()) {

				if (ProcessKey(q.front()) == true) {
					q.pop();
					break;
				}
				q.pop();
			}
		}
		player[0].DrawNextBrick(false, nextbrick);
	}
}
short controller::getAround(const short  x, const short  y, const short  b, const short  r) {
	short k = EMPTY;
	for (int i = 0; i< 4; ++i) {
		k = MAX(k, player[0].board[x + shape[b][r][i].x][y + shape[b][r][i].y].block);
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

	while (getAround(player[pnum].ax, player[pnum].ay + 1, brick, rot) == EMPTY) {
		player[pnum].ay += 1;
	}
}
bool controller::moveDown(const short pnum) {
	if (getAround(player[pnum].nx, player[pnum].ny + 1, brick, rot) != EMPTY) {
		TestFull(pnum);
		return true;
	}
	player[pnum].Printbrick(false, brick, rot);
	player[pnum].ny += 1;
	player[pnum].Printbrick(true, brick, rot);
	return false;
}
bool controller::moveDownAll() {
	for (short i = 0; i < 4; ++i) {
		moveDown(i);
	}
	return false;
}
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT:
		if (getAround(player[0].nx - 1, player[0].ny, brick, rot) == EMPTY) {

			player[0].Printbrick(false, brick, rot);
			player[0].Printbricka(false, brick, rot);
			player[0].ay = player[0].ny;
			player[0].nx -= 1;
			player[0].ax -= 1;
			moveDowna(0);
			player[0].Printbricka(true, brick, rot);
			player[0].Printbrick(true, brick, rot);

		}
		break;
	case RIGHT:
		if (getAround(player[0].nx + 1, player[0].ny, brick, rot) == EMPTY) {
			player[0].Printbrick(false, brick, rot);
			player[0].Printbricka(false, brick, rot);
			player[0].ay = player[0].ny;
			player[0].nx += 1;
			player[0].ax += 1;
			moveDowna(0);
			player[0].Printbricka(true, brick, rot);
			player[0].Printbrick(true, brick, rot);


		}

		break;
	case UP:
		trot = (rot == 3 ? 0 : rot + 1);
		rotnum = getAround(player[0].nx, player[0].ny, brick, trot);
		if (rotnum == EMPTY) {
			player[0].Printbrick(false, brick, rot);
			player[0].Printbricka(false, brick, rot);
			player[0].ay = player[0].ny;
			rot = trot;
			moveDowna(0);
			player[0].Printbricka(true, brick, rot);
			player[0].Printbrick(true, brick, rot);


		}
		else if (rotnum == 3) {
			rotnum = getAround(player[0].nx + 1, player[0].ny, brick, trot);
			if (rotnum == EMPTY) {
				player[0].Printbrick(false, brick, rot);
				player[0].Printbricka(false, brick, rot);
				player[0].ay = player[0].ny;
				rot = trot;
				player[0].nx++;
				player[0].ax++;
				moveDowna(0);
				player[0].Printbricka(true, brick, rot);
				player[0].Printbrick(true, brick, rot);

			}
		}
		else if (rotnum == 4) {
			rotnum = getAround(player[0].nx - 1, player[0].ny, brick, trot);
			if (rotnum == EMPTY) {
				player[0].Printbrick(false, brick, rot);
				player[0].Printbricka(false, brick, rot);
				player[0].ay = player[0].ny;
				rot = trot;
				player[0].nx--;
				player[0].ax--;
				moveDowna(0);
				player[0].Printbricka(true, brick, rot);
				player[0].Printbrick(true, brick, rot);

			}
		}

		break;
	case DOWN:
		if (moveDown(0))
			return true;

		break;
	case SPACE:
		while (!moveDown(0)) {}
		return true;
	}
	return false;
}

void controller::TestFull(const short pnum) {
	using namespace std::chrono_literals;

	int x, y, ty;
	for (int i = 0; i < 4; ++i) {
		player[pnum].board[player[pnum].nx + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].block = BRICK;
		player[pnum].board[player[pnum].nx + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].color = brick + 1;


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
			std::this_thread::sleep_for(100ms);
		}
	}
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
#include"inputclass.h"
#include "gameboard.h"
#include "gotoxy.h"
#include <conio.h>
#include <iostream>
#include <time.h>
static bool movecounter = false;
bool trig[4] = { false,false,false,false };
bool ttrig[4]= { false,false,false,false };
short cnt = 0;
bool inputclass::startflag = false;
std::vector<gameboard> player;
bool inputclass::operator()(std::queue<int>& q) {
	gtxy::SetCursor(false);
	while (!startflag) {
		ch = _getch();
		if (ch == SPACE)
			return true;
		else if (ch == ESC)
			return false;
	}
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
	using namespace std::chrono_literals;
	short brick, nextbrick, rot;
	player[pnum].setmtrig(true);
	nextbrick = rand() % 7; 
	std::unique_lock<std::mutex> l(m[pnum], std::defer_lock);
	while (true) {

		
		player[pnum].setxy(pnum);
		
		
		player[pnum].setbrick(nextbrick);
		player[pnum].setrot(0);
		
		nextbrick = rand() % 7;
		if (pnum == 0)
			player[pnum].DrawNextBrick(true,nextbrick);
		rot = 0;

	
		if (controller::getAround(pnum,player[pnum].getnx(), player[pnum].getny(), player[pnum].getbrick(), rot) != EMPTY) break;
		
		player[pnum].Printbrick(true, player[pnum].getbrick(), 0);
		ttrig[pnum] = false;
		player[pnum].setmtrig(true);
		if (pnum == 0) {
			controller::moveDowna(pnum);
			player[pnum].Printbricka(true, player[pnum].getbrick(), player[pnum].getrot());
		}
		while (!ttrig[pnum])
		{
			

		}
		if (pnum == 0)
		{
		
			player[pnum].DrawNextBrick(false, nextbrick);
			
		}
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
	

	player[0].Printscore();
	std::thread p1(moveFunc,0,std::ref(timetrig));
	std::thread p2(moveFunc, 1, std::ref(timetrig));
	std::thread p3(moveFunc, 2, std::ref(timetrig));
	std::thread p4(moveFunc, 3, std::ref(timetrig));
	std::unique_lock<std::mutex> l(m[0], std::defer_lock);

	while (cnt<4) {
		//while(!player[0].getmtrig()) {

		//}
		//controller::moveDowna(playernum);
		//player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot());
		while (true) {

			if (!q.empty()) {

				if (ProcessKey(q.front()) == true) {
					q.pop();
					break;
				}
				q.pop();
			}
			if (movecounter) {
				moveDownAll();
				
			}
		}

	}
}
short controller::getAround(const short pnum,const short  x, const short  y, const short  b, const short  r) {
	short k =100;
	
	for (int i = 0; i< 4; ++i) {
		if(pnum==0)
		k = std::min(k, player[pnum].board[x  + shape[b][r][i].x][y + shape[b][r][i].y].block);
		else if(pnum!=0)
		k = std::min(k, player[pnum].board[x-(28+22*(pnum-1)) + shape[b][r][i].x][y + shape[b][r][i].y].block);
		if (k <3)
		{
			if (x + shape[b][r][i].x < x) {
				return -1;
			}
			else if (x + shape[b][r][i].x > x) {
				return -2;
			}
		}
	}
	return k;
}
void controller::addboard() {
	player.emplace_back();

}
void controller::moveDowna(const short  pnum) {

	while (getAround(pnum,player[pnum].ax, player[pnum].ay + 1, player[pnum].brick, player[pnum].rot) >= EMPTY) {
		player[pnum].ay += 1;
	}
}
bool controller::moveDown(const short pnum,const short brick,const short rot,const bool spacebar) {
	std::unique_lock<std::mutex> l(m[pnum],std::defer_lock);
	
	l.lock();
	if (getAround(pnum,player[pnum].nx, player[pnum].ny + 1, brick, rot) <EMPTY) {
		if (spacebar)
		{
			player[pnum].printnextbrick(brick, rot);
			
		}
		if(player[pnum].getmtrig())	
			TestFull(pnum,brick,rot);
		if (!spacebar)
			player[pnum].setmtrig(true);
		l.unlock();
		return true;	
	}

	player[pnum].ny += 1;
	l.unlock();
	if(!spacebar)
		player[pnum].printnextbrick(brick, rot);
	return false;
}
bool controller::moveDownAll() {
	for (short i = 0; i < 4; ++i) {
		if (moveDown(i, player[i].getbrick(), player[i].getrot(), false))
		{
			ttrig[i] = true;
		}
	
	}
	
	return false;
}
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT:
		if (getAround(0,player[0].nx - 1, player[0].ny, player[0].brick, player[0].rot) == EMPTY) {
			//std::unique_lock<std::mutex> l(m[0]);
			
			std::unique_lock<std::mutex> l(m[0]);
			player[0].Printbricka(false, player[0].brick, player[0].rot);
			player[0].ay = player[0].ny;
			
			player[0].nx -= 1;
			player[0].ax -= 1;
			moveDowna(0);
			
			
			player[0].printnextbrick(player[0].brick, player[0].rot);
			player[0].Printbricka(true, player[0].brick, player[0].rot);
			l.unlock();
		}
		break;
	case RIGHT:
		if (getAround(0,player[0].nx + 1, player[0].ny, player[0].brick, player[0].rot) == EMPTY) {
			std::unique_lock<std::mutex> l(m[0]);
			player[0].Printbricka(false, player[0].brick, player[0].rot);
			player[0].ay = player[0].ny;
			player[0].nx += 1;
			player[0].ax += 1;
			moveDowna(0);
			
			//player[0].Printbricka(true, player[0].brick, player[0].rot);
			player[0].printnextbrick(player[0].brick, player[0].rot);
			player[0].Printbricka(true, player[0].brick, player[0].rot);
			l.unlock();

		}

		break;
	case UP:
		trot = (player[0].rot == 3 ? 0 : player[0].rot + 1);
		rotnum = getAround(0,player[0].nx, player[0].ny, player[0].brick, trot);
		if (rotnum == EMPTY) {
		std::unique_lock<std::mutex> l(m[0]);
			
		
		player[0].Printbricka(false, player[0].brick, player[0].rot);
		player[0].ay = player[0].ny;
	
		
		player[0].setrot(trot);
		
		moveDowna(0);
	
		//player[0].Printbricka(true, player[0].brick, player[0].rot);
		player[0].printnextbrick(player[0].brick, player[0].rot);
		
		player[0].Printbricka(true, player[0].brick, player[0].rot);

		l.unlock();
		}

		
		else if (rotnum == -1) {
			rotnum = getAround(0, player[0].nx + 1, player[0].ny, brick, trot);
			if (rotnum == EMPTY) {


				//player[0].Printbricka(false, player[0].brick, rot);
				//player[0].ay = player[0].ny;

				//rot = trot;
				std::unique_lock<std::mutex> l(m[0]);
				if (player[0].ay - player[0].ny>3)
				player[0].Printbricka(false, player[0].brick, player[0].rot);
				player[0].ay = player[0].ny;
				player[0].setrot(trot);
				player[0].nx++;
				player[0].ax++;
				
				//
				player[0].printnextbrick(player[0].brick, player[0].rot);
				moveDowna(0);
				if (player[0].ay - player[0].ny>3)
				player[0].Printbricka(true, player[0].brick, rot);
				l.unlock();

			}
		}
	
		else if (rotnum == -2) {
			if (player[0].brick == 0 && (trot == 0 || trot == 2)) {
				rotnum=getAround(0, player[0].nx - 2, player[0].ny, player[0].brick, trot);
			}
			else {
				rotnum = getAround(0, player[0].nx - 1, player[0].ny, player[0].brick, trot);
			}if (rotnum == EMPTY) {
				
	
				
				std::unique_lock<std::mutex> l(m[0]);
				if (player[0].ay - player[0].ny>3)
				player[0].Printbricka(false, player[0].brick, player[0].rot);
				player[0].ay = player[0].ny;
				player[0].setrot(trot);
				if (player[0].brick == 0 && (trot == 0 || trot == 2)) {
					player[0].nx -= 2;
					player[0].ax -= 2;
				}
				else
				{
					player[0].nx--;
					player[0].ax--;
				}
			
				player[0].printnextbrick(player[0].brick, player[0].rot);
				moveDowna(0);
				if (player[0].ay - player[0].ny>3)
				player[0].Printbricka(true, player[0].brick,player[0].rot);
	
		
		
				l.unlock();

			}
		}

		break;
	case DOWN:
		if (moveDown(0,player[0].brick,player[0].rot,false))
			return true;

		break;
	case SPACE:
		while (!moveDown(0,player[0].brick,player[0].rot,true)) {}
		ttrig[0] = true;

		return true;
	}
	return false;
}

void controller::TestFull(const short pnum,const short brick,const short rot) {
	using namespace std::chrono_literals;
	//std::unique_lock<std::mutex> l(m[pnum], std::defer_lock);
	int x, y, ty;
	player[pnum].setmtrig(false);
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
			{
				
				break;
			}
		}
		
		if (x == BW + 1) {
			//if (pnum == 0)
			//	player[pnum].score++;
			//for (int i = 0; i < 4; ++i) {
			//	player[pnum].board[player[pnum].nx + shape[brick][rot][i].x][player[pnum].ny + shape[brick][rot][i].y].block = EMPTY;
			//}
			for (ty = y; ty > 1; --ty) {
				for (x = 1; x < BW + 1; ++x) {
					//l.lock();
					player[pnum].board[x][ty] = player[pnum].board[x][ty - 1];
					//l.unlock();
				}
			}
			player[pnum].DrawBoard();
			if (pnum == 0)
				player[pnum].Printscore();
			std::this_thread::sleep_for(50ms);
		}
	}
	//player[pnum].setxy(pnum);
	
}

void Timer::operator()(bool &trig) {
	while (true) {
		if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() > 2)
		{
			start = std::chrono::high_resolution_clock::now();
			trig = true;
			movecounter = true;
		}
		else {
			trig = false;
			movecounter = false;
		}
	}
}


void MoveFunc::operator()(gameboard & p) {

	
}

//void Movecontroller::operator()() {
//	while (true)
//	{
//		if (movecounter) {
//			controller::moveDownAll();
//			movecounter = false;
//		}
//	}
//}
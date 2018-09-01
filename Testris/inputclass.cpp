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
const short PLAYERNUM = 4;
std::mutex queuem;
bool inputclass::startflag = false;
std::vector<gameboard> player;
bool inputclass::operator()(std::queue<int>& q) {
	using namespace std::chrono_literals;
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
			if (ch == UP || ch == LEFT || ch == RIGHT || ch == DOWN) {
				std::lock_guard<std::mutex> ql(queuem);
				q.emplace(ch);
			}
		}
		else if (ch == 1 || ch == 2 || ch == 3 || ch == 4 || ch == 13) {
			std::lock_guard<std::mutex> ql(queuem);
			q.emplace(ch);
		}
		else
		{

			if (ch == SPACE)
			{
				std::lock_guard<std::mutex> ql(queuem);
				q.emplace(ch);
				if (cnt == 4)
					return true;
			}
			else if (ch == ESC) {
				if (cnt == 4)
					return false;
				std::lock_guard<std::mutex> ql(queuem);
				q.emplace(ch);
			}
		}
		std::this_thread::sleep_for(1ms);
	}
}

controller::controller() :brick(0), trot(0), rot(0), playernum(0), timetrig(false) {

	ttrig[0] = false;

}

controller::controller(controller &&rhs) {
	brick = std::move(rhs.brick);
	rot = std::move(rhs.rot);
	//player = std::move(rhs.player);
	playernum = rhs.playernum;
}

void moveFunc(const short pnum) {
	using namespace std::chrono_literals;
	short brick, nextbrick, rot;
	player[pnum].setmtrig(true);
	nextbrick = rand() % 7; 
	//std::unique_lock<std::mutex> l(m[pnum], std::defer_lock);
	while (true) {

		
		player[pnum].setxy(pnum);
		player[pnum].setbrick(nextbrick);
		player[pnum].setrot(0);
		
		nextbrick = rand() % 7;
		if (pnum == 0)
		{

			player[pnum].DrawNextBrick(true, nextbrick);
		
		}
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
			
			std::this_thread::sleep_for(1ms);
		}
		if (pnum == 0)
		{
		
			player[pnum].DrawNextBrick(false, nextbrick);
			
		}
		std::this_thread::sleep_for(10ns);
	}
	trig[pnum] = true;
	cnt++;
}

void controller::operator()(std::queue<int> & q) {
	srand(time(NULL));
	using namespace std::chrono_literals;
	cnt = 0;
	Timer t;
	std::thread timert(t, std::ref(timetrig));
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();
	
	
	for (int p = 0; p < player.size(); ++p) {
		trig[p] = false;
	}
	player[playernum].Printscore();
	std::thread p1(moveFunc,0);
	std::thread p2(moveFunc, 1);
	std::thread p3(moveFunc, 2);
	std::thread p4(moveFunc, 3);
	thread_guard tg1(p1);
	thread_guard tg2(p2);
	thread_guard tg3(p3);
	thread_guard tg4(p4);


	Movecontroller mc;
	std::thread mcthread(mc);
	
	while (cnt<PLAYERNUM) {
		while (true) {

				if (!q.empty()) {

					if (ProcessKey(q.front()) == true) {
						{std::lock_guard<std::mutex> ql(queuem);
							for (size_t i = 0; i < q.size(); ++i) {
								q.pop();
							}}
						std::this_thread::sleep_for(30ms);
						break;
					}
					std::lock_guard<std::mutex> ql(queuem);
					for (size_t i = 0; i < q.size(); ++i) {
						q.pop();
					}
				}

				
				std::this_thread::sleep_for(30ms);

		}
		std::this_thread::sleep_for(10ms);
	}
	
	gameboard::playerid = 0;
	player.clear();
	mcthread.join();
	timert.join();
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
	
	
	if (getAround(pnum,player[pnum].getnx(), player[pnum].getny() + 1, brick, rot) <EMPTY) {
		if (spacebar)
		{
			player[pnum].printnextbrick(brick, rot);
		}
		if(player[pnum].getmtrig())	
			TestFull(pnum,brick,rot);
		if (!spacebar)
			player[pnum].setmtrig(true);
		
		return true;	
	}
	l.lock();
	player[pnum].setny(player[pnum].getny()+1);
	l.unlock();
	if (!spacebar)
	{
		player[pnum].printnextbrick(brick, rot);
		//if (pnum == 0 && (player[0].getay()-player[0].getny())==5) {
		//	player[pnum].Printbricka(false, player[pnum].getbrick(), player[pnum].getrot());
		//}
		
	}
	
	return false;
}
bool controller::moveDownAll() {
	for (short i = 0; i < PLAYERNUM; ++i) {
		if (!trig[i])
		{
			if (moveDown(i, player[i].getbrick(), player[i].getrot(), false))
			{
				ttrig[i] = true;
			}
		}
	
	}
	
	return false;
}
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT:
		if (getAround(playernum,player[playernum].getnx() - 1, player[playernum].getny(), player[playernum].getbrick(), player[playernum].getrot()) == EMPTY) {
			
			
			player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot());
			std::unique_lock<std::mutex> l(m[playernum]);
			player[playernum].setay( player[playernum].getny());
			
			player[playernum].setnx(player[playernum].getnx()-1);
			player[playernum].setax(player[playernum].getax() - 1);
			moveDowna(playernum);
			l.unlock();
			
			player[playernum].printnextbrick(player[playernum].brick, player[playernum].rot);
			player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot());
		
			
		}
		break;
	case RIGHT:
		if (getAround(playernum,player[playernum].getnx() + 1, player[playernum].getny(), player[playernum].getbrick(), player[playernum].getrot()) == EMPTY) {
			player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot());
			std::unique_lock<std::mutex> l(m[playernum]);
			player[playernum].setay(player[playernum].getny()); 
			player[playernum].setnx(player[playernum].getnx() + 1);
			player[playernum].setax(player[playernum].getax() +1);
			moveDowna(playernum);
			l.unlock();
			player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
			player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot());
	
		}

		break;
	case UP:
		trot = (player[playernum].getrot() == 3 ? 0 : player[playernum].getrot()+1);
		rotnum = getAround(playernum,player[playernum].getnx(), player[playernum].getny(), player[playernum].getbrick(), trot);
		if (rotnum == EMPTY) {
		
			
		//if (player[playernum].getay() - player[playernum].getny()>3)
		player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot());
		std::unique_lock<std::mutex> l(m[playernum]);
		player[playernum].setay( player[playernum].getny());
		
		player[playernum].setrot(trot);
		moveDowna(playernum);
	
		l.unlock();
		player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot());
		player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
		
			


		
		}

		
		else if (rotnum == -1) {
			rotnum = getAround(playernum, player[playernum].getnx() + 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			if (rotnum == EMPTY) {


				//std::unique_lock<std::mutex> l(m[0]);
				//if (player[playernum].getay() - player[playernum].getny()>3)
				player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot());
				std::unique_lock<std::mutex> l(m[0]);
				player[playernum].setay(player[playernum].getny());
				player[playernum].setrot(trot);
				player[playernum].setnx(player[playernum].getnx() + 1);
				player[playernum].setax(player[playernum].getax() + 1);
				moveDowna(playernum);
				l.unlock();
				player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
				
				//if (player[playernum].getay() - player[playernum].getny()>4)
				player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot());
				//l.unlock();

			}
		}
	
		else if (rotnum == -2) {
			if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) {
				rotnum=getAround(playernum, player[playernum].getnx() - 2, player[playernum].getny(), player[playernum].getbrick(), trot);
			}
			else {
				rotnum = getAround(playernum, player[playernum].getnx() - 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			}if (rotnum == EMPTY) {
				
				
				//if (player[playernum].getay() - player[playernum].getny()>3)
				player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot());
				std::unique_lock<std::mutex> l(m[playernum]);
				player[playernum].setay(player[playernum].getny());
				player[playernum].setrot(trot);
				if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) {
					player[playernum].setnx(player[playernum].getnx() - 2);
					player[playernum].setax(player[playernum].getax() -2);
				}
				else
				{
					player[playernum].setnx(player[playernum].getnx() - 1);
					player[playernum].setax(player[playernum].getax() - 1);
				}
				moveDowna(playernum);
				l.unlock();
				player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
				
				//if (player[playernum].getay() - player[playernum].getny()>4)
				player[playernum].Printbricka(true, player[playernum].getbrick(),player[playernum].getrot());
	
		
		
			
			}
		}

		break;
	case DOWN:
		if (moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), false))
		{	
			ttrig[playernum] = true;
			return true;
		}
		break;
	case SPACE:
		if (!trig[playernum]) {
			while (!moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), true)) {}
			ttrig[playernum] = true;
		}
		return true;
	case 2:
		playernum = 1;
		break;

	}
	return false;
}

void controller::TestFull(const short pnum,const short brick,const short rot) {
	using namespace std::chrono_literals;
	int x, y, ty;
	player[pnum].setmtrig(false);
	for (int i = 0; i < 4; ++i) {
		if (pnum == 0) {
			std::lock_guard<std::mutex> l(m[pnum]);
			player[pnum].board[player[pnum].getnx()  + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx()  + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
		}
		if (pnum > 0) {
			std::lock_guard<std::mutex> l(m[pnum]);
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
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
			std::lock_guard<std::mutex> l(m[pnum]);
			for (ty = y; ty > 1; --ty) {
				
				for (x = 1; x < BW + 1; ++x) {
					
					player[pnum].board[x][ty] = player[pnum].board[x][ty - 1];
					
					
				}
			}
			player[pnum].plusscore();
			player[pnum].DrawBoard();
			if (pnum == 0)
				player[pnum].Printscore();
			std::this_thread::sleep_for(50ms);
		}
	}
	
}

void Timer::operator()(bool &trig) {
	using namespace std::chrono_literals;
	while (cnt<PLAYERNUM) {
		if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() > 0.5)
		{
			start = std::chrono::high_resolution_clock::now();
			movecounter = true;
			std::this_thread::sleep_for(1ms);

		}
		else {
			
			movecounter = false;
		}
		std::this_thread::sleep_for(1ms);
	}
}




void Movecontroller::operator()() {
	using namespace std::chrono_literals;
	while (cnt<PLAYERNUM)
	{
		if (movecounter) {
			controller::moveDownAll();
			movecounter = false;
		}
		std::this_thread::sleep_for(1ms);
	}
}
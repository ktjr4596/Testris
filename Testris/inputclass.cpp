
//2018.10.4
//
//��Ʈ����
//
//inputclass.cpp
//�Է� Ŭ���� , ��Ʈ�ѷ�, Ÿ�̸� Ŭ���� ����
//-------------------------------------------------------------------

#include"inputclass.h"
#include "gameboard.h"
#include "gotoxy.h"
#include <conio.h>
#include <iostream>
#include <time.h>

static bool movecounter = false; // ���� �ð��� �Ǹ� true�� �Ǽ� ���� ���� ��ĭ �������� �ϴ� ����
bool trig[4] = { false,false,false,false };	// ���ӿ����� �ƴ��� Ȯ���ϴ� �Ҹ��� ����
bool ttrig[4] = { false,false,false,false };	// ���� �ٴڿ� �������� �� ���� ���� �������ϴ� Ʈ����
short cnt = 0;	// ���ӿ��� �� �÷��̾ ī��Ʈ �ϴ� ����
const short PLAYERNUM = 4;	// �÷��̾� ���� ����
std::mutex queuem; // ť�� ���� ������� �� ���ؽ�
bool inputclass::startflag = false;
std::vector<gameboard> player; // �÷��̾��� ���Ӻ��带 ������ ����

// Ű�Է��� �޴� �Լ� ��ü
bool inputclass::operator()(std::queue<int>& q) {
	using namespace std::chrono_literals;
	gtxy::SetCursor(false);
	while (!startflag) { // ���� ���� ȭ�鿡�� space�� �Է��ϸ� ����
		ch = _getch();
		if (ch == SPACE)
			return true;
		else if (ch == ESC)
			return false;
	}
	while (true) {	// Ű �Է��� ť�� ����
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
	playernum = rhs.playernum;
}


// �÷��̾�� ���Ӻ��� ó���� ������� ����ó���ϱ� ���� �Լ�
void moveFunc(const short pnum) {
	srand(time(NULL));
	using namespace std::chrono_literals;
	short  nextbrick, rot;
	player[pnum].setmtrig(true);
	nextbrick = rand() % 7;
	while (true) {


		player[pnum].setxy(pnum);
		player[pnum].setbrick(nextbrick);
		player[pnum].setrot(0);

		nextbrick = rand() % 7;
		if (pnum == 0) // 1�÷��̾�(�ڽ�)�� ���� ���� �̸� ������
		{

			player[pnum].DrawNextBrick(true, nextbrick);

		}
		rot = 0;

		// ���� ������ ��ġ�� ������� ������ break
		if (controller::getAround(pnum, player[pnum].getnx(), player[pnum].getny(), player[pnum].getbrick(), rot) != EMPTY) break;

		player[pnum].Printbrick(true, player[pnum].getbrick(), 0);
		ttrig[pnum] = false;
		player[pnum].setmtrig(true);
		if (pnum == 0) {
			controller::moveDowna(pnum); // 1�÷��̾�(�ڽ�)�� ���� ���� ���

			player[pnum].Printbricka(true, player[pnum].getbrick(), player[pnum].getrot(), false);
		}
		while (!ttrig[pnum])	// �������� ���� �ٴڿ� ���������� ���
		{

			std::this_thread::sleep_for(1ms);
		}
		if (pnum == 0)
			player[pnum].DrawNextBrick(false, nextbrick);

		std::this_thread::sleep_for(10ns);
	}
	trig[pnum] = true; // ������ ������(���̻� ���� ���� �� ���� ��) ���� ����
	controller::gameOver(pnum);
	cnt++;
}

//��Ʈ�ѷ� �Լ���ü
void controller::operator()(std::queue<int> & q) {
	
	using namespace std::chrono_literals;
	cnt = 0;
	Timer t;
	std::thread timert(t, std::ref(timetrig)); // ���� ������ �����ð��� ī��Ʈ�ϴ� ������ ����
	player.emplace_back();	// �÷��̾ �߰�
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();


	for (int p = 0; p < player.size(); ++p) {
		trig[p] = false;
	}
	player[playernum].Printscore();
	std::thread p1(moveFunc, 0); // �� �÷��̾��� ���带 ó���ϴ� ������
	std::thread p2(moveFunc, 1);
	std::thread p3(moveFunc, 2);
	std::thread p4(moveFunc, 3);
	thread_guard tg1(p1);		
	thread_guard tg2(p2);
	thread_guard tg3(p3);
	thread_guard tg4(p4);


	Movecontroller mc;	
	std::thread mcthread(mc); // �����ð��̵Ǹ� ���� ���� ��ĭ�� ������ ������

	// �÷��̾ ����ִ� ���� ť���� �Է��� ���� ó��
	while (cnt < PLAYERNUM) {
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

// �������� ���� ��� ���� �ִ��� Ȯ���ϴ� �Լ�
short controller::getAround(const short pnum, const short  x, const short  y, const short  b, const short  r) {
	short k = 100;

	for (int i = 0; i < 4; ++i) {
		if (pnum == 0)
			k = std::min(k, player[pnum].board[x + shape[b][r][i].x][y + shape[b][r][i].y].block); // ���� �ֺ������� ���ؼ� ���� ���� ����(1=������ ��, 2= ��, 3= ��ĭ)
		else if (pnum > 0)
			k = std::min(k, player[pnum].board[x - (28 + 22 * (pnum - 1)) + shape[b][r][i].x][y + shape[b][r][i].y].block);
		if (k < 3) // �ֺ��� ���̳� ������ ���� ���� �� ������ ȸ���� ��츦 Ȯ��
		{
			if (x + shape[b][r][i].x < x) {  //���� ������ ȸ���� -1
				return -1;
			}
			else if (x + shape[b][r][i].x > x) { // ������ ������ ȸ���� -2
				return -2;
			}
		}
	}
	return k;
}

// ���� ��(������ ����) �� ��ġ�� �ٲٴ� �Լ�
void controller::moveDowna(const short  pnum) {

	while (getAround(pnum, player[pnum].ax, player[pnum].ay + 1, player[pnum].brick, player[pnum].rot) >= EMPTY) {
		player[pnum].ay += 1;

	}
}

//���� ��ĭ�� ������ �Լ�
bool controller::moveDown(const short pnum, const short brick, const short rot, const bool spacebar) {
	std::unique_lock<std::mutex> l(m[pnum], std::defer_lock);

	l.lock();
	// �ֺ��� ���̳� ���� ���� ��
	if (getAround(pnum, player[pnum].getnx(), player[pnum].getny() + 1, brick, rot) < EMPTY) {
		l.unlock();
		if (spacebar) // �����̽���(�ٷγ���)�� ������ ��
		{
			player[pnum].printnextbrick(brick, rot); // �ٷ� ���� ����
		}
		if (player[pnum].getmtrig())	// ���̻� ������ �� ���� �� ������ �� �� �ִ� �� Ȯ��
			TestFull(pnum, brick, rot);
		if (!spacebar) // �Ʒ� ����Ű�� �����ų� �ڵ����� ������ ��
			player[pnum].setmtrig(true);

		return true;
	}

	//y ��ǥ�� ������Ű�� ���� �׸�
	player[pnum].setny(player[pnum].getny() + 1); 
	l.unlock();
	if (!spacebar)
	{
		player[pnum].printnextbrick(brick, rot);
	
	}

	return false;
}

// ���� �ð��� ������ �� ��� �÷��̾��� ���� ��ĭ�� ������ �Լ�
bool controller::moveDownAll() {
	for (short i = 0; i < PLAYERNUM; ++i) {
		if (!trig[i])	// ����ִ� �÷��̾ ����
		{
			if (moveDown(i, player[i].getbrick(), player[i].getrot(), false))
			{
				ttrig[i] = true; // ���� �� �������� ���� �� ���� Ʈ���Ÿ� true�� �ٲ�
			}
		}

	}

	return false;
}

// ť���� �� Ű�� ó���ϴ� �Լ�
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT: // ����Ű �Է½� �������� ���� �̵������ϸ� �������� �̵��� ���
		moveDirection(LEFT);
		break;
	case RIGHT: // ������
		moveDirection(RIGHT);
		break;
	case UP: //ȸ�� 
		trot = (player[playernum].getrot() == 3 ? 0 : player[playernum].getrot() + 1);
		rotnum = getAround(playernum, player[playernum].getnx(), player[playernum].getny(), player[playernum].getbrick(), trot);
		if (rotnum == EMPTY) { // �Ϲ����� ȸ����
			moveDirection(UP);
		}

		// ���� ������ ȸ���ϸ� ���� ��ĭ ���������� �ű� �� ȸ��
		else if (rotnum == -1) {
			rotnum = getAround(playernum, player[playernum].getnx() + 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			if (rotnum == EMPTY) {
				moveDirection(-1);
			}
		}
		// ������ ������ ȸ���ϸ� ���� �������� �ű� �� ȸ��
		else if (rotnum == -2) {
			if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) { // ���� ���� ���� ������ �� ȸ�� ó��
				rotnum = getAround(playernum, player[playernum].getnx() - 2, player[playernum].getny(), player[playernum].getbrick(), trot);
			}
			else { // ������ ���� ���� ������ �� ȸ�� ó��
				rotnum = getAround(playernum, player[playernum].getnx() - 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			}if (rotnum == EMPTY) {

				std::unique_lock<std::mutex> l(m[playernum]);
				player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot(), true);

				if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) {// ���� ���� ���� ������ �� ȸ�� ó��
					player[playernum].setnx(player[playernum].getnx() - 2);
					player[playernum].setax(player[playernum].getax() - 2);
				}
				else// ������ ���� ���� ������ �� ȸ�� ó��
				{
					player[playernum].setnx(player[playernum].getnx() - 1);
					player[playernum].setax(player[playernum].getax() - 1);

				}
				player[playernum].setay(player[playernum].getny());
				player[playernum].setrot(trot);
				moveDowna(playernum);

				l.unlock();
				player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
				player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot(), true);

			}
		}

		break;
	case DOWN: // �Ʒ�����Ű ó��
		if (moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), false)) // ���� �� �������� Ʈ���Ÿ� true�� �ٲ�
		{
			ttrig[playernum] = true; 
			return true;
		}
		break;
	case SPACE: // �����̽��� ó��
		if (!trig[playernum]) { // ���ӿ����� �ȵ��� ��츸 ����
			while (!moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), true)) {}
			ttrig[playernum] = true;
		}
		return true;


	}
	return false;
}
//���� �¿�� ȸ�� ó���� �ϴ� �Լ�
void controller::moveDirection(const int key) {
	int value=0;
	if (key == LEFT)// kEY�� �����ϰ�� 1ĭ �������� �̵�
		value = 1;
	else if (key == RIGHT||key==-1)  //KEY�� ������ Ȥ�� -1(���ʺ����� ȸ����) ���������� ��ĭ �̵�
		value = -1;

	if (key == UP) { // �Ϲ����� ȸ����
	player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot(), true);
	std::unique_lock<std::mutex> l(m[playernum]);
	player[playernum].setay(player[playernum].getny());

	player[playernum].setrot(trot);
	moveDowna(playernum);
	player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot(), true);
	l.unlock();

	player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
	}
	else if (getAround(playernum, player[playernum].getnx() - value, player[playernum].getny(), player[playernum].getbrick(), player[playernum].getrot()) == EMPTY) {

		player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot(), false);
		std::unique_lock<std::mutex> l(m[playernum]);
		player[playernum].setay(player[playernum].getny());
		
		player[playernum].setnx(player[playernum].getnx() - value);
		player[playernum].setax(player[playernum].getax() - value);
		
		if (key == -1)
			player[playernum].setrot(trot);
		moveDowna(playernum);
		player[playernum].Printbricka(true, player[playernum].getbrick(), player[playernum].getrot(), false);
		l.unlock();

		player[playernum].printnextbrick(player[playernum].getbrick(), player[playernum].getrot());
	}	

}
// ������ ���� �ִ��� Ȯ�� �ϰ� ó���ϴ� �Լ�
void controller::TestFull(const short pnum, const short brick, const short rot) {
	using namespace std::chrono_literals;
	int x, y, ty;
	player[pnum].setmtrig(false);
	std::lock_guard<std::mutex> l(m[pnum]);
	for (int i = 0; i < 4; ++i) { // ���峻�� ������ �� ��ġ ���°��� ������ ��(BRICK,1)�� ����
		if (pnum == 0) {

			player[pnum].board[player[pnum].getnx() + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx() + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
		}
		if (pnum > 0) {
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
		}
	}
	short FullCount{ 0 }; // ������ ��-1 �� ������ ����
	for (y = 1; y < BH + 1; ++y) { 
		for (x = 1; x < BW + 1; ++x) {
			if (player[pnum].board[x][y].block != BRICK) // ���پ� Ȯ���ϸ� ��ĭ�� ������ break
			{

				break;
			}
		}
		
		if (x == BW + 1) { // ���ٿ� ��ĭ�� ���� ��
			FullCount++;
			// ���峻�� �������� ���پ� ���� ����
			for (ty = y; ty > 1; --ty) {

				for (x = 1; x < BW + 1; ++x) {

					player[pnum].board[x][ty] = player[pnum].board[x][ty - 1];

				}
			}
			player[pnum].plusscore();
			player[pnum].DrawBoard(); // ���带 �ٽñ׸�
			if (pnum == 0)
				player[pnum].Printscore();
			std::this_thread::sleep_for(50ms);
		}
		
	}
	if(FullCount>1) // 2���̻� ���� ó���� �������
	AddLine(pnum, FullCount-1);

}

// 2�� �̻� ���� ������ ���濡�� ������ �� - 1 ��ŭ�� ���� �߰���
void controller::AddLine(const short pnum, const short nline) {
	bool gameover{ false };
	for (int i = 0; i < PLAYERNUM; ++i) {
		if (i==pnum) // ������ �÷��̾ �����ϰ� ����
			continue;
		if (!trig[i]) { // ������ �÷��̾ ����
			int y;
			//������ �� ���� �ٺ��� ������ �� �� ��ŭ �������� �÷��� ����
			for (y = BY+1; y < BH+1-nline; ++y) {
				for (int x =1; x < BW + 1; ++x) {
					
	
					player[i].board[x][y] = player[i].board[x][y + nline];
						
				}
			}
			for (y; y < BH + 1; ++y) { // ������ ���� ���ݺ����� �ٲ�
				for (int x = 1; x < BW + 1; ++x) {
					player[i].board[x][y].block = ATTACKBLOCK;
					player[i].board[x][y].color = gtxy::White;
				}
			}
			player[i].DrawBoard(); // ���带 �ٽ� �׸�
			player[i].Printbrick(true, player[i].getbrick(), player[i].getrot());
		}

	}
}
// ���ӿ����ø� ó���ϴ� �Լ�
void controller::gameOver(const short pnum) {
	for (int y = 1; y < BH + 1 ; ++y) {
		for (int x = 1; x < BW + 1; ++x) {


			player[pnum].board[x][y].block=ATTACKBLOCK;
			player[pnum].board[x][y].color = gtxy::White;
		

		}
	}
	player[pnum].DrawBoard();
}
// �������� �ð��� ī��Ʈ �ϴ� �Լ���ü
void Timer::operator()(bool &trig) {
	using namespace std::chrono_literals;
	while (cnt < PLAYERNUM) {
		//������ �ð��� �Ǹ� movecounter�� true�� �ٲ�
		if (std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start).count() > 0.3)
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



// ���� �ð��� ������ ��ĭ �� �������� ����ϴ� �Լ� ��ü
void Movecontroller::operator()() {
	using namespace std::chrono_literals;
	while (cnt < PLAYERNUM)
	{
		if (movecounter) {									 //Timer �Լ���ü���� ������ movecounter�� true�Ͻ� ��� �÷��̾��� ������� ��ĭ�� ����
			controller::moveDownAll();
			movecounter = false;
		}
		std::this_thread::sleep_for(1ms);
	}
}
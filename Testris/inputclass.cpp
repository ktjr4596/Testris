
//2018.10.4
//
//테트리스
//
//inputclass.cpp
//입력 클래스 , 컨트롤러, 타이머 클래스 정의
//-------------------------------------------------------------------

#include"inputclass.h"
#include "gameboard.h"
#include "gotoxy.h"
#include <conio.h>
#include <iostream>
#include <time.h>

static bool movecounter = false; // 일정 시간이 되면 true가 되서 현재 블럭을 한칸 내려오게 하는 변수
bool trig[4] = { false,false,false,false };	// 게임오버가 됐는지 확인하는 불리언 변수
bool ttrig[4] = { false,false,false,false };	// 블럭이 바닥에 내려왔을 때 다음 블럭을 나오게하는 트리거
short cnt = 0;	// 게임오버 된 플레이어를 카운트 하는 변수
const short PLAYERNUM = 4;	// 플레이어 수를 정함
std::mutex queuem; // 큐에 대한 순서제어를 할 뮤텍스
bool inputclass::startflag = false;
std::vector<gameboard> player; // 플레이어의 게임보드를 저장할 벡터

// 키입력을 받는 함수 객체
bool inputclass::operator()(std::queue<int>& q) {
	using namespace std::chrono_literals;
	gtxy::SetCursor(false);
	while (!startflag) { // 게임 시작 화면에서 space를 입력하면 시작
		ch = _getch();
		if (ch == SPACE)
			return true;
		else if (ch == ESC)
			return false;
	}
	while (true) {	// 키 입력을 큐에 저장
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


// 플레이어마다 게임보드 처리를 쓰레드로 병행처리하기 위한 함수
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
		if (pnum == 0) // 1플레이어(자신)만 다음 블럭을 미리 보여줌
		{

			player[pnum].DrawNextBrick(true, nextbrick);

		}
		rot = 0;

		// 블럭이 나오는 위치가 비어있지 않으면 break
		if (controller::getAround(pnum, player[pnum].getnx(), player[pnum].getny(), player[pnum].getbrick(), rot) != EMPTY) break;

		player[pnum].Printbrick(true, player[pnum].getbrick(), 0);
		ttrig[pnum] = false;
		player[pnum].setmtrig(true);
		if (pnum == 0) {
			controller::moveDowna(pnum); // 1플레이어(자신)만 예상 블럭을 출력

			player[pnum].Printbricka(true, player[pnum].getbrick(), player[pnum].getrot(), false);
		}
		while (!ttrig[pnum])	// 내려오는 블럭이 바닥에 닿을때까지 대기
		{

			std::this_thread::sleep_for(1ms);
		}
		if (pnum == 0)
			player[pnum].DrawNextBrick(false, nextbrick);

		std::this_thread::sleep_for(10ns);
	}
	trig[pnum] = true; // 루프가 끝나면(더이상 블럭이 나올 수 없을 시) 게임 종료
	controller::gameOver(pnum);
	cnt++;
}

//컨트롤러 함수객체
void controller::operator()(std::queue<int> & q) {
	
	using namespace std::chrono_literals;
	cnt = 0;
	Timer t;
	std::thread timert(t, std::ref(timetrig)); // 블럭이 내려올 일정시간을 카운트하는 쓰레드 실행
	player.emplace_back();	// 플레이어를 추가
	player.emplace_back();
	player.emplace_back();
	player.emplace_back();


	for (int p = 0; p < player.size(); ++p) {
		trig[p] = false;
	}
	player[playernum].Printscore();
	std::thread p1(moveFunc, 0); // 각 플레이어의 보드를 처리하는 쓰레드
	std::thread p2(moveFunc, 1);
	std::thread p3(moveFunc, 2);
	std::thread p4(moveFunc, 3);
	thread_guard tg1(p1);		
	thread_guard tg2(p2);
	thread_guard tg3(p3);
	thread_guard tg4(p4);


	Movecontroller mc;	
	std::thread mcthread(mc); // 일정시간이되면 현재 블럭을 한칸씩 내리는 쓰레드

	// 플레이어가 살아있는 동안 큐에서 입력을 꺼내 처리
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

// 내려오는 블럭이 닿는 곳이 있는지 확인하는 함수
short controller::getAround(const short pnum, const short  x, const short  y, const short  b, const short  r) {
	short k = 100;

	for (int i = 0; i < 4; ++i) {
		if (pnum == 0)
			k = std::min(k, player[pnum].board[x + shape[b][r][i].x][y + shape[b][r][i].y].block); // 블럭의 주변정보를 비교해서 작은 값을 저장(1=내려온 블럭, 2= 벽, 3= 빈칸)
		else if (pnum > 0)
			k = std::min(k, player[pnum].board[x - (28 + 22 * (pnum - 1)) + shape[b][r][i].x][y + shape[b][r][i].y].block);
		if (k < 3) // 주변에 벽이나 내려온 블럭이 있을 시 벽에서 회전할 경우를 확인
		{
			if (x + shape[b][r][i].x < x) {  //왼쪽 벽에서 회전시 -1
				return -1;
			}
			else if (x + shape[b][r][i].x > x) { // 오른쪽 벽에서 회전시 -2
				return -2;
			}
		}
	}
	return k;
}

// 예상 블럭(내려올 지점) 의 위치를 바꾸는 함수
void controller::moveDowna(const short  pnum) {

	while (getAround(pnum, player[pnum].ax, player[pnum].ay + 1, player[pnum].brick, player[pnum].rot) >= EMPTY) {
		player[pnum].ay += 1;

	}
}

//블럭을 한칸씩 내리는 함수
bool controller::moveDown(const short pnum, const short brick, const short rot, const bool spacebar) {
	std::unique_lock<std::mutex> l(m[pnum], std::defer_lock);

	l.lock();
	// 주변에 블럭이나 벽이 있을 시
	if (getAround(pnum, player[pnum].getnx(), player[pnum].getny() + 1, brick, rot) < EMPTY) {
		l.unlock();
		if (spacebar) // 스페이스바(바로내림)를 눌렀을 때
		{
			player[pnum].printnextbrick(brick, rot); // 바로 블럭을 내림
		}
		if (player[pnum].getmtrig())	// 더이상 내려갈 수 없을 때 삭제할 줄 이 있는 지 확인
			TestFull(pnum, brick, rot);
		if (!spacebar) // 아래 방향키로 내리거나 자동으로 내려갈 때
			player[pnum].setmtrig(true);

		return true;
	}

	//y 좌표를 증가시키고 블럭을 그림
	player[pnum].setny(player[pnum].getny() + 1); 
	l.unlock();
	if (!spacebar)
	{
		player[pnum].printnextbrick(brick, rot);
	
	}

	return false;
}

// 일정 시간이 지났을 때 모든 플레이어의 블럭을 한칸씩 내리는 함수
bool controller::moveDownAll() {
	for (short i = 0; i < PLAYERNUM; ++i) {
		if (!trig[i])	// 살아있는 플레이어만 실행
		{
			if (moveDown(i, player[i].getbrick(), player[i].getrot(), false))
			{
				ttrig[i] = true; // 블럭이 다 내려오면 다음 블럭 출현 트리거를 true로 바꿈
			}
		}

	}

	return false;
}

// 큐에서 뺀 키를 처리하는 함수
bool controller::ProcessKey(const int key) {
	short rotnum = 0;
	switch (key)
	{
	case LEFT: // 왼쪽키 입력시 왼쪽으로 블럭이 이동가능하면 왼쪽으로 이동후 출력
		moveDirection(LEFT);
		break;
	case RIGHT: // 오른쪽
		moveDirection(RIGHT);
		break;
	case UP: //회전 
		trot = (player[playernum].getrot() == 3 ? 0 : player[playernum].getrot() + 1);
		rotnum = getAround(playernum, player[playernum].getnx(), player[playernum].getny(), player[playernum].getbrick(), trot);
		if (rotnum == EMPTY) { // 일반적인 회전시
			moveDirection(UP);
		}

		// 왼쪽 벽에서 회전하면 블럭을 한칸 오른쪽으로 옮긴 후 회전
		else if (rotnum == -1) {
			rotnum = getAround(playernum, player[playernum].getnx() + 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			if (rotnum == EMPTY) {
				moveDirection(-1);
			}
		}
		// 오른쪽 벽에서 회전하면 블럭을 왼쪽으로 옮긴 후 회전
		else if (rotnum == -2) {
			if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) { // ㅡ자 블럭에 대한 오른쪽 벽 회전 처리
				rotnum = getAround(playernum, player[playernum].getnx() - 2, player[playernum].getny(), player[playernum].getbrick(), trot);
			}
			else { // 나머지 블럭에 대한 오른쪽 벽 회전 처리
				rotnum = getAround(playernum, player[playernum].getnx() - 1, player[playernum].getny(), player[playernum].getbrick(), trot);
			}if (rotnum == EMPTY) {

				std::unique_lock<std::mutex> l(m[playernum]);
				player[playernum].Printbricka(false, player[playernum].getbrick(), player[playernum].getrot(), true);

				if (player[playernum].getbrick() == 0 && (trot == 0 || trot == 2)) {// ㅡ자 블럭에 대한 오른쪽 벽 회전 처리
					player[playernum].setnx(player[playernum].getnx() - 2);
					player[playernum].setax(player[playernum].getax() - 2);
				}
				else// 나머지 블럭에 대한 오른쪽 벽 회전 처리
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
	case DOWN: // 아래방향키 처리
		if (moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), false)) // 블럭이 다 내려오면 트리거를 true로 바꿈
		{
			ttrig[playernum] = true; 
			return true;
		}
		break;
	case SPACE: // 스페이스바 처리
		if (!trig[playernum]) { // 게임오버가 안됐을 경우만 실행
			while (!moveDown(playernum, player[playernum].getbrick(), player[playernum].getrot(), true)) {}
			ttrig[playernum] = true;
		}
		return true;


	}
	return false;
}
//블럭을 좌우및 회전 처리를 하는 함수
void controller::moveDirection(const int key) {
	int value=0;
	if (key == LEFT)// kEY가 왼쪽일경우 1칸 왼쪽으로 이동
		value = 1;
	else if (key == RIGHT||key==-1)  //KEY가 오른쪽 혹은 -1(왼쪽벽에서 회전시) 오른쪽으로 한칸 이동
		value = -1;

	if (key == UP) { // 일반적인 회전시
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
// 삭제할 줄이 있는지 확인 하고 처리하는 함수
void controller::TestFull(const short pnum, const short brick, const short rot) {
	using namespace std::chrono_literals;
	int x, y, ty;
	player[pnum].setmtrig(false);
	std::lock_guard<std::mutex> l(m[pnum]);
	for (int i = 0; i < 4; ++i) { // 보드내의 내려온 블럭 위치 상태값을 내려온 블럭(BRICK,1)로 저장
		if (pnum == 0) {

			player[pnum].board[player[pnum].getnx() + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx() + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
		}
		if (pnum > 0) {
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].block = BRICK;
			player[pnum].board[player[pnum].getnx() - (28 + 22 * (pnum - 1)) + shape[brick][rot][i].x][player[pnum].getny() + shape[brick][rot][i].y].color = brick + 1;
		}
	}
	short FullCount{ 0 }; // 공격할 줄-1 을 저장할 변수
	for (y = 1; y < BH + 1; ++y) { 
		for (x = 1; x < BW + 1; ++x) {
			if (player[pnum].board[x][y].block != BRICK) // 한줄씩 확인하며 빈칸이 있으면 break
			{

				break;
			}
		}
		
		if (x == BW + 1) { // 한줄에 빈칸이 없을 시
			FullCount++;
			// 보드내의 블럭정보를 한줄씩 내려 저장
			for (ty = y; ty > 1; --ty) {

				for (x = 1; x < BW + 1; ++x) {

					player[pnum].board[x][ty] = player[pnum].board[x][ty - 1];

				}
			}
			player[pnum].plusscore();
			player[pnum].DrawBoard(); // 보드를 다시그림
			if (pnum == 0)
				player[pnum].Printscore();
			std::this_thread::sleep_for(50ms);
		}
		
	}
	if(FullCount>1) // 2줄이상 동시 처리시 상대방공격
	AddLine(pnum, FullCount-1);

}

// 2줄 이상 동시 삭제시 상대방에게 삭제한 줄 - 1 만큼의 줄을 추가함
void controller::AddLine(const short pnum, const short nline) {
	bool gameover{ false };
	for (int i = 0; i < PLAYERNUM; ++i) {
		if (i==pnum) // 공격한 플레이어를 제외하고 실행
			continue;
		if (!trig[i]) { // 생존한 플레이어만 실행
			int y;
			//공격할 줄 이후 줄부터 공격할 줄 수 만큼 블럭정보를 올려서 저장
			for (y = BY+1; y < BH+1-nline; ++y) {
				for (int x =1; x < BW + 1; ++x) {
					
	
					player[i].board[x][y] = player[i].board[x][y + nline];
						
				}
			}
			for (y; y < BH + 1; ++y) { // 공격할 줄을 공격블럭으로 바꿈
				for (int x = 1; x < BW + 1; ++x) {
					player[i].board[x][y].block = ATTACKBLOCK;
					player[i].board[x][y].color = gtxy::White;
				}
			}
			player[i].DrawBoard(); // 보드를 다시 그림
			player[i].Printbrick(true, player[i].getbrick(), player[i].getrot());
		}

	}
}
// 게임오버시를 처리하는 함수
void controller::gameOver(const short pnum) {
	for (int y = 1; y < BH + 1 ; ++y) {
		for (int x = 1; x < BW + 1; ++x) {


			player[pnum].board[x][y].block=ATTACKBLOCK;
			player[pnum].board[x][y].color = gtxy::White;
		

		}
	}
	player[pnum].DrawBoard();
}
// 내려오는 시간을 카운트 하는 함수객체
void Timer::operator()(bool &trig) {
	using namespace std::chrono_literals;
	while (cnt < PLAYERNUM) {
		//지정한 시간이 되면 movecounter를 true로 바꿈
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



// 일정 시간이 지나면 한칸 씩 내려오게 명령하는 함수 객체
void Movecontroller::operator()() {
	using namespace std::chrono_literals;
	while (cnt < PLAYERNUM)
	{
		if (movecounter) {									 //Timer 함수객체에서 설정한 movecounter가 true일시 모든 플레이어의 현재블럭을 한칸씩 내림
			controller::moveDownAll();
			movecounter = false;
		}
		std::this_thread::sleep_for(1ms);
	}
}
//2018.10.4
//
//테트리스
//
//inputclass.h
//입력 클래스 , 컨트롤러, 타이머 클래스 선언
//-------------------------------------------------------------------


#ifndef INPUT_H
#define INPUT_H


#include <thread>
#include <queue>
#include <vector>
#include <chrono>
#include <mutex>
#define UP 0x48
#define LEFT 0x4b
#define RIGHT 0x4d
#define DOWN 0x50
#define SPACE 0x20
#define ESC 0x1b

class controller;
class gameboard;



// 키입력을 받는 클래스
class inputclass {
private:
	static bool startflag; // 특정 키를 입력받으면 게임이 시작되게할 플래그
	int ch;
public:
	
	inline bool getflag() { return startflag; }
	inline void setflag(const bool f) { startflag = f; }
	
	bool operator()(std::queue<int>&); // 입력한 키를 저장하는 큐를 처리하는 함수 객체

};


// 일정 시간이 지나면 한칸 씩 내려오게 명령하는 클래스 및 함수 객체
class Movecontroller {

public:
	void operator()();
};

// 키입력에 대한 처리 및 게임보드를 컨트롤 하는 컨트롤러 클래스
class controller {
private:
	short brick;	// 현재블럭
	short nextbrick;// 다음 블럭
	short trot = 0;
	short rot;
	short playernum;
	bool timetrig = false; // 일정시간이 지나면 내려오게할 트리거

public:
	controller();
	controller(const controller &) = delete;
	controller & operator()(const controller &) = delete;
	controller(controller &&);
	void operator()(std::queue<int>&); // 입력된 키가 들어있는 큐에서 빼는 함수객체

	
	static void moveDowna(const short); // 예상 블럭(내려올 지점) 의 위치를 바꾸는 함수
	void moveDirection(const int key);	//블럭을 좌우및 회전 처리를 하는 함수
	bool ProcessKey(const int);	// 큐에서 뺀 키를 처리하는 함수
	static bool moveDown(const short,const short,const short,const bool);	//블럭을 한칸씩 내리는 함수
	static void AddLine(const short, const short);		// 2줄 이상 동시 삭제시 상대방에게 삭제한 줄 - 1 만큼의 줄을 추가함
	static void TestFull(const short,const short,const short);	// 삭제할 줄이 있는지 확인 하고 처리하는 함수
	static bool moveDownAll();	// 일정 시간이 지났을 때 모든 플레이어의 블럭을 한칸씩 내리는 함수
	static short getAround(const short,const short  x, const short  y, const short  b, const short  r); // 내려오는 블럭이 닿는 곳이 있는지 확인하는 함수
	static void gameOver(const short);	// 게임오버시를 처리하는 함수
	
	friend class gameboard;
};

// 쓰레드를 안전하게 join할 수 있게끔 쓰레드를 래핑하는 클래스
class thread_guard {
private:
	std::thread & t;
public:
	thread_guard(std::thread &t_) :t(t_) {};
	~thread_guard()// 가드가 소멸할때 변수로 가진 쓰레드가 join가능하면 join 시킴
	{
		if (t.joinable()) { 
			t.join();
		}
	}
	thread_guard(std::thread &&t_) :t(t_) {  };
	thread_guard(const thread_guard &rhs) = delete;
	thread_guard & operator=(const thread_guard & rhs) = delete;
};


// 내려오는 시간을 카운트 하는 클래스 및 함수객체
class Timer {
private:
	std::chrono::high_resolution_clock::time_point start;
public:
	Timer() :start(std::chrono::high_resolution_clock::now()) {};
	void operator()(bool &);
};

#endif // !INPUT_H
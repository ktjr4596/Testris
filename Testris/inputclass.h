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

#define MAX(a,b) (((a)>(b))?(a):(b))
class controller;
class gameboard;
class MoveFunc;

static std::mutex m[4];

class inputclass {
private:
	static bool startflag;
	int ch;
public:
	inline bool getflag() { return startflag; }
	inline void setflag(const bool f) { startflag = f; }
	bool operator()(std::queue<int>&);

};

class Movecontroller {

public:
	void operator()();
};
class controller {
private:
	short brick;
	short nextbrick;
	short trot = 0;
	short rot;
	short playernum;
	bool timetrig = false;

public:
	controller();
	void operator()(std::queue<int>&);
	controller(controller &&);
	static void moveDowna(const short);
	void addboard();
	bool ProcessKey(const int);
	static bool moveDown(const short,const short,const short,const bool);
	
	static void TestFull(const short,const short,const short);
	static bool moveDownAll();
	static short getAround(const short,const short  x, const short  y, const short  b, const short  r);
	friend class gameboard;
	friend class MoveFunc;
};


class thread_guard {
private:
	std::thread & t;
public:
	thread_guard(std::thread &t_) :t(t_) {};
	~thread_guard()
	{
		if (t.joinable()) {
			t.join();
		}
	}
	thread_guard(std::thread &&t_) :t(t_) {  };
	thread_guard(const thread_guard &rhs) = delete;
	thread_guard & operator=(const thread_guard & rhs) = delete;
};

class Timer {
private:
	std::chrono::high_resolution_clock::time_point start;
public:
	Timer() :start(std::chrono::high_resolution_clock::now()) {};
	void operator()(bool &);
};
class MoveFunc {

public:
	void operator()(gameboard&);
	friend class controller;
};
#endif // !INPUT_H
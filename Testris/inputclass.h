#ifndef INPUT_H
#define INPUT_H


#include <thread>
#include <queue>
#include <vector>
#include <chrono>

#define UP 0x48
#define LEFT 0x4b
#define RIGHT 0x4d
#define DOWN 0x50
#define SPACE 0x20
#define ESC 0x1b

#define MAX(a,b) (((a)>(b))?(a):(b))
class controller;
class gameboard;
class inputclass {
private:
	int ch;
public:
	void operator()(std::queue<int>&);

};


class controller {
private:
	short brick;
	short nextbrick;
	short trot = 0;
	short rot;
	short playernum;
	std::vector<gameboard> player;
	bool timetrig = false;

public:
	controller();
	void operator()(std::queue<int>&);
	controller(controller &&);
	void moveDowna(const short);
	void addboard();
	bool ProcessKey(const int);
	bool moveDown(const short);
	bool moveDownAll();
	void TestFull(const short);
	short getAround(const short  x, const short  y, const short  b, const short  r);
	friend class gameboard;
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

#endif // !INPUT_H
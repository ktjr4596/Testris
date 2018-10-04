//2018.10.4
//
//��Ʈ����
//
//inputclass.h
//�Է� Ŭ���� , ��Ʈ�ѷ�, Ÿ�̸� Ŭ���� ����
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



// Ű�Է��� �޴� Ŭ����
class inputclass {
private:
	static bool startflag; // Ư�� Ű�� �Է¹����� ������ ���۵ǰ��� �÷���
	int ch;
public:
	
	inline bool getflag() { return startflag; }
	inline void setflag(const bool f) { startflag = f; }
	
	bool operator()(std::queue<int>&); // �Է��� Ű�� �����ϴ� ť�� ó���ϴ� �Լ� ��ü

};


// ���� �ð��� ������ ��ĭ �� �������� ����ϴ� Ŭ���� �� �Լ� ��ü
class Movecontroller {

public:
	void operator()();
};

// Ű�Է¿� ���� ó�� �� ���Ӻ��带 ��Ʈ�� �ϴ� ��Ʈ�ѷ� Ŭ����
class controller {
private:
	short brick;	// �����
	short nextbrick;// ���� ��
	short trot = 0;
	short rot;
	short playernum;
	bool timetrig = false; // �����ð��� ������ ���������� Ʈ����

public:
	controller();
	controller(const controller &) = delete;
	controller & operator()(const controller &) = delete;
	controller(controller &&);
	void operator()(std::queue<int>&); // �Էµ� Ű�� ����ִ� ť���� ���� �Լ���ü

	
	static void moveDowna(const short); // ���� ��(������ ����) �� ��ġ�� �ٲٴ� �Լ�
	void moveDirection(const int key);	//���� �¿�� ȸ�� ó���� �ϴ� �Լ�
	bool ProcessKey(const int);	// ť���� �� Ű�� ó���ϴ� �Լ�
	static bool moveDown(const short,const short,const short,const bool);	//���� ��ĭ�� ������ �Լ�
	static void AddLine(const short, const short);		// 2�� �̻� ���� ������ ���濡�� ������ �� - 1 ��ŭ�� ���� �߰���
	static void TestFull(const short,const short,const short);	// ������ ���� �ִ��� Ȯ�� �ϰ� ó���ϴ� �Լ�
	static bool moveDownAll();	// ���� �ð��� ������ �� ��� �÷��̾��� ���� ��ĭ�� ������ �Լ�
	static short getAround(const short,const short  x, const short  y, const short  b, const short  r); // �������� ���� ��� ���� �ִ��� Ȯ���ϴ� �Լ�
	static void gameOver(const short);	// ���ӿ����ø� ó���ϴ� �Լ�
	
	friend class gameboard;
};

// �����带 �����ϰ� join�� �� �ְԲ� �����带 �����ϴ� Ŭ����
class thread_guard {
private:
	std::thread & t;
public:
	thread_guard(std::thread &t_) :t(t_) {};
	~thread_guard()// ���尡 �Ҹ��Ҷ� ������ ���� �����尡 join�����ϸ� join ��Ŵ
	{
		if (t.joinable()) { 
			t.join();
		}
	}
	thread_guard(std::thread &&t_) :t(t_) {  };
	thread_guard(const thread_guard &rhs) = delete;
	thread_guard & operator=(const thread_guard & rhs) = delete;
};


// �������� �ð��� ī��Ʈ �ϴ� Ŭ���� �� �Լ���ü
class Timer {
private:
	std::chrono::high_resolution_clock::time_point start;
public:
	Timer() :start(std::chrono::high_resolution_clock::now()) {};
	void operator()(bool &);
};

#endif // !INPUT_H
//2018.10.4
//
//테트리스
//
//gameboard.h
//게임보드클래스 선언
//-------------------------------------------------------------------



#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include <mutex>
#include <queue>
#define BX 5	//보드 시작x좌표
#define BY 5	//보드 시작y좌표
#define BW 15	//보드 너비
#define BH 30	//보드 높이

//블럭좌표 구조체
struct Point 
{
	short x;
	short y;

};
struct Pointex //게임보드내에서 블럭의 좌표 및 상태값을 가진 구조체
{
	short x;
	short y;
	short state;

};


extern std::mutex m[4]; // 플레이어마다 잠금 및 해제 제어 할 뮤텍스 선언
extern Point shape[][4][4];
static std::mutex printm;	//화면에 출력시 순서제어를 위한 뮤텍스 선언

// 블럭의 상태값(블럭 번호, 색상) 저장 구조체
struct boardinfo
{
	short block;
	short color;
};
enum { ATTACKBLOCK, BRICK, WALL, EMPTY, TEMP, TEMPNEXT };

//게임보드 클래스 선언
class gameboard {
private:
	boardinfo board[BW + 2][BH + 2]; 
	short nx;	// 내려오고 있는 블럭의 x,y좌표
	short ny;
	short ax;	// 블럭이 놓여질 지점을 미리보여줄 x,y좌표
	short ay;

	int id;
	int score;
	static int playerid;
	short brick;		
	short rot;	
	bool mtrig;	// 블럭이 더 내려올 수 있는지에 대한 트리거
	Pointex p[4];	// 
	Pointex n[4];

public:
	gameboard();
	//explicit gameboard(int);
	gameboard(gameboard &&) noexcept;
	gameboard& operator=(gameboard &&) noexcept;
	gameboard(const gameboard &) = delete;
	gameboard& operator=(const gameboard &) = delete;

	void DrawScreen();	// 게임보드 틀을 그리는 함수
	void Printbrick(bool, short, short); // 처음 블럭이 화면에 그려지는 함수
	void DrawBoard();	// 플레이어 게임보드 내부를 그리는 함수
	void DrawNextBoard();	// 다음 블럭에 대한 정보를 보여주는 틀을 그려주는 함수
	void DrawNextBrick(bool, short);	// 다음 블럭에 대한 정보를 보여주는 함수
	void Printbricka(bool, short, short, bool);	// 현재 내려오는 블럭의 놓여질 지점을 보여주는 함수
	void Printscore();	
	void printnextbrick(const short, const short);	//내려오는 블럭을 그려주는 함수
	void setxy(const short);
	short getnx() const { return nx; };
	short getax() const { return ax; };
	short getny()const {
		return ny;
	};
	short getay() const {
		return ay;
	};
	inline void setnx(const short x) { nx = x; }
	inline void setny(const short y) { ny = y; }
	inline void setax(const short x) { ax = x; }
	inline void setay(const short y) { ay = y; }
	inline void setbrick(const short b) { brick = b; }
	inline void setrot(const short r) { rot = r; }
	inline short getrot() { return rot; }
	inline int getid() const { return id; };
	inline bool getmtrig() const { return mtrig; }
	inline void setmtrig(const bool t) { mtrig = t; }
	inline short getbrick()const { return brick; }
	inline int getscore() const { return score; };
	inline void plusscore() { score += 1; }
	friend class controller;
};

#endif // !GAMEBOARD_H
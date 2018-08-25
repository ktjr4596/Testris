#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include <mutex>
#include <queue>
#include <shared_mutex>
#define BX 5
#define BY 5
#define BW 15
#define BH 30
struct Point
{
	short x;
	short y;
	
};
struct Pointex
{
	short x;
	short y;
	short state;

};
extern Point shape[][4][4];
static std::mutex printm;
struct boardinfo
{
	short block;
	short color;
};
enum {INIT,BRICK, WALL, EMPTY, TEMP, TEMPNEXT};
static std::queue<int> q1;
class gameboard {
private:
	boardinfo board[BW + 2][BH + 2];
	short nx;
	short ny;
	short ax;
	short ay;
	int id;
	int score;
	static int playerid;
	short brick;
	short rot;
	bool mtrig;
	Pointex p[4];
	Pointex n[4];
	
public:
	gameboard();
	//explicit gameboard(int);
	gameboard(gameboard &&) noexcept;
	gameboard& operator=(gameboard &&) noexcept;
	gameboard(const gameboard &) = delete;
	gameboard& operator=(const gameboard &) = delete;
	~gameboard()
	{

	}
	void DrawScreen();
	void Printbrick(bool, short, short);
	void DrawBoard();
	void DrawNextBoard();
	void DrawNextBrick(bool, short);
	void Printbricka(bool, short, short);
	void Printscore();
	void printnextbrick(const short, const short);
	void setxy(const short);
	short getnx() const { return nx; };
	short getax() const { return ax; };
	short getny ()const {
		return ny;
	};
	short getay () const {
		return ay;
	};
	void setnx(const short x) { nx = x; }
	void setny(const short y) { ny = y; }
	void setax(const short x) { ax = x; }
	void setay(const short y) { ay = y; }
	void setbrick(const short b) { brick = b; }
	void setrot(const short r) { rot = r; }
	short getrot() { return rot; }
	inline int getid() const { return id; };
	bool getmtrig() const { return mtrig; }
	void setmtrig(const bool t) { mtrig = t; }
	short getbrick()const  { return brick; }
	int getscore() const { return score; };
	void plusscore() { score += 1; }
	friend class controller;
};

#endif // !GAMEBOARD_H
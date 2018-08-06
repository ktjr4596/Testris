#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include <mutex>

#define BX 5
#define BY 5
#define BW 15
#define BH 30
struct Point
{
	short x;
	short y;
};
extern Point shape[][4][4];
static std::mutex printm;
struct boardinfo
{
	short block;
	short color;
};
enum { EMPTY, BRICK, WALL };

class gameboard {
private:
	boardinfo board[BW + 2][BH + 2];
	short nx;
	short ny;
	short ax;
	short ay;
	static int id;
	int score;
	int playerid;
	short brick;
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
	void setxy(const short);
	short getnx() const { return nx; };
	short getax() const { return ax; };
	short getny ()const {
		return ny;
	};
	short getay () const {
		return ay;
	};
	void setnx(short x) { nx = x; }
	void setny(short y) { ny = y; }
	void setax(short x) { ax = x; }
	void setay(short y) { ay = y; }
	void setbrick(short b) { brick = b; }
	inline int getid() { return id; };
	friend class controller;
};

#endif // !GAMEBOARD_H
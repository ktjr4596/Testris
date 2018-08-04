#ifndef GAMEBOARD_H
#define GAMEBOARD_H


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
	inline int getid() { return id; };
	friend class controller;
};

#endif // !GAMEBOARD_H
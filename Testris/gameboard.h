//2018.10.4
//
//��Ʈ����
//
//gameboard.h
//���Ӻ���Ŭ���� ����
//-------------------------------------------------------------------



#ifndef GAMEBOARD_H
#define GAMEBOARD_H
#include <mutex>
#include <queue>
#define BX 5	//���� ����x��ǥ
#define BY 5	//���� ����y��ǥ
#define BW 15	//���� �ʺ�
#define BH 30	//���� ����

//����ǥ ����ü
struct Point 
{
	short x;
	short y;

};
struct Pointex //���Ӻ��峻���� ���� ��ǥ �� ���°��� ���� ����ü
{
	short x;
	short y;
	short state;

};


extern std::mutex m[4]; // �÷��̾�� ��� �� ���� ���� �� ���ؽ� ����
extern Point shape[][4][4];
static std::mutex printm;	//ȭ�鿡 ��½� ������� ���� ���ؽ� ����

// ���� ���°�(�� ��ȣ, ����) ���� ����ü
struct boardinfo
{
	short block;
	short color;
};
enum { ATTACKBLOCK, BRICK, WALL, EMPTY, TEMP, TEMPNEXT };

//���Ӻ��� Ŭ���� ����
class gameboard {
private:
	boardinfo board[BW + 2][BH + 2]; 
	short nx;	// �������� �ִ� ���� x,y��ǥ
	short ny;
	short ax;	// ���� ������ ������ �̸������� x,y��ǥ
	short ay;

	int id;
	int score;
	static int playerid;
	short brick;		
	short rot;	
	bool mtrig;	// ���� �� ������ �� �ִ����� ���� Ʈ����
	Pointex p[4];	// 
	Pointex n[4];

public:
	gameboard();
	//explicit gameboard(int);
	gameboard(gameboard &&) noexcept;
	gameboard& operator=(gameboard &&) noexcept;
	gameboard(const gameboard &) = delete;
	gameboard& operator=(const gameboard &) = delete;

	void DrawScreen();	// ���Ӻ��� Ʋ�� �׸��� �Լ�
	void Printbrick(bool, short, short); // ó�� ���� ȭ�鿡 �׷����� �Լ�
	void DrawBoard();	// �÷��̾� ���Ӻ��� ���θ� �׸��� �Լ�
	void DrawNextBoard();	// ���� ���� ���� ������ �����ִ� Ʋ�� �׷��ִ� �Լ�
	void DrawNextBrick(bool, short);	// ���� ���� ���� ������ �����ִ� �Լ�
	void Printbricka(bool, short, short, bool);	// ���� �������� ���� ������ ������ �����ִ� �Լ�
	void Printscore();	
	void printnextbrick(const short, const short);	//�������� ���� �׷��ִ� �Լ�
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
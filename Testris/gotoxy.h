
#ifndef GOTOXY_H
#define GOTOXY_H


namespace gtxy {
	enum { // 컬러명 enum
		Black,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Brown,
		LightGray,
		DarkGray,
		LightBlue,
		LightGreen,
		LightCyan,
		LightRed,
		LightMagenta,
		Yellow,
		White


	};
	void ClearConsoleToColors(int ForgC, int BackC); // 콘솔의 색상 변경
	void gotoxy(short x, short y);
	void SetCursor(bool bVisible); 
}
#endif // !GOTOXY_H#pragma once

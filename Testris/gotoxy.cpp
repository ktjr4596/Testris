#include"gotoxy.h"
#include<Windows.h>


void gtxy::ClearConsoleToColors(int ForgC, int BackC) {
	WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hStdOut, wColor);
	return;
}

void gtxy::gotoxy(short x, short y) {
	COORD pos = { x  , y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void gtxy::SetCursor(bool bVisible) {
	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = bVisible; // true ş¸ŔÓ, false žČş¸ŔÓ
	ConsoleCursor.dwSize = 1; // Äżź­ťçŔĚÁî
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
}
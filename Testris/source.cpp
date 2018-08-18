#include"gotoxy.h"
#include"inputclass.h"
#include "gameboard.h"

#include <thread>
#include<iostream>


void mainmenu();
int main()
{
	//system("clear");
	gtxy::SetCursor(false);
	system("mode con: lines=50 cols=200");
	mainmenu();
	inputclass ic;
	std::queue<int> commandq;
	if (!ic(commandq))
		return 0;
	ic.setflag(true);
	gtxy::gotoxy(70, 25);
	puts("                                                                ");
	gtxy::gotoxy(80, 30);
	puts("                                                                ");
	


	
	
	std::thread inputthread(ic, std::ref(commandq));
	thread_guard g(inputthread);
	controller ctrl;
	

	std::thread ctrlthread(std::move(ctrl), std::ref(commandq));
	ctrlthread.detach();

}

void mainmenu() {
	gtxy::gotoxy(70, 25);
	puts("테\t\t트\t\t리\t\t스");
	gtxy::gotoxy(80, 30);
	puts("시작 : SPACE BAR 종료 : ESC");
}
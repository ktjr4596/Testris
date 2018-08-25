#include"gotoxy.h"
#include"inputclass.h"
#include "gameboard.h"

#include <thread>
#include<iostream>


void mainmenu();
int main()
{
	
	gtxy::SetCursor(false);
	system("mode con: lines=50 cols=200");
	inputclass ic;
	std::queue<int> commandq;

	while (true)
	{
		system("cls");
		mainmenu();

		
		
		if (!ic(commandq))
			break;
		ic.setflag(true);
		std::thread inputthread(ic, std::ref(commandq));
		thread_guard g(inputthread);
		
		{


			
			system("cls");


			
			controller ctrl;


			std::thread ctrlthread(std::move(ctrl), std::ref(commandq));
			thread_guard g1(ctrlthread);
	
			
		}

		system("cls");
		gtxy::gotoxy(70, 25);
		puts("GAME OVER");
		ic.setflag(false);
		while (!commandq.empty()) {
			commandq.pop();
		}

	}
}

void mainmenu() {
	gtxy::gotoxy(70, 25);
	puts("��\t\tƮ\t\t��\t\t��");
	gtxy::gotoxy(80, 30);
	puts("���� : SPACE BAR ���� : ESC");
}
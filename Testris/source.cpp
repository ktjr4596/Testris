#include"gotoxy.h"
#include"inputclass.h"
#include "gameboard.h"


#include<iostream>


void mainmenu(); // 메인메뉴를 출력하는 함수
int main()
{
	
	gtxy::SetCursor(false);
	system("mode con: lines=50 cols=200");
	inputclass ic; // 입력을 받아 큐에 넣는 inputclass 생성
	std::queue<int> commandq; // 입력 키를 저장할 큐 생성

	while (true)
	{
		system("cls");
		mainmenu();
		if (!ic(commandq)) // 입력으로 esc가 들어오면 종료
			break;
		ic.setflag(true); // 게임을 시작하는 플래그를 true로 설정
		std::thread inputthread(ic, std::ref(commandq));	// 커맨드큐를 매개변수로하는 inputclass 스레드 생성
		thread_guard g(inputthread);						 // 스레드가드로 래핑
		
		{
	
			system("cls");

			controller ctrl;
			std::thread ctrlthread(std::move(ctrl), std::ref(commandq));	// 커맨드큐를 매개변수로하는 컨트롤러 스레드 생성 및 래핑
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
	puts("테\t\t트\t\t리\t\t스");
	gtxy::gotoxy(80, 30);
	puts("시작 : SPACE BAR 종료 : ESC");
}
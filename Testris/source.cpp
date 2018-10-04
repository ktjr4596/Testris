#include"gotoxy.h"
#include"inputclass.h"
#include "gameboard.h"


#include<iostream>


void mainmenu(); // ���θ޴��� ����ϴ� �Լ�
int main()
{
	
	gtxy::SetCursor(false);
	system("mode con: lines=50 cols=200");
	inputclass ic; // �Է��� �޾� ť�� �ִ� inputclass ����
	std::queue<int> commandq; // �Է� Ű�� ������ ť ����

	while (true)
	{
		system("cls");
		mainmenu();
		if (!ic(commandq)) // �Է����� esc�� ������ ����
			break;
		ic.setflag(true); // ������ �����ϴ� �÷��׸� true�� ����
		std::thread inputthread(ic, std::ref(commandq));	// Ŀ�ǵ�ť�� �Ű��������ϴ� inputclass ������ ����
		thread_guard g(inputthread);						 // �����尡��� ����
		
		{
	
			system("cls");

			controller ctrl;
			std::thread ctrlthread(std::move(ctrl), std::ref(commandq));	// Ŀ�ǵ�ť�� �Ű��������ϴ� ��Ʈ�ѷ� ������ ���� �� ����
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
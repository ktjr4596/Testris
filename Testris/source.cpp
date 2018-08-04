#include"gotoxy.h"
#include"inputclass.h"
#include "gameboard.h"

#include <thread>
#include<iostream>



int main()
{
	gtxy::SetCursor(false);
	system("mode con: lines=50 cols=200");


	std::queue<int> commandq;
	inputclass ic;
	std::thread inputthread(ic, std::ref(commandq));
	thread_guard g(inputthread);
	controller ctrl;
	ctrl.addboard();
	ctrl.addboard();
	ctrl.addboard();
	ctrl.addboard();

	std::thread ctrlthread(std::move(ctrl), std::ref(commandq));
	ctrlthread.detach();




}
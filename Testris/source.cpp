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
	

	std::thread ctrlthread(std::move(ctrl), std::ref(commandq));
	ctrlthread.detach();




}
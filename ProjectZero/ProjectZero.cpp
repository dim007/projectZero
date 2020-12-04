// ProjectZero.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "Game.h"
#include "Application.h"
#include <iostream>
using namespace std;

int main()
{
	try {
		Application app;
		app.run();
	}
	catch (std::exception& e) {
		cout << "\nEXCEPTION: " << e.what() << endl;
	}
}


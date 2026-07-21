#include<Windows.h>
#include<iostream>

using std::cin;
using std::cout;
using std::endl;

void Plus()
{
	while (true)
	{
		cout << "+ ";
	}
}
void Minus()
{
	while (true)
	{
		cout << "- ";
	}
}

void main()
{
	setlocale(LC_ALL, " ");
	//Plus();
	//Minus();
	HANDLE hThreads[2] = {};
	hThreads[0] = CreateThread
	(
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)Plus,
		NULL,
		NULL,
		NULL
	);
	hThreads[1] = CreateThread
	(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)Minus,
		NULL,
		0,
		NULL
	);
	cin.get();
	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);
}
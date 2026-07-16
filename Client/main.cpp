#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>

// LINK2019 (LNK - Linker, Компоновщик) возникает в том случае, когда компоновщик видит прототип функции (SYMBOL), НО НЕ может сопоставить с ним реализацию функции.
// Это может бытьь из-за того, что список принимаемых параметров в прототипе и реализации отличается либо же реализация вообще нет.
// WS2_32.lib
#pragma comment(lib, "WS2_32.lib")

using std::cin;
using std::cout;
using std::endl;

#define MTU		1500 //Maximum Transfer Unit - максимальный блок данныхб который можно передать по сети для сетей семейства Ethernet MTU составляет 1500 Byte

void main()
{
	setlocale(LC_ALL, "");

	INT iResult; //эта переменная будет хранить результаты работы функций
	//0) Инициализация WinSock
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		return;
	}

	//1) Задаем параметры подключения:
	addrinfo hints;
	addrinfo* target;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;		 // TCP/IP
	hints.ai_socktype = SOCK_STREAM; // SOCK_STREAM и IPPROTO_TCP говорят о том, что мы будем подключать
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("127.0.0.1", "27015", &hints, &target);
	//В 'target' 
	if (iResult)
	{
		cout << "getaddrinfo() failed with error " << iResult << endl;
		WSACleanup();
		return;
	}

	//2) Создаем сокет, при помощи которого бдуем подключаться к Серверу:
	SOCKET connect_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Socket creation failed with error " << WSAGetLastError() << endl;
		freeaddrinfo(target); 
		WSACleanup();
		return;
	}

	//3) Подключаемся к Серверу:
	iResult = connect(connect_socket, target->ai_addr, target->ai_addrlen);
	if (connect_socket == INVALID_SOCKET)
	{
		cout << "Unable to connect to Server. Error: " << WSAGetLastError() << endl;
		closesocket(connect_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}
	freeaddrinfo(target);

	//4) Отправка данных на Сервер:
	CHAR send_buffer[MTU] = "Привет Сервер!";
	iResult = send(connect_socket, send_buffer, strlen(send_buffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Send failed with error: " << WSAGetLastError() << endl;
		closesocket(connect_socket);
		WSACleanup();
		return;
	}
	cout << "Sent" << iResult << " Bytes" << endl;

	//5) Получение данных от Сервера:
	CHAR recv_buffer[MTU] = {};
	iResult = recv(connect_socket, recv_buffer, MTU, NULL);
	if (iResult > 0)cout << iResult << "Byte received. Message: " << recv_buffer << endl;
	else if (iResult == 0)cout << "Nothing received." << endl;
	else cout << "Receive failed with error: " << WSAGetLastError() << endl;

	// Объект 'wsaData' занимает ресурсы памяти, поэтому, после того как WinSock больше не нужен, эти ресурсы нужно освободить: 
	//6) Завершаем сеанс работы с Сервером и освобождаем ресурсы:
	iResult = shutdown(connect_socket, SD_BOTH); // Закрываем соединение с Сервером в обоих направлениях
	if (iResult == SOCKET_ERROR)cout << "Shutdown failed with error: " << WSAGetLastError() << endl;
	closesocket(connect_socket);
	WSACleanup();
}
#include <iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<iphlpapi.h>

using std::cin;
using std::cout;
using std::endl;

#define MTU 1500 //Maximum Transfel Unit
#pragma comment(lib, "WS2_32.lib")

void main()
{
	setlocale(LC_ALL, " ");
	cout << "SERVER" << endl;

	INT iResult = 0;

	//0) Инициализация WinSock:
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	//1) Параметры подключения:
	addrinfo hints;
	addrinfo* target;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // TCP/IP
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // Соединение будет работать в режиме 'LISTENING'

	iResult = getaddrinfo(NULL, "27015", &hints, &target); // NULL - '0.0.0.0'. Сервер будет прослушивать порт '27015' на всех доступных IP-адресах, на всех сетевых картах, установленных на ПК.
	if (iResult) // '0' - это FALSE, TRUE - это все что НЕ '0'
	{
		cout << "getaddrinfo() failed with error: " << iResult << endl;
		WSACleanup();
		return;
	}

	//2) Создаем SOCKET Сервера
	SOCKET listen_socket = socket(target->ai_family, target->ai_socktype, target->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << "LISTEN SOCKET creation failed with error: " << WSAGetLastError() << endl;
		freeaddrinfo(target);
		WSACleanup();
		return;
	}
	
	//3) BIND - привязываем серверный сокет к интерфейсу, который он будет слушать
	iResult = bind(listen_socket, target->ai_addr, target->ai_addrlen);
	if (iResult)
	{
		cout << "BIND failed with error: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//4) Запускаем прослушивание порта
	if (listen(listen_socket, 1) == SOCKET_ERROR)
	{
		cout << "Listen failed with error: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//5) Принимаем подключение от клиентов
	SOCKET client_socket = accept(listen_socket, NULL, NULL);
	if (client_socket == INVALID_SOCKET)
	{
		cout << "Accept failed with error: " << WSAGetLastError() << endl;
		closesocket(listen_socket);
		freeaddrinfo(target);
		WSACleanup();
		return;
	}

	//6) Получение данных от клиента
	CHAR recv_buffer[MTU] = {};
	iResult = recv(client_socket, recv_buffer, MTU, NULL);
	if (iResult > 0)
	{
		cout << iResult << " Bytes received. Message: " << recv_buffer << endl;
	}
	else if (iResult == 0) cout << "Nothing received, connection closing" << endl;
	else cout << "Receive failed with error: " << WSAGetLastError() << endl;

	cin.get(); // Ожидает нажатие клавиши 'Enter'

	//7) Закрываем соединение с клиентом
	if (iResult) cout << "shutdown failed with error: " << WSAGetLastError() << endl;

	//8) Освободить ресурсы
	closesocket(client_socket);
	closesocket(listen_socket);
	freeaddrinfo(target);
	WSACleanup();

	//) Освободить ресурсы:
	WSACleanup();
}
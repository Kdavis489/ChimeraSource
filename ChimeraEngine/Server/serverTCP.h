#ifndef SERVERTCP_H
#define SERVERTCP_H

#include <iostream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif
class servertcp
{
public:
	servertcp() : serverSocket(INVALID_SOCKET)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			std::cerr << "Failed to initialize Winsock" << std::endl;
		}
		std::cout << "Winsock Initialized" << std::endl;
	}

	~servertcp()
	{
		if (serverSocket != INVALID_SOCKET)
		{
			closesocket(serverSocket);
		}
		std::cout << "Socket Closed successfully" << std::endl;
		WSACleanup();
	}

	bool Start(int port)
	{
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket == INVALID_SOCKET)
		{
			std::cerr << "Error creating socket" << std::endl;
			return false;
		}
		std::cout << "Socket created" << std::endl;
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(80);
		serverAddr.sin_addr.s_addr = INADDR_ANY;

		if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
		{
			std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			return false;
		}
		std::cout << "Socket Bound" << std::endl;

		if (listen(serverSocket, SOMAXCONN))
		{
			std::cerr << "Listen failed:" << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			return false;
		}

		std::cout << "Server listening" << std::endl;
		return true;
	}
	void Accept()
	{
		FD_ZERO(&master);
		FD_SET(serverSocket, &master);
		SOCKET maxSocket = serverSocket;
		while (true) 
		{
			fd_set copy = master;
			int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

			for (int i = 0; i <= maxSocket && socketCount > 0; ++i)
			{
				if (FD_ISSET(i, &copy))
				{
					if (i == serverSocket)
					{
						SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
						FD_SET(clientSocket, &master);
						if (clientSocket > maxSocket)
						{
							maxSocket = clientSocket;
						}
						std::cout << "new connection accepted" << std::endl;
					}
					else
					{
						char buffer[1024];
						memset(buffer, 0, sizeof(buffer));
						int bytesReceived = recv(i, buffer, sizeof(buffer), 0);
						if (bytesReceived <= 0)
						{
							std::cout << "Client disconnected" << std::endl;
							closesocket(serverSocket);
							FD_CLR(i, &master);
						}
						else
						{
							std::cout << "Received: " << buffer << std::endl;
						}
					}
				}
			}
			--socketCount;
		}
	}

private:
	fd_set master;
	SOCKET serverSocket;
};
#endif
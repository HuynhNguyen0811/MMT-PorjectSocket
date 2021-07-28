#define	WIN32_LEAN_AND_MEAN

#include<iostream>
#include<WS2tcpip.h>
#include<sstream>
#include<string>
#pragma comment (lib, "ws2_32.lib")
using namespace std;

#define PORT 8080


int main() {
	string ss;
	//initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);

	if (wsok != 0) {
		cout << "can't initialize quiting " << endl;
		return 1;
	}

	//create a socket

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cout << "can't create a socket quiting" << endl;
		return 1;
	}
	else {
		cout << "waiting for client" << endl;
	}
	//blind socket with ip address and port

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; //COULD ALSO USE INET_PTON



	bind(listening, (sockaddr*)&hint, sizeof(hint));



	//tell winsock the socket is for listenning
	listen(listening, SOMAXCONN);

	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true) {

		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);    //queue

		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];

			if (sock == listening) {
				//Accept new connection
				cout << endl;
				sockaddr_in client;
				int clientsize = sizeof(client);

				//add new connection to list
				SOCKET clientSocket = accept(listening, NULL, NULL);
				if (clientSocket == INVALID_SOCKET) {
					cout << " ERROR #" << WSAGetLastError() << endl;
					closesocket(sock);
					FD_CLR(sock, &master);
					continue;
				}
				FD_SET(clientSocket, &master);
				getpeername(clientSocket, (sockaddr*)&client, &clientsize);
				//Send welcome message
				string welcome = "Welcome to Chat Server";
				send(clientSocket, welcome.c_str(), welcome.size() + 1, 0);

				//TODO:
				char host[NI_MAXHOST]; //CLIENT'S REMOTE NAME
				char service[NI_MAXSERV];// service(i.e port) the client is connect on


				ZeroMemory(host, NI_MAXHOST); //same as memset(host, 0 ,NI_)
				ZeroMemory(service, NI_MAXSERV);
				if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
					cout << host << "connected on port" << service << endl;
				}
				else {
					inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
					char* ip = inet_ntoa(client.sin_addr);
					cout << ip << "connected on port" << ntohs(client.sin_port) << endl;
				}

				while (true) {
					//while loop: accecpt and echo message back to client
					break;

				}
			}
			else {
				ss = "Username";
				send(sock, ss.c_str(), ss.size(), 0);
				char buf[1024];

				ZeroMemory(buf, 1024);

				//wait client send data

				int byteReceived = recv(sock, buf, 1024, 0);

				if (byteReceived == SOCKET_ERROR) {
					cout << " error in recv" << sock << "quiting" << endl;
					closesocket(sock);
					FD_CLR(sock, &master);
					continue;
				}
				if (byteReceived <= 0) {
					cout << "client " << sock << " disconnected" << endl;
					closesocket(sock);
					FD_CLR(sock, &master);
					continue;
				}
				else {
					cout << "Client " << sock << " sent > " << buf << endl;
				}

				//echo message back to client
				//cout << ">";
				//getline(cin, ss);


			}
		}
	}
		//cleanup winsock
	WSACleanup();

	return 0;
}



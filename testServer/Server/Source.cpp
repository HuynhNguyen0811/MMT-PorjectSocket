#define	WIN32_LEAN_AND_MEAN

#include<iostream>
#include<WS2tcpip.h>
#include<sstream>
#include<string>
#include <fstream>

//json
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#pragma comment (lib, "ws2_32.lib")

using namespace std;

#define PORT 8080

void charToString(string &destination, char* source, int n) {
	for (int i = 0; i < n; i++) {
		destination += source[i];
	}
}

//check login
bool checkLogin(string user, string pass) {
	ifstream fileIn;
	fileIn.open("login.txt");

	string checkUser, checkPass;
	while (fileIn) {
		getline(fileIn, checkUser, ',');
		getline(fileIn, checkPass, '\n');
		if (checkUser == user && checkPass == pass) {
			fileIn.close();
			return 1;
		}
	}

	fileIn.close();
	return 0;
}

//check bi trung username
bool checkRegis(string user) {
	ifstream fileIn;
	fileIn.open("login.txt");

	if (fileIn.fail()) return 1;


	string checkUser, checkPass;
	while (fileIn) {
		getline(fileIn, checkUser, ',');
		getline(fileIn, checkPass, '\n');
		if (checkUser == user) {
			fileIn.close();
			return 0;
		}
	}

	fileIn.close();	
	return 1;
}

//them tai khoan moi dang ky
void regis(string user, string pass) {
	ofstream fileOut;
	fileOut.open("login.txt", ios_base::app);

	fileOut << user << "," << pass << endl;

	fileOut.close();
}

bool searchDataByDate(std::string date, Document& doc) {
	std::ifstream fileIn;
	fileIn.open("Data\\" + date + ".txt");
	//string a = "Data\\" + date + ".txt";
	//cout << a;
	if (fileIn.fail()) return 0;

	std::string temp;

	getline(fileIn, temp);
	doc.Parse(temp.c_str());

	fileIn.close();
	return 1;
}

bool searchDataByCountryName(std::string name, std::string date, string& res) {
	Document doc;
	if (searchDataByDate(date, doc)) {
		for (int i = 0; i < doc.Size(); i++) {
			cout << doc[i]["country"].GetString() << endl;
			if (doc[i]["country"].GetString() == name) {
				StringBuffer buffer;
				rapidjson::Writer<StringBuffer> writer(buffer);
				doc[i].Accept(writer);

				res = buffer.GetString();
				//cout << data["cases"].GetInt64();
				//cout << data["country"].IsString();
				return 1;
			}
		}
	}
	return 0;
}

int main() {
	string ss;
	string temp1, temp2;
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
				string welcome = "Welcome to Chat Server\n1. Login\n2. Regis";
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
					//char* ip = inet_ntoa(client.sin_addr);
					cout << host << "connected on port" << ntohs(client.sin_port) << endl;
				}
				char buf[1024];
				ZeroMemory(buf, 1024);

				int byteReceived = recv(clientSocket, buf, 1024, 0);
				if (byteReceived == SOCKET_ERROR) {
					cout << "client " << sock << " disconnected" << endl;
					closesocket(sock);
					FD_CLR(sock, &master);
					continue;
				}

				if (buf[0] == '1') cout << "Login"; else cout << "Regis";
				if (strcmp(buf, "1") == 0) {
					string username = "", password = "";
					string message = "Please enter your username and password\nUsername: ";
					send(clientSocket, message.c_str(), message.size() + 1, 0);
					ZeroMemory(buf, 1024);
					byteReceived = recv(clientSocket, buf, 1024, 0);
					username = string(buf, byteReceived);

					message = "Password: ";
					send(clientSocket, message.c_str(), message.size() + 1, 0);
					ZeroMemory(buf, 1024);
					byteReceived = recv(clientSocket, buf, 1024, 0);
					password = string(buf, byteReceived);
					if (checkLogin(username, password)) {
						message = "Login successful\nEnter name of country you want to know Covid situation\n";
						send(clientSocket, message.c_str(), message.size() + 1, 0);

						ss = "dummy";
						send(sock, ss.c_str(), ss.size(), 0);
						break;
					}
					else {
						closesocket(clientSocket);
						FD_CLR(clientSocket, &master);
					}
				}
				else if (strcmp(buf, "2") == 0) {
					string username, password;
					string message = "Please enter your username and password\nUsername: ";
					send(clientSocket, message.c_str(), message.size() + 1, 0);
					ZeroMemory(buf, 1024);
					byteReceived = recv(clientSocket, buf, 1024, 0);
					username = string(buf, byteReceived);

					message = "Password: ";
					send(clientSocket, message.c_str(), message.size() + 1, 0);
					ZeroMemory(buf, 1024);
					byteReceived = recv(clientSocket, buf, 1024, 0);
					password = string(buf, byteReceived);

					if (checkRegis(username)) {
						regis(username, password);
						message = "Regis successful\nEnter name of country you want to know Covid situation\n";
						send(clientSocket, message.c_str(), message.size() + 1, 0);

						ss = "dummy";
						send(sock, ss.c_str(), ss.size(), 0);
						break;
					}
					else {
						closesocket(clientSocket);
						FD_CLR(clientSocket, &master);
					}
				}
				else {
					string message = "Invalid output\n";
					send(clientSocket, message.c_str(), message.size() + 1, 0);
					closesocket(clientSocket);
					FD_CLR(clientSocket, &master);
				}
			}
			else {
				char buf[1024];

				ZeroMemory(buf, 1024);

				//wait client send data

				int byteReceived = recv(sock, buf, 1024, 0);

				if (byteReceived == SOCKET_ERROR) {
					cout << "client " << sock << " disconnected" << endl;
					closesocket(sock);
					FD_CLR(sock, &master);
					continue;
				}

				else {
					cout << "Client " << sock << " sent > " << buf << endl;
					string res;
					if (searchDataByCountryName(buf, "26072021", res)) {
						send(sock, res.c_str(), res.size(), 0);
					}
					else {
						res = "Can't find";
						send(sock, res.c_str(), res.size(), 0);
					}
				}
			}
		}
	}
	WSACleanup();

	return 0;
}




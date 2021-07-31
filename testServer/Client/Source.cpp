
#include<iostream>
#include<WS2tcpip.h>
#include<string>

#include "rapidjson/document.h"
using namespace rapidjson;

#pragma comment(lib, "ws2_32.lib")

using namespace std;
#define PORT 8080
int main() {
	string ipAddress = "127.0.0.1"; //ipaddress of server

	/*cout << " input IP Address of Server:";
	getline(std::cin, ipAddress);*/

	//initialise winsock

	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);

	if (wsResult != 0) {
		cout << "can't start winsock, error #" << wsResult << endl;
		return 1;
	}
	else cout << "winsock started" << endl;

	//create winsock

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cout << "can't create socket #" << WSAGetLastError() << endl;
		return 1;
	}
	else cout << "winsock created" << endl;
	//fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(PORT);
	//inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
	DWORD ip = inet_addr(ipAddress.c_str());
	hint.sin_addr.S_un.S_addr = ip;
	//connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cout << "can't connect to server, error#  " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	else {
		cout << " * enter something to start * " << endl;
	}

	char buf[1024];
	string userInput;

	ZeroMemory(buf, 1024);
	int byteReceived = recv(sock, buf, 1024, 0);
	cout << buf;

	string option;
	cin >> option;
	int sendResult = send(sock, option.c_str(), option.size() + 1, 0);

	ZeroMemory(buf, 1024);
	recv(sock, buf, 1024, 0);
	cout << buf;
	cin.ignore(1000, '\n');
	getline(cin, userInput);
	send(sock, userInput.c_str(), userInput.size() + 1, 0);

	ZeroMemory(buf, 1024);
	recv(sock, buf, 1024, 0);
	cout << buf;
	getline(cin, userInput);
	send(sock, userInput.c_str(), userInput.size() + 1, 0);

	do {
		//Promt the user for some text
		ZeroMemory(buf, 1024);
		byteReceived = recv(sock, buf, 1024, 0);
		if (byteReceived <= 0) {
			cout << "server disconnected " << endl;
			closesocket(sock);
			WSACleanup();
			system("PAUSE");
			return 1;
		}
		else
			if (byteReceived > 0) {
				Document document;
				document.Parse(buf);
				if (document.IsObject()) {
					cout << "SERVER>Country: " << document["country"].GetString() << " Cases: " << document["cases"].GetInt64() << endl;
				}
				else {
					cout << "SERVER>" << buf << endl;
				}
			}
		cout << ">";
		getline(cin, userInput);

		if (userInput.size() > 0) {
			sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult == SOCKET_ERROR) {
				cout << "server disconnected " << endl;
				closesocket(sock);
				WSACleanup();
				system("PAUSE");
				return 1;
			}
		}
	} while (userInput.size() > 0);
	closesocket(sock);
	WSACleanup();

	return 0;
}
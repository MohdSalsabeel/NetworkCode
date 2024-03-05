#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include <string>
using namespace std;

/*
1. Initialize Socket
2. Create Socket
3. Connect to the socket
4. send/recv
5. close the socket
*/

#pragma comment(lib,"ws2_32.lib")

bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}


void SendMessageToServer(SOCKET s) {

	cout << "Enter your chat name : " << endl;
	string name;
	getline(cin, name);
	string message;

	while (1) {
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent=send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR) {
			cout << "error sending message " << endl;
			break;
		}

		if (message == "quit") {
			cout << "Stopping the application" << endl;
			break;
		}
	}

	closesocket(s);
	WSACleanup();
}

void ReceiveMessageFromServer(SOCKET s) {
	char buffer[4096];
	int recvlength;
	string msg = "";
		
	while (1) {
		recvlength=recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "Disconnected from the server" << endl;
		}
		else {
			msg = string(buffer, recvlength);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main() {

	cout << "Client program started " << endl;

	if (!Initialize()) {
		cout << "WinSoc initialization failed" << endl;
		return 1;
	}

	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "Socket creation failed" << endl;
		return 1;
	}
	//string serveraddress = "172.0.0.1";
	string serveraddress = "192.168.0.105";
	int port = 123;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Not able to connect to server" << endl;
		cout << ":" << WSAGetLastError() << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connect to server" << endl;

	thread senderthread(SendMessageToServer, s);
	thread receiverthread(ReceiveMessageFromServer, s);

	senderthread.join();
	receiverthread.join();

	//send and recieve
	/*string message = "Hello Salsabeel";
	int bytesent = send(s, message.c_str(), message.length(), 0);

	if (bytesent == SOCKET_ERROR) {
		cout << "send failed" << endl;
	}*/

	//closesocket(s);



	//WSACleanup();
	return 0;
}



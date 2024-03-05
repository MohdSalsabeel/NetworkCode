// NetworkCode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*
1. initialize winsoc library
2. create the socket
3. get ip and port
4. bind ip and port with the socket
5. listen on the socket.
6. accept
7. receive and send
8. close the socket
9. clean up the socket.


*/




#include <iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

#pragma comment(lib,"ws2_32.lib")

using namespace std;

bool Initialize() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2,2),&data)==0;
}

void InteractWithClient(SOCKET clientSocket,vector<SOCKET>& clients) {
    //send and receive with client
    char buffer[4096];
    while (1) {
        int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesrecvd <= 0) {
            cout << "Client disconnected " << endl;
            break;
        }

        string message(buffer, bytesrecvd);
        cout << "Message from client: " << message << endl;

        for (auto client : clients) {
            if (client != clientSocket) {
                send(client, message.c_str(), message.length(), 0);
            }
        }

    }

    auto it = find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end()) {
        clients.erase(it);

    }

    closesocket(clientSocket);
}

int main()
{

    if (!Initialize()) {
        cout << "WinSoc initialization failed" << endl;
        return 1;
    }



    std::cout << "Hello World!\n";

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        return 1;
    }

    //create address structure
    int port = 123;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);

    //convert the ipaddress(0.0.0.0) put it inside the sin_family into binary format
    if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
        cout << "setting address structure failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }


    //bind
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "bind failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    //listen
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "listen failed" << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server has start listening port: " <<port<< endl;

    vector<SOCKET> clients;

    while (1) {
        //accept
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "invalid client socket " << endl;
        }

        clients.push_back(clientSocket);    
        thread t1(InteractWithClient, clientSocket,ref(clients));

        t1.detach();
    }

    
    closesocket(listenSocket);

    WSACleanup();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

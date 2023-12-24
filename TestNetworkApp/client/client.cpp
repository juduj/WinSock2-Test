#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <thread>


#pragma comment(lib, "ws2_32.lib")

void receiveMessages(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Server details
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(54000); // Server's port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP (localhost)
    //InetPton(AF_INET, _T("127.0.0.1"), &RecvAddr.sin_addr.s_addr);

    // Connect to server
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::thread recvThread(receiveMessages, clientSocket);

    // Chat functionality
    char buffer[4096];
    int bytesSent;

    while (true) {
        // Send message to the server
        std::cout << "Enter message: ";
        std::cin.getline(buffer, sizeof(buffer));
        bytesSent = send(clientSocket, buffer, strlen(buffer), 0);

        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Failed to send message" << std::endl;
            break;
        }
    }

    // Close socket and cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
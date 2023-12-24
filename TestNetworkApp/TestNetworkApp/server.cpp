#include <iostream>
#include <WinSock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")


void clientHandler(SOCKET clientSocket) {
    char buffer[4096];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Client: " << buffer << std::endl;

            // Send the received message to other clients (if needed)
            // Implement this based on your chat logic
        }
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Create a socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Accept connections on any interface
    serverAddr.sin_port = htons(54000); // Use port 54000

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Binding failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listening failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server is waiting for clients to connect..." << std::endl;

    // Accept two client connections
    SOCKET clientSocket1 = accept(serverSocket, NULL, NULL);
    SOCKET clientSocket2 = accept(serverSocket, NULL, NULL);
    //SOCKET clientSocket3 = accept(serverSocket, NULL, NULL);

    if (clientSocket1 == INVALID_SOCKET || clientSocket2 == INVALID_SOCKET /*|| clientSocket3 == INVALID_SOCKET*/ ) {
        std::cerr << "Failed to accept client connections" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "2 clients (people) have connected to the server" << std::endl;
    // Create threads to handle clients
    std::thread threadClient1(clientHandler, clientSocket1);
    std::thread threadClient2(clientHandler, clientSocket2);


    // Join threads to the main thread
    threadClient1.join();
    threadClient2.join();

    // Chat functionality
    char buffer[4096];
    int bytesReceived;

    while (true) {
        // Receive message from client 1
        bytesReceived = recv(clientSocket1, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Client 1: " << buffer << std::endl;

            // Send the received message to client 2
            send(clientSocket2, buffer, bytesReceived, 0);
        }

        // Receive message from client 2
        bytesReceived = recv(clientSocket2, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << "Client 2: " << buffer << std::endl;

            // Send the received message to client 1
            send(clientSocket1, buffer, bytesReceived, 0);
        }
    }

    // Close sockets and cleanup
    closesocket(clientSocket1);
    closesocket(clientSocket2);
   // closesocket(clientSocket3);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
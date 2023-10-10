#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE 4096
#define SERVER_PORT 8888

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind server socket to IP and port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen for connections." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server started. Waiting for incoming connections..." << std::endl;

    // Accept incoming connections
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to accept incoming connection." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Display client information
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
    std::cout << "Client connected: " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

    // Send and receive messages
    while (true) {
        // Receive message from client
        int bytesRead = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Client: " << buffer << std::endl;

            // Echo the message back to the client
            if (send(clientSocket, buffer, bytesRead, 0) == SOCKET_ERROR) {
                std::cerr << "Failed to send message to client." << std::endl;
                closesocket(clientSocket);
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }
        }
        else if (bytesRead == 0) {
            // Client closed the connection
            std::cout << "Client closed the connection." << std::endl;
            break;
        }
        else {
            // Error occurred
            std::cerr << "Error in receiving message from client." << std::endl;
            closesocket(clientSocket);
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Send message to client
        std::cout << "Server: ";
        std::cin.getline(buffer, MAX_BUFFER_SIZE);

        if (send(clientSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send message to client." << std::endl;
            closesocket(clientSocket);
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }
    }

    // Close sockets and cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
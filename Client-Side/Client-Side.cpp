#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_SIZE 4096
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    SOCKADDR_IN serverAddr;
    char buffer[MAX_BUFFER_SIZE];

        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock." << std::endl;
            return 1;
        }

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create client socket." << std::endl;
        WSACleanup();
        return 1;
    }

    // Set server address information
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_ADDRESS, &(serverAddr.sin_addr)) <= 0) {
        std::cerr << "Invalid server address." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Connect to the server
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to the server." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    // Send and receive messages
    while (true) {
        // Send message to server
        std::cout << "Client: ";
        std::cin.getline(buffer, MAX_BUFFER_SIZE);

        if (send(clientSocket, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            std::cerr << "Failed to send message to server." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // Receive message from server
        int bytesRead = recv(clientSocket, buffer, MAX_BUFFER_SIZE, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        }
        else if (bytesRead == 0) {
            // Server closed the connection
            std::cout << "Server closed the connection." << std::endl;
            break;
        }
        else {
            // Error occurred
            std::cerr << "Error in receiving message from server." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }

    // Close socket and cleanup
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
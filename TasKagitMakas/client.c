#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    const char* moves[] = {"rock", "paper", "scissors"};
    int choice;

    // Winsock initialization
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address.\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed. Error code: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server!\n");

    // Game loop
    while (1) {
        // Choose a move
        printf("Choose your move (1: rock, 2: paper, 3: scissors): ");
        scanf("%d", &choice);
        if (choice < 1 || choice > 3) {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        // Send move to server
        send(client_socket, moves[choice - 1], strlen(moves[choice - 1]), 0);

        // Receive the result from server
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received == SOCKET_ERROR) {
            printf("Failed to receive data. Error code: %d\n", WSAGetLastError());
            break;
        }
        buffer[bytes_received] = '\0';

        // Print the result
        printf("Game result: %s\n", buffer);
    }

    // Close socket
    closesocket(client_socket);
    WSACleanup();

    return 0;
}

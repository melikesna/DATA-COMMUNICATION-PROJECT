#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

// Function to determine the winner of the game
const char* determine_winner(const char* move1, const char* move2) {
    if (strcmp(move1, move2) == 0) {
        return "It's a tie!";
    } else if ((strcmp(move1, "rock") == 0 && strcmp(move2, "scissors") == 0) ||
               (strcmp(move1, "scissors") == 0 && strcmp(move2, "paper") == 0) ||
               (strcmp(move1, "paper") == 0 && strcmp(move2, "rock") == 0)) {
        return "Player 1 wins!";
    } else {
        return "Player 2 wins!";
    }
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket1, client_socket2;
    struct sockaddr_in server, client1, client2;
    int c;
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];

    // Winsock initialization
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Winsock initialization failed. Error code: %d\n", WSAGetLastError());
        return 1;
    }

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed. Error code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(SERVER_PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed. Error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen for connections
    listen(server_socket, 2);  // We expect 2 players
    printf("Waiting for players to connect...\n");

    c = sizeof(struct sockaddr_in);

    // Accept the first player
    client_socket1 = accept(server_socket, (struct sockaddr *)&client1, &c);
    if (client_socket1 == INVALID_SOCKET) {
        printf("Failed to accept player 1. Error code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Player 1 connected: %s:%d\n", inet_ntoa(client1.sin_addr), ntohs(client1.sin_port));

    // Accept the second player
    client_socket2 = accept(server_socket, (struct sockaddr *)&client2, &c);
    if (client_socket2 == INVALID_SOCKET) {
        printf("Failed to accept player 2. Error code: %d\n", WSAGetLastError());
        closesocket(client_socket1);
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Player 2 connected: %s:%d\n", inet_ntoa(client2.sin_addr), ntohs(client2.sin_port));

    // Main game loop
    while (1) {
        // Receive move from player 1
        int bytes_received1 = recv(client_socket1, buffer1, BUFFER_SIZE, 0);
        if (bytes_received1 == SOCKET_ERROR) {
            printf("Failed to receive data from player 1. Error code: %d\n", WSAGetLastError());
            break;
        }
        buffer1[bytes_received1] = '\0';

        // Receive move from player 2
        int bytes_received2 = recv(client_socket2, buffer2, BUFFER_SIZE, 0);
        if (bytes_received2 == SOCKET_ERROR) {
            printf("Failed to receive data from player 2. Error code: %d\n", WSAGetLastError());
            break;
        }
        buffer2[bytes_received2] = '\0';

        // Show the moves
        printf("Player 1's move: %s\n", buffer1);
        printf("Player 2's move: %s\n", buffer2);

        // Determine the winner
        const char* result = determine_winner(buffer1, buffer2);
        printf("Result: %s\n", result);

        // Optionally, send the result back to the players
        send(client_socket1, result, strlen(result), 0);
        send(client_socket2, result, strlen(result), 0);
    }

    // Close sockets
    closesocket(client_socket1);
    closesocket(client_socket2);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}

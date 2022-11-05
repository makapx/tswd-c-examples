/**
 * @file client.c
 * @author makapx (https://github.com/makapx)
 * @brief Client program for the book store
 * @version 0.1
 * @date 2022-11-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 3030
#define BUFSIZE 1024
#define SERVER_IP "0.0.0.0"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: bookclient <bookname>");
        exit(1);
    }

    int client_socket;
    int retcode;
    struct sockaddr_in server_addr;

    // receive buffer
    char buf[BUFSIZE];
    memset(buf, 0, BUFSIZE);

    // buffer for the book name, readed from the command line
    char bookname[BUFSIZE];
    memset(bookname, 0, BUFSIZE);
    strncpy(bookname, argv[1], BUFSIZE);
    bookname[strlen(bookname)] = '\0'; // add the null terminator. End of string.

    // creating the socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        perror("creating client socket");
        exit(1);
    }

    // setting the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // connecting to the server
    retcode =
        connect(client_socket, (struct sockaddr *)&server_addr,
                sizeof(server_addr));
    if (retcode == -1) // error
    {
        perror("connecting to server");
        exit(1);
    }

    printf("Connecting to server. Please wait...\n");
    sleep(1); // pretend to be busy

    sendto(client_socket, bookname, strlen(bookname), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    recv(client_socket, buf, BUFSIZE, 0);

    if (strcmp(buf, "Not found") == 0)
    {
        printf("Book not found\n");
        exit(1);
    }

    printf("Book found\nTitle - Author - Price\n");
    printf("%s", buf);
}
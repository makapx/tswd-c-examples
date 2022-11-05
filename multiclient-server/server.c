/**
 * @file server.c
 * @author makapx (https://github.com/makapx)
 * @brief Server program for the book store
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

#include <string.h>
#include <unistd.h>

#define PORT 3030    // the port used by the server
#define BUFSIZE 1024 // buffer size for receiving requests
#define BOOKCOUNT 10 // number of books in the store
#define CONNMAX 10   // maximum number of connections

struct book
{
    char title[100];
    char author[100];
    int price;
};

// TODO: considering use a file to store the books
struct book booklist[BOOKCOUNT] = {
    {"The Picture of Dorian Gray", "Oscar Wilde", 15},
    {"Hirtettyjen kettujen metsä", "Arto Paasilinna", 20},
    {"Orlando", "Virginia Woolf", 10},
    {"The Perks of Being a Wallflower", "Stephen Chbosky", 18},
    {"The Left Hand of Darkness", "Ursula K. Le Guin", 25},
    {"Harry Potter and the Philosopher's Stone", "J.K. Rowling", 20},
    {"The Handmaid's Tale", "Margaret Atwood", 10},
    {"The Lord of the Rings", "J.R.R. Tolkien", 30},
    {"The Hitchhiker's Guide to the Galaxy", "Douglas Adams", 15},
    {"Perché sono vegetariana", "Margherita Hack", 13}};

int main()
{

    int server_socket_fd;
    int client_socket_fd;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // listen on all interfaces

    server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // check if the socket was created
    if (server_socket_fd == -1)
    {
        perror("socket");
        exit(1);
    }

    // bind the socket to the server address, and check if it was successful
    if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    // listen for connections
    if (listen(server_socket_fd, CONNMAX) == -1)
    {
        perror("listen");
        exit(1);
    }

    // loop, waiting for connections
    while (1)
    {
        printf("Waiting for connection...\n");
        client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);

        // check if the connection was successful
        if (client_socket_fd == -1)
        {
            perror("accept");
            exit(1);
        }

        printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // create a child process to handle the request
        if (fork() == 0)
        {
            printf("Child process created. Handling client request...\n");
            char recv_buff[BUFSIZE];
            memset(recv_buff, 0, BUFSIZE);
            while (1)
            {
                // read the request from the client. Blocking call
                int len = recv(client_socket_fd, recv_buff, sizeof(recv_buff), 0);

                // check for errors
                if (len == -1)
                {
                    perror("recv");
                    exit(1);
                }

                // check if the client closed the connection
                if (len == 0)
                {
                    printf("client closed\n");
                    exit(1);
                }

                // print the request
                printf("Received %d bytes from client: %s\n", len, recv_buff);

                // search for the book
                for (int i = 0; i < BOOKCOUNT; i++)
                {
                    if (strcmp(recv_buff, booklist[i].title) == 0)
                    {
                        char send_buff[BUFSIZE];

                        // print on the server the book found
                        printf("Book found: %s, %s, %d\n", booklist[i].title, booklist[i].author, booklist[i].price);

                        // send the book info back to the client
                        sprintf(send_buff, "%s %s %d \n", booklist[i].title, booklist[i].author, booklist[i].price);
                        send(client_socket_fd, send_buff, strlen(send_buff), 0);
                        return 1;
                    }
                }

                // if the book was not found, send a message to the client
                send(client_socket_fd, "Not found", 9, 0);
                printf("Book not found\n");
            }
        }
    }
}

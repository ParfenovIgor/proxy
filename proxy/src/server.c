#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>

#include <server.h>
#include <client.h>
#include <error.h>

// Maximum number of client connections at one time, using in listen call
#define N_CONNECTIONS 20

// Starts server using given options and returns status
int server_execute(struct Options *options) {
    int server_sock;
    if ((server_sock = server_create_socket(options)) < 0) {
        return server_sock;
    }

    return server_loop(options, server_sock);
}

// Prepares socket using given options and returns either the socket descriptor or an error
int server_create_socket(struct Options *options) {
    int server_sock = -1;
    int optval = 1;
    struct addrinfo hints;
    struct addrinfo *server_addrinfo = NULL;
    char portstr[12];

    memset(&hints, 0, sizeof(hints));

    hints.ai_flags      = AI_NUMERICSERV | AI_PASSIVE;
    hints.ai_socktype   = SOCK_STREAM;
    hints.ai_family     = AF_INET6;

    sprintf(portstr, "%d", options->local_port);

    if (getaddrinfo(NULL, portstr, &hints, &server_addrinfo) != 0) {
        return SERVER_RESOLVE_ERROR;
    }

    if ((server_sock = socket(server_addrinfo->ai_family, server_addrinfo->ai_socktype, server_addrinfo->ai_protocol)) < 0) {
        return SERVER_SOCKET_ERROR;
    }

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        return SERVER_SETSOCKOPT_ERROR;
    }

    if (bind(server_sock, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen) == -1) {
        return SERVER_BIND_ERROR;
    }

    if (listen(server_sock, N_CONNECTIONS) < 0) {
        return SERVER_LISTEN_ERROR;
    }

    if (server_addrinfo != NULL) {
        freeaddrinfo(server_addrinfo);
    }

    return server_sock;
}

// Accepts connections using given options and server socket descriptor and returns status
int server_loop(struct Options *options, int server_sock) {
    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof(client_addr);

    while (1) {
        // Handle each new connection in separate process
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen);
        int pid;
        if ((pid = fork()) == 0) {
            close(server_sock);
            exit(client_execute(options, client_sock));
        }
        else if (pid < 0) {
            return FORK_ERROR;
        }
        close(client_sock);
    }

    return EXIT_SUCCESS;
}
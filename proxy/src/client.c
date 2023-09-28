#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netdb.h>
#include <resolv.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/wait.h>

#include <client.h>
#include <error.h>

// Size of receive buffer -- 16 Kb
#define BUF_SIZE 16 * 1024

// Starts connection between the client and the remote host using given options and client socket descriptor and return status
int client_execute(struct Options *options, int client_sock) {
    int remote_sock;
    if ((remote_sock = client_connect(options)) < 0) {
        close(remote_sock);
        close(client_sock);
        return remote_sock;
    }

    int pid;
    // Handle connection between client and remote host in different processes
    // One process per each direction, log only from client to remote host -- the queries
    if ((pid = fork()) == 0) {
        client_forward_data(client_sock, remote_sock, options->log_file);
        exit(EXIT_SUCCESS);
    }
    else if (pid < 0) {
        return FORK_ERROR;
    }

    if ((pid = fork()) == 0) {
        client_forward_data(remote_sock, client_sock, NULL);
        exit(0);
    }
    else if (pid < 0) {
        return FORK_ERROR;
    }

    close(remote_sock);
    close(client_sock);
    return EXIT_SUCCESS;
}

// Prepares connection between client and remote host using given options and returns either the socket descriptor or an error
int client_connect(struct Options *options) {
    struct addrinfo hints;
    struct addrinfo *remote_addrinfo = NULL;
    int remote_sock;
    char portstr[12];

    memset(&hints, 0, sizeof(hints));

    hints.ai_flags    = AI_NUMERICSERV | AI_NUMERICHOST;
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    sprintf(portstr, "%d", options->remote_port);

    if (getaddrinfo(options->remote_host, portstr, &hints, &remote_addrinfo) != 0) {
        return CLIENT_RESOLVE_ERROR;
    }

    if ((remote_sock = socket(remote_addrinfo->ai_family, remote_addrinfo->ai_socktype, remote_addrinfo->ai_protocol)) < 0) {
        return CLIENT_SOCKET_ERROR;
    }

    if (connect(remote_sock, remote_addrinfo->ai_addr, remote_addrinfo->ai_addrlen) < 0) {
        return CLIENT_CONNECT_ERROR;
    }

    if (remote_addrinfo != NULL) {
        freeaddrinfo(remote_addrinfo);
    }

    return remote_sock;
}

// Repeatedly forwards data between given sockets and writes logs, if FILE* in not NULL
int client_forward_data(int source_sock, int destination_sock, FILE *log_file) {
    ssize_t size;
    char buffer[BUF_SIZE];

    while ((size = recv(source_sock, buffer, BUF_SIZE, 0)) > 0) {
        if (log_file) {
            // Queries are started with Q letter, and the plain text starts from index 5
            if (buffer[0] == 'Q') {
                fprintf(log_file, "%s\n", buffer + 5);
            }
        }
        send(destination_sock, buffer, size, 0);
    }

    if (size < 0) {
        return BROKEN_PIPE_ERROR;
    }

    shutdown(destination_sock, SHUT_RDWR);
    close(destination_sock);

    shutdown(source_sock, SHUT_RDWR);
    close(source_sock);

    return EXIT_SUCCESS;
}
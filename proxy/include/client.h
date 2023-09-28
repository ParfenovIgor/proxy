#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED

#include <sys/socket.h>
#include <options.h>

int client_execute      (struct Options *options, int client_sock);
int client_connect      (struct Options *options);
int client_forward_data (int source_sock, int destination_sock, FILE *log_file);

#endif // CONNECTION_H_INCLUDED
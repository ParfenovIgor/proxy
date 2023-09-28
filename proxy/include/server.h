#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <options.h>

int server_execute      (struct Options *options);
int server_create_socket(struct Options *options);
int server_loop         (struct Options *options, int server_sock);

#endif // SERVER_H_INCLUDED
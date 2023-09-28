#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include <stdio.h>

struct Options {
    char *remote_host;
    int local_port;
    int remote_port;

    FILE *log_file;
};

#endif // OPTIONS_H_INCLUDED
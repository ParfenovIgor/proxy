#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <server.h>
#include <client.h>
#include <error.h>

// Prepares options structure using input arguments
struct Options *parse_options(int argc, char *argv[]) {
    struct Options *options = (struct Options*)malloc(sizeof(struct Options));
    memset(options, 0, sizeof(struct Options));
    
    int c;
    while ((c = getopt(argc, argv, "l:p:h:f:")) != -1) {
        switch(c) {
            case 'l':
                options->local_port = atoi(optarg);
                break;
            case 'p':
                options->remote_port = atoi(optarg);
                break;
            case 'h':
                options->remote_host = optarg;
                break;
            case 'f':
                const char *filename = optarg;
                options->log_file = fopen(filename, "a");
                if (!options->log_file) {
                    printf("Could not open file for writing\n");
                    exit(FILE_ERROR);
                }
                break;
        }
    }

    // Not all require options are specified
    if (!options->local_port || !options->remote_port || !options->remote_host) {
        printf("Syntax: %s -l local_port -h remote_host -p remote_port [-f log_file]\n", argv[0]);
        exit(SYNTAX_ERROR);
    }
    return options;
}

int main(int argc, char *argv[]) {
    return server_execute(parse_options(argc, argv));
}

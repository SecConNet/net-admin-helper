#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"

#include "wireguard.h"



void usage(const char * cmd) {
    fprintf(stderr, "Usage: %s <command> <arguments>\n\n", cmd);

    fprintf(stderr, "Available commands:\n");
    fprintf(stderr, "    %s", SYNOPSIS_WIREGUARD_CREATE);
    fprintf(stderr, "    %s", SYNOPSIS_WIREGUARD_ADD_PEER);
    fprintf(stderr, "\n");

    fprintf(stderr, "%s", USAGE_WIREGUARD_CREATE);
    fprintf(stderr, "%s", USAGE_WIREGUARD_ADD_PEER);
}


int main(int argc, char * argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    DISPATCH_WIREGUARD_CREATE(argv[1]);
    DISPATCH_WIREGUARD_ADD_PEER(argv[1]);

    fprintf(stderr, "Unknown command %s\n", argv[1]);
    return EXIT_FAILURE;
}


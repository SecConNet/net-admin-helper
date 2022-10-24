#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "config.h"

#include "capabilities.h"
#include "container_wireguard.h"


void usage(const char * cmd) {
    fprintf(stderr, "Usage: %s <command> <arguments>\n\n", cmd);

    fprintf(stderr, "Available commands:\n");
    fprintf(stderr, "    %s", SYNOPSIS_CWG_CREATE);
    fprintf(stderr, "    %s", SYNOPSIS_CWG_CONNECT);
    fprintf(stderr, "    %s", SYNOPSIS_CWG_DESTROY);
    fprintf(stderr, "\n");

    fprintf(stderr, "%s", USAGE_CWG_CREATE);
    fprintf(stderr, "%s", USAGE_CWG_CONNECT);
    fprintf(stderr, "%s", USAGE_CWG_DESTROY);
}


int main(int argc, char * argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Ensure private keys and the like don't get swapped out to a potentially
    // unencrypted swap partition.
    enable_cap(CAP_IPC_LOCK);
    mlockall(MCL_CURRENT | MCL_FUTURE);
    disable_cap(CAP_IPC_LOCK);

    DISPATCH_CWG_CREATE(argv[1]);
    DISPATCH_CWG_CONNECT(argv[1]);
    DISPATCH_CWG_DESTROY(argv[1]);

    fprintf(stderr, "Unknown command %s\n", argv[1]);
    return EXIT_FAILURE;
}


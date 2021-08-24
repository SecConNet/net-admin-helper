#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "subprocess.h"
#include "validation.h"
#include "wireguard.h"


#define WG_KEY_SIZE 44l


/** Validate the input for the wireguard_create command. */
static void create_validate(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of command line arguments\n");
        goto exit_usage;
    }

    if (validate_dev(argv[0], NULL)) {
        fprintf(stderr, "Invalid device name\n");
        goto exit_usage;
    }

    if (validate_network(argv[1], NULL)) {
        fprintf(stderr, "Invalid VPN IP/netmask\n");
        goto exit_usage;
    }

    if (validate_number(5, argv[2], NULL)) {
        fprintf(stderr, "Invalid listen port\n");
        goto exit_usage;
    }
    return;

exit_usage:
    fprintf(
            stderr, "Usage: %s wireguard_create <dev> <vpn_ip/netmask>"
            " <listen_port>\n", argv[0]);
    exit(EXIT_FAILURE);
}


int wireguard_create(int argc, char * argv[]) {
    const char * private_key = NULL, * public_key = NULL;
    ssize_t out_size = 0l;

    // get inputs
    create_validate(argc, argv);
    const char * dev = argv[0];
    const char * vpn_ip = argv[1];
    const char * port = argv[2];

    // create endpoint
    const char * const genkey_args[] = { WG, "genkey", NULL };
    if (run_check(WG, genkey_args, NULL, NULL, 0l, &private_key, &out_size))
        goto exit_0;

    const char * const pubkey_args[] = { WG, "pubkey", NULL };
    if (run_check(
                WG, pubkey_args, NULL, private_key, WG_KEY_SIZE,
                &public_key, &out_size))
        goto exit_1;

    const char * const create_dev_args[] = {
        IP, "link", "add", dev, "type", "wireguard", NULL };
    if (run_check2(IP, create_dev_args))
        goto exit_2;

    const char * const set_addr_args[] = {
        IP, "addr", "add", vpn_ip, "dev", dev, NULL };
    if (run_check2(IP, set_addr_args))
        goto exit_3;

    const char * const set_key_args[] = {
        WG, "set", dev, "listen-port", port, "private-key", "/dev/stdin",
        NULL };
    if (run_check(WG, set_key_args, NULL, private_key, WG_KEY_SIZE, NULL, NULL))
        goto exit_3;

    const char * const ifup_args[] = { IP, "link", "set", dev, "up", NULL };
    if (run_check2(IP, ifup_args))
        goto exit_3;

    // produce output
    printf("%s\n", public_key);

    // clean up
    free((void*)public_key);
    free((void*)private_key);

    return EXIT_SUCCESS;

exit_3:;
    const char * const destroy_dev_args[] = { IP, "link", "delete", dev, NULL };
    run_check2(IP, destroy_dev_args);

exit_2:
    free((void*)public_key);

exit_1:
    free((void*)private_key);

exit_0:
    return EXIT_FAILURE;
}


/** Validate the input for the wireguard_add_peer command. */
static void add_peer_validate(int argc, char * argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Incorrect number of command line arguments\n");
        goto exit_usage;
    }

    if (validate_dev(argv[0], NULL)) {
        fprintf(stderr, "Invalid device name\n");
        goto exit_usage;
    }

    if (validate_endpoint(argv[1], NULL)) {
        fprintf(stderr, "Invalid endpoint\n");
        goto exit_usage;
    }

    if (validate_network(argv[2], NULL)) {
        fprintf(stderr, "Invalid VPN IP/netmask\n");
        goto exit_usage;
    }

    if (validate_wireguard_key(argv[3], NULL)) {
        fprintf(stderr, "Invalid key\n");
        goto exit_usage;
    }
    return;

exit_usage:
    fprintf(
            stderr, "Usage: %s wireguard_add_peer <dev> <endpoint>"
            "  <vpn_ip/netmask> <key>\n", argv[0]);
    exit(EXIT_FAILURE);
}


int wireguard_add_peer(int argc, char * argv[]) {
    // get inputs
    add_peer_validate(argc, argv);
    const char * dev = argv[0];
    const char * endpoint = argv[1];
    const char * vpn_ip_nm = argv[2];
    const char * peer_key = argv[3];

    // add peer
    const char * const add_args[] = {
        WG, "set", dev, "peer", peer_key, "allowed-ips", vpn_ip_nm, "endpoint",
        endpoint, NULL };

    if (run_check2(WG, add_args))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


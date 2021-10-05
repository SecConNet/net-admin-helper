#include <fcntl.h>
#include <inttypes.h>
#include <sched.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "capabilities.h"
#include "subprocess.h"
#include "validation.h"

#include "config.h"
#include "container_wireguard.h"


#define WG_KEY_SIZE 44l


/** Validate the pid, net and host inputs each command has.
 *
 * This assumes that there are at least three arguments, do check that first.
 **/
static void cwg_validate_pid_net_host(char * argv[]) {
    int value = -1;

    if (validate_number(7, argv[0], NULL)) {
        fprintf(stderr, "Invalid network namespace PID\n");
        goto exit_usage;
    }

    if (validate_number(7, argv[1], NULL)) {
        fprintf(stderr, "Invalid network number\n");
        goto exit_usage;
    }

    value = atoi(argv[0]);

    if ((value < 0) || ((1 << 23) <= value)) {
        fprintf(stderr, "Network number out of range [0, %d]\n", (1 << 23) - 1);
        goto exit_usage;
    }

    if (validate_number(1, argv[2], NULL)) {
        fprintf(stderr, "Invalid host number\n");
        goto exit_usage;
    }

    value = atoi(argv[2]);

    if ((value < 0) || (1 < value)) {
        fprintf(stderr, "Host number must be 0 or 1\n");
        goto exit_usage;
    }

    return;

exit_usage:
    fprintf(stderr, "Usage: " SYNOPSIS_CWG_CREATE);
    exit(EXIT_FAILURE);
}


/** Compute the device name from net and host.
 *
 * Caller owns the returned buffer and needs to free() it. Returns NULL on
 * error.
 */
static const char * cwg_device_name(char * argv[]) {
    int net = atoi(argv[1]);
    int host = atoi(argv[2]);

    const size_t dev_buf_size = strlen(CWG_PREFIX) + 11;
    char * dev = (char *)malloc(dev_buf_size);
    if (!dev) return NULL;
    snprintf(dev, dev_buf_size, "%s-%u-%u", CWG_PREFIX, net, host);
    return dev;
}


/** Compute network IP. */
static uint32_t cwg_network_ip(char * argv[]) {
    int net = atoi(argv[1]);

    return (10u << 24) | (net << 1);
}


/** Compute host IP. */
static uint32_t cwg_host_ip(char * argv[]) {
    int host = atoi(argv[2]);

    return cwg_network_ip(argv) | host;
}


/** Compute network IP/netmask and return as dotted-quad/nm.
 *
 * Caller owns the returned buffer and needs to free() it. Returns NULL on
 * error.
 */
static const char * cwg_network_ip_nm(char * argv[]) {
    uint32_t ip = cwg_network_ip(argv);
    char * ips = (char *)malloc(19);
    if (!ips) return NULL;
    snprintf(
            ips, 19, "%"PRIu32".%"PRIu32".%"PRIu32".%"PRIu32"/31",
            ip >> 24, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    return ips;
}


/** Compute device IP address and return as dotted-quad.
 *
 * Caller owns the returned buffer and needs to free() it. Returns NULL on
 * error.
 */
static const char * cwg_device_ip(char * argv[]) {
    uint32_t ip = cwg_host_ip(argv);
    char * ips = (char *)malloc(16);
    if (!ips) return NULL;
    snprintf(
            ips, 16, "%"PRIu32".%"PRIu32".%"PRIu32".%"PRIu32,
            ip >> 24, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    return ips;
}


/** Switch to the target ns for the device.
 *
 * Returns 0 on success, 1 on failure.
 *
 * */
static int cwg_set_ns(const char * netns_pid) {


    char netns_path[32];
    snprintf(netns_path, 32, "/proc/%s/ns/net", netns_pid);

    enable_cap(CAP_SYS_PTRACE);
    int netns_fd = open(netns_path, O_RDONLY | O_NONBLOCK);
    disable_cap(CAP_SYS_PTRACE);

    if (netns_fd == -1) {
        fprintf(stderr, "When opening %s\n", netns_path);
        perror("Could not open network namespace\n");
        goto exit_fail;
    }

    enable_cap(CAP_SYS_ADMIN);
    int err = setns(netns_fd, CLONE_NEWNET);
    disable_cap(CAP_SYS_ADMIN);

    if (err) {
        fprintf(stderr, "Could not enter namespace\n");
        perror(NULL);
        goto exit_fd;
    }

    close(netns_fd);
    return 0;

exit_fd:
    close(netns_fd);

exit_fail:
    return 1;
}


/** Validate the input for the cwg_create command. */
static void cwg_create_validate(int argc, char * argv[]) {
    int value = -1;

    if (argc != 4) {
        fprintf(stderr, "Incorrect number of command line arguments\n");
        goto exit_usage;
    }

    cwg_validate_pid_net_host(argv);

    if (validate_number(5, argv[3], NULL)) {
        fprintf(stderr, "Invalid listen port\n");
        goto exit_usage;
    }

    value = atoi(argv[3]);

    if ((value < 1) || (65536 <= value)) {
        fprintf(stderr, "Port number of out of range [1, 65535]\n");
        goto exit_usage;
    }
    return;

exit_usage:
    fprintf(stderr, "Usage: " SYNOPSIS_CWG_CREATE);
    exit(EXIT_FAILURE);
}


int cwg_create(int argc, char * argv[]) {
    const char * private_key = NULL, * public_key = NULL;
    ssize_t public_key_size = 0l, private_key_size = 0l;

    // get inputs
    cwg_create_validate(argc, argv);

    const char * netns_pid = argv[0];

    const char * dev = cwg_device_name(argv);
    if (!dev) goto exit_fail;

    const char * ips = cwg_device_ip(argv);
    if (!ips) goto exit_dev;

    const char * vpn_ip_nm = cwg_network_ip_nm(argv);
    if (!vpn_ip_nm) goto exit_ips;

    const char * port = argv[3];

    // create endpoint
    const char * const genkey_args[] = { WG, "genkey", NULL };
    if (run_check(
                WG, genkey_args, NULL, NULL, 0l,
                &private_key, &private_key_size)) {
        fprintf(stderr, "Error generating private key\n");
        goto exit_vpn_ip_nm;
    }

    const char * const pubkey_args[] = { WG, "pubkey", NULL };
    if (run_check(
                WG, pubkey_args, NULL, private_key, WG_KEY_SIZE,
                &public_key, &public_key_size)) {
        fprintf(stderr, "Error calculating public key\n");
        goto exit_private_key;
    }

    const char * const create_dev_args[] = {
        IP, "link", "add", dev, "type", "wireguard", NULL };
    if (run_check2(IP, create_dev_args)) {
        fprintf(stderr, "Error creating device\n");
        goto exit_public_key;
    }

    const char * const move_dev_args[] = {
        IP, "link", "set", dev, "netns", netns_pid, NULL };
    if (run_check2(IP, move_dev_args)) {
        fprintf(stderr, "Error moving device into namespace\n");
        goto exit_if;
    }

    if (cwg_set_ns(netns_pid))
        goto exit_if;

    const char * const set_addr_args[] = {
        IP, "addr", "add", ips, "dev", dev, NULL };
    if (run_check2(IP, set_addr_args)) {
        fprintf(stderr, "Error setting IP address\n");
        goto exit_if;
    }

    const char * const set_key_args[] = {
        WG, "set", dev, "listen-port", port, "private-key", "/dev/stdin",
        NULL };
    if (run_check(
                WG, set_key_args, NULL, private_key, WG_KEY_SIZE,
                NULL, NULL)) {
        fprintf(stderr, "Error setting port and key\n");
        goto exit_if;
    }

    const char * const ifup_args[] = { IP, "link", "set", dev, "up", NULL };
    if (run_check2(IP, ifup_args)) {
        fprintf(stderr, "Error bringing up interface\n");
        goto exit_if;
    }

    const char * const route_args[] = {
        IP, "route", "add", vpn_ip_nm, "dev", dev, NULL };
    if (run_check2(IP, route_args)) {
        fprintf(stderr, "Error adding route\n");
        goto exit_if;
    }

    // produce output
    printf("%s\n", public_key);

    // clean up
    explicit_bzero((void*)public_key, public_key_size);
    free((void*)public_key);

    explicit_bzero((void*)private_key, private_key_size);
    free((void*)private_key);

    free((void*)vpn_ip_nm);
    free((void*)ips);
    free((void*)dev);

    return EXIT_SUCCESS;

exit_if:;
    const char * const destroy_dev_args[] = { IP, "link", "delete", dev, NULL };
    run_check2(IP, destroy_dev_args);

exit_public_key:
    explicit_bzero((void*)public_key, public_key_size);
    free((void*)public_key);

exit_private_key:
    explicit_bzero((void*)private_key, private_key_size);
    free((void*)private_key);

exit_vpn_ip_nm:
    free((void*)vpn_ip_nm);

exit_ips:
    free((void*)ips);

exit_dev:
    free((void*)dev);

exit_fail:
    return EXIT_FAILURE;
}


/** Validate the input for the cwg_connect command. */
static void cwg_connect_validate(int argc, char * argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Incorrect number of command line arguments\n");
        goto exit_usage;
    }

    cwg_validate_pid_net_host(argv);

    if (validate_endpoint(argv[3], NULL)) {
        fprintf(stderr, "Invalid endpoint\n");
        goto exit_usage;
    }

    if (validate_wireguard_key(argv[4], NULL)) {
        fprintf(stderr, "Invalid key\n");
        goto exit_usage;
    }
    return;

exit_usage:
    fprintf(stderr, "Usage: " SYNOPSIS_CWG_CONNECT);
    exit(EXIT_FAILURE);
}


int cwg_connect(int argc, char * argv[]) {
    // get inputs
    cwg_connect_validate(argc, argv);

    const char * netns_pid = argv[0];

    const char * dev = cwg_device_name(argv);
    if (!dev) goto exit_fail;

    const char * vpn_ip_nm = cwg_network_ip_nm(argv);
    if (!vpn_ip_nm) goto exit_dev;

    const char * peer_endpoint = argv[3];
    const char * peer_key = argv[4];

    // add peer
    if (cwg_set_ns(netns_pid))
        goto exit_ip;

    const char * const add_args[] = {
        WG, "set", dev, "peer", peer_key, "allowed-ips", vpn_ip_nm, "endpoint",
        peer_endpoint, NULL };

    if (run_check2(WG, add_args))
        goto exit_ip;

    return EXIT_SUCCESS;

exit_ip:
    free((void*)vpn_ip_nm);

exit_dev:
    free((void*)dev);

exit_fail:
    return EXIT_FAILURE;
}


int cwg_destroy(int argc, char * argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of command line arguments\n");
        goto exit_usage;
    }
    cwg_validate_pid_net_host(argv);

    const char * netns_pid = argv[0];
    cwg_set_ns(netns_pid);

    const char * dev = cwg_device_name(argv);
    const char * const delete_args[] = { IP, "link", "delete", dev, NULL };
    int err = run_check2(IP, delete_args);

    free((void*)dev);
    if (err) return EXIT_FAILURE;
    return EXIT_SUCCESS;

exit_usage:
    fprintf(stderr, "Usage: " SYNOPSIS_CWG_DESTROY);
    return EXIT_FAILURE;
}


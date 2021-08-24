#pragma once

#include "config.h"
#include "dispatch.h"


#ifdef ENABLE_WIREGUARD_CREATE

#define SYNOPSIS_WIREGUARD_CREATE \
    "wireguard_create <dev> <vpn_ip/nm> <port>\n"

#define USAGE_WIREGUARD_CREATE \
    "--------------------------------------------------------------------\n"\
    "\n"                                                                    \
    "NAME\n"                                                                \
    "    wireguard_create - Creates a WireGuard interface.\n\n"             \
    "SYNOPSIS\n"                                                            \
    "    wireguard_create <dev> <vpn_ip> <listen_ip>\n\n"                   \
    "ARGUMENTS:\n"                                                          \
    "    dev: Name of the interface device to create.\n"                    \
    "    vpn_ip/netmask: Our IPv4 address within the VPN in dotted-quad\n"  \
    "            notation followed by a slash and the netmask.\n"           \
    "    port: The local IP port to listen on for incoming connections.\n\n"\
    "EXECUTED COMMANDS:\n"                                                  \
    "    wg genkey\n"                                                       \
    "    wg pubkey\n"                                                       \
    "    ip link add <dev> type wireguard\n"                                \
    "    ip addr add <vpn_ip> dev <dev>\n"                                  \
    "    wg set wg0 listen-port <port> private-key <generated key>\n"       \
    "    ip link set <dev> up\n\n"                                          \
    "OUTPUT:\n"                                                             \
    "    The public key for the new interface will be printed on standard\n"\
    "    output. In case of failure, an error will be printed on standard\n"\
    "    error.\n\n"                                                        \
    "EXIT CODE:\n"                                                          \
    "    0 on success, 1 on failure.\n\n"


#define DISPATCH_WIREGUARD_CREATE(CMD) \
    DISPATCH(wireguard_create, CMD)


/** Create a WireGuard tunnel endpoint.
 *
 * Command line arguments:
 *
 * `dev` Name of the network device to create (e.g. wg0).
 * `vpn_ip/netmask` Our IPv4 address in the VPN followed by a slash and the
 *      netmask of the VPN subnet (e.g. 10.0.33.1/24).
 * `port` The local IP port to listen on for incoming connections.
 *
 * This will run
 *
 *     wg genkey
 *     wg pubkey
 *     ip link add <dev> type wireguard
 *     ip addr add <vpn_ip/netmask> dev <dev>
 *     wg set wg0 listen-port <port> private-key <generated key>
 *     ip link set <dev> up
 *
 * It will then print the generated public key required to connect to this
 * endpoint, followed by a newline.
 *
 * @return The exit code to exit with.
 */
int wireguard_create(int argc, char * argv[]);

#else

#define SYNOPSIS_WIREGUARD_CREATE ""
#define USAGE_WIREGUARD_CREATE ""
#define DISPATCH_WIREGUARD_CREATE(CMD)

#endif


#ifdef ENABLE_WIREGUARD_ADD_PEER

#define SYNOPSIS_WIREGUARD_ADD_PEER \
    "wireguard_add_peer <dev> <endpoint> <vpn_network> <public_key>\n"

#define USAGE_WIREGUARD_ADD_PEER \
    "--------------------------------------------------------------------\n"\
    "\n"                                                                    \
    "NAME\n"                                                                \
    "    wireguard_add_peer - Adds a peer to a WireGuard interface.\n\n"    \
    "SYNOPSIS\n"                                                            \
    "    wireguard_add_peer <dev> <endpoint> <vpn_network> <public_key>\n\n"\
    "ARGUMENTS:\n"                                                          \
    "    dev: Name of the interface device to add to.\n"                    \
    "    endpoint: IPv4 endpoint of the peer, in dotted-quad\n"             \
    "            notation followed by a colon and the port number.\n"       \
    "    vpn_network: The IPv4 address range of the VPN in dotted-quad\n"   \
    "            notation followed by a slash and the netmask.\n"           \
    "    key: The peer's public key.\n\n"                                   \
    "EXECUTED COMMANDS\n"                                                   \
    "    wg set <dev> peer <key> allowed-ips <vpn_network> endpoint <endpoint>\n\n" \
    "OUTPUT:\n"                                                             \
    "    None on success, an error message on stderr in case of\n"          \
    "    failure.\n\n"                                                      \
    "EXIT CODE:\n"                                                          \
    "    0 on success, 1 on failure.\n\n"


#define DISPATCH_WIREGUARD_ADD_PEER(CMD) \
        DISPATCH(wireguard_add_peer, CMD)


/** Add a peer to a WireGuard interface.
 *
 * Command line arguments:
 *
 * `dev` Name of the network device to modify (e.g. wg0).
 * `endpoint` ip:port to connect to for this peer.
 * `vpn_ip/netmask` The peer's IPv4 address in the VPN followed by a slash and
 *      the netmask of the VPN subnet (e.g. 10.0.33.1/24).
 * `key` The public key of the peer.
 *
 * This will run
 *
 *     wg set <dev> peer <key> allowed-ips <vpn_ip/nm> endpoint <endpoint>
 *
 * @return The exit code to exit with.
 */
int wireguard_add_peer(int argc, char * argv[]);

#else

#define SYNOPSIS_WIREGUARD_ADD_PEER ""
#define USAGE_WIREGUARD_ADD_PEER ""
#define DISPATCH_WIREGUARD_ADD_PEER(CMD)

#endif


#pragma once

#include "config.h"
#include "dispatch.h"


#ifdef ENABLE_CWG_CREATE

#define SYNOPSIS_CWG_CREATE "cwg_create <pid> <net> <host> <port>\n"

#define USAGE_CWG_CREATE \
    "--------------------------------------------------------------------\n"\
    "\n"                                                                    \
    "NAME\n"                                                                \
    "    cwg_create - Creates a WireGuard interface.\n\n"                   \
    "SYNOPSIS\n"                                                            \
    "    " SYNOPSIS_CWG_CREATE "\n"                                         \
    "ARGUMENTS:\n"                                                          \
    "    pid: PID of the network namespace to put the device into.\n"       \
    "    net: Number of the network to use, in [0, 8388607].\n"             \
    "    host: Number of this host on that network, 0 or 1.\n"              \
    "    port: The local IP port to listen on for incoming connections.\n\n"\
    "OUTPUT:\n"                                                             \
    "    The public key for the new interface will be printed on standard\n"\
    "    output. In case of failure, an error will be printed on standard\n"\
    "    error.\n\n"                                                        \
    "EXIT CODE:\n"                                                          \
    "    0 on success, 1 on failure.\n\n"


#define DISPATCH_CWG_CREATE(CMD) DISPATCH(cwg_create, CMD)

int cwg_create(int argc, char * argv[]);

#else

#define SYNOPSIS_CWG_CREATE ""
#define USAGE_CWG_CREATE ""
#define DISPATCH_CWG_CREATE(CMD)

#endif


#ifdef ENABLE_CWG_CONNECT

#define SYNOPSIS_CWG_CONNECT \
    "cwg_connect <pid> <net> <host> <peer_endpoint> <peer_key>\n"

#define USAGE_CWG_CONNECT \
    "--------------------------------------------------------------------\n"\
    "\n"                                                                    \
    "NAME\n"                                                                \
    "    cwg_connect - Connect to the other host on this network.\n\n"      \
    "SYNOPSIS\n"                                                            \
    "    " SYNOPSIS_CWG_CONNECT "\n"                                        \
    "ARGUMENTS:\n"                                                          \
    "    pid: PID of the network namespace the device is in.\n"             \
    "    net: Network of the local interface device to connect.\n"          \
    "    host: Host of the local interface device to connect.\n"            \
    "    peer_endpoint: IPv4 endpoint of the peer, in dotted-quad\n"        \
    "            notation followed by a colon and the port number.\n"       \
    "    peer_key: The peer's public key.\n\n"                              \
    "OUTPUT:\n"                                                             \
    "    None on success, an error message on stderr in case of\n"          \
    "    failure.\n\n"                                                      \
    "EXIT CODE:\n"                                                          \
    "    0 on success, 1 on failure.\n\n"


#define DISPATCH_CWG_CONNECT(CMD) \
        DISPATCH(cwg_connect, CMD)

int cwg_connect(int argc, char * argv[]);

#else

#define SYNOPSIS_CWG_CONNECT ""
#define USAGE_CWG_CONNECT ""
#define DISPATCH_CWG_CONNECT(CMD)

#endif


#ifdef ENABLE_CWG_DESTROY

#define SYNOPSIS_CWG_DESTROY "cwg_destroy <pid> <net> <host>\n"

#define USAGE_CWG_DESTROY \
    "--------------------------------------------------------------------\n"\
    "\n"                                                                    \
    "NAME\n"                                                                \
    "    cwg_destroy - Remove a network interface.\n\n"                     \
    "SYNOPSIS\n"                                                            \
    "    " SYNOPSIS_CWG_DESTROY "\n"                                        \
    "ARGUMENTS:\n"                                                          \
    "    pid: PID of the network namespace the device is in.\n"             \
    "    net: Network of the network device to remove.\n"                   \
    "    host: Host of the network device to remove.\n"                     \
    "OUTPUT:\n"                                                             \
    "    None on success, an error message on stderr in case of\n"          \
    "    failure.\n\n"                                                      \
    "EXIT CODE:\n"                                                          \
    "    0 on success, 1 on failure.\n\n"

#define DISPATCH_CWG_DESTROY(CMD) DISPATCH(cwg_destroy, CMD)

int cwg_destroy(int argc, char * argv[]);

#else

#define SYNOPSIS_CWG_DESTROY ""
#define USAGE_CWG_DESTROY ""
#define DISPATCH_CWG_DESTROY(CMD)

#endif


#define DISPATCH_CWG_CREATE_NS(CMD) DISPATCH(cwg_create_ns, CMD)

int cwg_create_ns(int arcg, char * argv[]);


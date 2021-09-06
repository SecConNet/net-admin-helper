# Container-to-container WireGuard VPN

This scenario covers the case of connecting containers on two or more hosts
using a WireGuard point-to-point link. It has three tasks: creating a WireGuard
interface/endpoint, adding a peer endpoint to a WireGuard interface, and
removing an interface.

Conceptually, this assumes that there is a set of available VPN networks, on
each of which there are two hosts. Networks and hosts are identified by a single
number each.

A virtual network device may be created inside a given network namespace for a
given network and host using `cwg_create`, with the corresponding WireGuard
socket listening on a given port on the local physical machine and the physical
network. The public key for the device will be returned, so that it can be
communicated to the other physical machine that will take part in the VPN. The
other physical machine then needs to create a virtual network device as well.

To connect a virtual network device to a virtual network device on another
physical machine, that physical machine needs to send us its physical IP
address, the port number of the socket corresponding to the virtual network
device on its side, and the public key. These can then be passed to
`cwg_connect` to make the connection.

Finally, a device can be removed using `cwg_destroy`. Note that it will be
removed automatically when the container exits, so explicitly deleting it may
not be necessary.

Assuming two physical machines at different sites that want to set up a
point-to-point connection between two containers, one at each site, and some way
of exchanging messages between them, the connection can be set up by the
following sequence of events:

- Site 1 picks a network and host, and allocates a local port.
- Site 1 creates a virtual network device inside its local container.
- Site 1 sends network, host, its public IP address, port, and public key to
  Site 2.
- Site 2 allocates a port.
- Site 2 creates a virtual network device inside its local container, with the
  same network and the other host in that network, using the allocated port.
- Site 2 connects its virtual network device to the one at Site 1.
- Site 2 sends its IP address, port, and public key to Site 1.
- Site 1 connects its virtual network device to the one at Site 2.
- The containers communicate.


## Tasks

### Creating a virtual network device

`cwg_create <pid> <net> <host> <port>`

This creates a new WireGuard network device inside a given namespace. The device
will be on the given virtual network, with the corresponding socket listening on
a given port in the current namespace. The device will be named
`<prefix>-<net>-<host>` (see Configuration below for more on the prefix).

Arguments:

`pid`: The pid of the network namespace to create the device inside of.

`net`: The number of the network to use, in the range [0, 8388607]. This number
designates a `/31` subnetwork of the IPv4 class-A 10.0.0.0 private network
space. Network number 0 represents 10.0.0.0/31, network 1 is 10.0.0.2/31,
network 1024 is 10.0.8.0/31 and so on. Each network has two host addresses, for
hosts 0 and 1.

`host`: The number of the host, which is `0` on one side of the link, and `1` on
the other.

`port`: The local port to listen on, in the range [0-65535].


Return value:

The public key for this device will be printed on standard output. This needs to
be combined with the physical IP address of the current host and the port
number passed in, and sent to the other side together with the network and host
numbers to use for it to connect to this endpoint.

Exit code:

0 for success, 1 for failure. In case of error, an error message will be printed
on standard error.


### Connecting peers

`cwg_connect <pid> <net> <host> <peer_endpoint> <peer_key>`

Connects a local network device to its remote peer.

Arguments:

`pid`: The pid of the network namespace the device is in.

`net`: The number of the network to use. Both sides must use the same number for
their virtual network devices.

`host`: The host number that the remote virtual network device was created with.

`peer_endpoint`: IP-address:port of the remote endpoint to connect to. This is
the IP address of the remote physical machine and the port its virtual network
device is listening on.

`peer_key`: The public key of the peer virtual network device.

Return value:

None.

Exit code:

0 for success, 1 for failure. In case of error, an error message will be printed
on standard error.


### Removing a device

`cwg_destroy <pid> <net> <host>`

Removes the device for the given network and host.

Argements:

`pid`: The pid of the network namespace the device is in.

`net`: The number of the network the device is in.

`host`: The host number of the device to remove.

Return value:

None.

Exit code:

0 for success, 1 for failure. In case of error, an error message will be printed
on standard error.


## Configuration

The following settings may be changed in `config.h`:

`CWG_PREFIX`: A string prefix for the names of the network devices managed by
`net-admin-helper`. For example `wg` or `myapp`. The device names will look like
`<prefix>-<net>-<host>`. The compiled binary will not be able to modify any
network devices that do not conform to this naming scheme, which is a security
enhancement.

`ENABLE_CWG_CREATE`, `ENABLE_CWG_CONNECT`, and `ENABLE_CWG_DELETE` enable the
corresponding functions.


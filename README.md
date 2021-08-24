# net-admin-helper

A helper program for network administration tasks that require privileged
operations.

Network administration tasks such as configuring the firewall, modifying routing
tables, or setting up VPN connections require special privileges, either full
root (administrator) rights or a specific capability (CAP_NET_ADMIN on Linux).

If these tasks need to be done by an application that also does many other
things (e.g. drawing a GUI, handling network requests) then that entire
application needs elevated privileges, at least initially. This is problematic
in general because it increases the attack surface, and it's practically
impossible if the application is written in an interpreted language such as
Python or Java.

A better solution then is to create a separate program which implements exactly
the network administration tasks needed by the application, can be installed
with CAP_NET_ADMIN enabled, and can then be called from the unprivileged
process. Thus, if an attacker were to breach the application then they will be
able to perform the given network administration tasks (which is inevitable) but
nothing else.

net-admin-helper is that program. It can do a number of different network
administration tasks, and is easy to extend with new ones. It is compiled with
only the required tasks enabled, then given CAP_NET_ADMIN privileges and called
from the application. Its inputs and outputs are designed for easy communication
with the calling application. net-admin-helper validates all its inputs and is
paranoid about checking for and handling errors in everything it does.

net-admin-helper relies on standard utilities like `ip` and `wg` to perform its
tasks, rather than talking directly to the kernel. This reduces the size of the
program and makes it easier to implement new tasks given a sequence of shell
commands.


## Supported operating systems

- GNU/Linux with Linux kernel >=4.3


## Configuration

net-admin-helper is configured by editing (or overwriting) the file `config.h`.
In this file, individual tasks can be enabled, and the paths to the helper
programs are configured. (net-admin-helper does not search the PATH, for reasons
of security). See the file itself for detailed instructions.

Also see `docs/` for some scenarios and which tasks are needed to support them.


## Building and installing

Because net-admin-helper is capabilities-aware, it needs to link to the `libcap`
library. Installing the `libcap-dev` or `libcap-devel` package should make it
available.

Once `libcap` is available, net-admin-helper can be built by running `make`:

```bash
net-admin-helper$ make
```

This will produce an executable in `bin/`. Next, this executable needs to be
given CAP_NET_ADMIN capability:

```bash
net-admin-helper$ sudo make setcap
```

Note that this requires root access.

You can now run the executable where it is, or install it together with your
application.

Most tasks supported by net-admin-helper require the `ip` command, which is
available in the `iproute2` package on most GNU/Linux distributions. If you want
to run any tasks involving WireGuard then you'll need to install the
`wireguard-tools` package as well to make the `wg` command available.


## Using from an application

All you need to be able to do is to run an external command, passing it command
line arguments and obtaining the exit code and standard output and standard
error streams. For example in Python:

```python
import subprocess


result = subprocess.run(
        ['net-admin-helper', 'move_to_ns', 'tun0', str(pid)],
        encoding='utf-8', capture_output=True)

if result.returncode != 0:
    error_message = result.stderr
    # log error
    # raise or otherwise handle

output = result.stdout
# use output, if applicable
```


## Extending with new tasks

TODO


## Authors

- Lourens Veen (Netherlands eScience Center)


## Legal

net-admin-helper is Copyright 2021 Netherlands eScience Center and
University of Amsterdam.

Licensed under the Apache License 2.0.


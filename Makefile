base_objects = bin/main.o bin/capabilities.o bin/subprocess.o bin/validation.o
task_objects = bin/container_wireguard.o

objects = $(base_objects) $(task_objects)

# _DEFAULT_SOURCE is needed for explicit_bzero()
CFLAGS=-g -std=c11 -D_GNU_SOURCE -Wall -Wextra -pedantic -O0 -I. -Isrc
LDFLAGS=-lcap


.PHONY: all
all: bin/net-admin-helper


.PHONY: setcap
setcap: bin/net-admin-helper
	# Ensure normal users cannot modify the binary
	chown root:root bin/net-admin-helper
	chmod 755 bin/net-admin-helper
	# Give it the needed capabilities
	setcap 'cap_net_admin,cap_sys_ptrace,cap_sys_admin,cap_ipc_lock=p' bin/net-admin-helper


export DOCKER_BUILDKIT = 1

.PHONY: docker
docker:
	docker build . -t net-admin-helper:latest
	docker save net-admin-helper:latest >bin/net-admin-helper.tar


.PHONY: clean
clean:
	-rm -f bin/*
	-docker rmi net-admin-helper:latest


bin/main.o: config.h src/container_wireguard.h
bin/capabilities.o: src/capabilities.h
bin/subprocess.o: src/capabilities.h src/subprocess.h
bin/validation.o: src/validation.h

bin/container_wireguard.o: config.h src/container_wireguard.h src/dispatch.h src/subprocess.h src/validation.h

bin/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

bin/net-admin-helper: $(objects)
	cc -o bin/net-admin-helper $(objects) $(LDFLAGS)


base_objects = bin/main.o bin/capabilities.o bin/subprocess.o bin/validation.o
task_objects = bin/wireguard.o

objects = $(base_objects) $(task_objects)

CFLAGS=-g -std=c11 -Wall -Wextra -pedantic -O0 -I. -Isrc
LDFLAGS=-lcap


.PHONY: all
all: bin/net-admin-helper


.PHONY: setcap
setcap: bin/net-admin-helper
	# Ensure normal users cannot modify the binary
	chown root:root bin/net-admin-helper
	chmod 755 bin/net-admin-helper
	# Give it CAP_NET_ADMIN
	setcap cap_net_admin=p bin/net-admin-helper


.PHONY: clean
clean:
	rm -f bin/*


bin/main.o: config.h src/wireguard.h
bin/capabilities.o: src/capabilities.h
bin/subprocess.o: src/capabilities.h src/subprocess.h
bin/validation.o: src/validation.h
bin/wireguard.o: src/wireguard.h src/dispatch.h src/subprocess.h src/validation.h

bin/%.o: src/%.c
	$(CC) -c $< $(CFLAGS) -o $@

bin/net-admin-helper: $(objects)
	cc -o bin/net-admin-helper $(objects) $(LDFLAGS)


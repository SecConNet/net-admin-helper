FROM ubuntu:20.04

RUN \
    apt-get update && \
    apt-get install -y gcc make iproute2 wireguard-tools libcap2 libcap2-bin libcap-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

RUN \
    mkdir /usr/local/src/net-admin-helper && \
    mkdir /usr/local/src/net-admin-helper/bin

COPY Makefile /usr/local/src/net-admin-helper/Makefile
COPY config.h /usr/local/src/net-admin-helper/config.h
COPY src /usr/local/src/net-admin-helper/src

RUN \
    cd /usr/local/src/net-admin-helper && \
    make && make setcap && \
    mv /usr/local/src/net-admin-helper/bin/net-admin-helper /usr/local/bin/net-admin-helper

RUN rm -r /usr/local/src/net-admin-helper

RUN apt-get remove -y gcc make libcap-dev && apt-get -y autoremove

USER nobody


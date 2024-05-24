
FROM ubuntu:22.04
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake-curses-gui \
    vim \
    gdb \
    sudo

COPY ./bind_this /opt/bind_this

RUN cmake \
    -S "/opt/bind_this" \
    -B "/opt/bind_this/build" \
    -D CMAKE_INSTALL_PREFIX=/usr/local && \
    cmake --build "/opt/bind_this/build" && \
    cmake --install "/opt/bind_this/build" && \
    ldconfig && \
    chmod -R 777 /opt/bind_this && \
    rm -r /opt/bind_this/build

    
RUN echo "ZGV2Q1RGezEK" | base64 -d > /root/root_part1.txt && \
    echo "bnQzcnAwemkK" | base64 -d > /root/root_part2.txt && \
    echo "dGlvbl9jNG4K" | base64 -d > /root/root_part3.txt && \
    echo "X2JfdzFsZH0K" | base64 -d > /root/root_part4.txt
    
RUN useradd -m -s /bin/bash binder && \
echo "binder ALL=(ALL) NOPASSWD: /usr/local/bin/bind_this"  > /etc/sudoers.d/custom

USER binder
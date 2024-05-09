
FROM ubuntu:22.04 AS build_env
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake-curses-gui \
    vim \
    libboost-test-dev \
    sudo

COPY ./privacyplz /opt/privacyplz-src

RUN cmake \
    -S "/opt/privacyplz-src" \
    -B "/opt/privacyplz-src/build" \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/opt/privacyplz && \
    cmake --build "/opt/privacyplz-src/build" && \
    cmake --install "/opt/privacyplz-src/build" && \
    chmod -R 775 "/opt" && \
    chmod -R 777 "/opt/privacyplz/include"

RUN echo "devCTF{pr1vacy_m0dz_a1nt_c0mp1led}" > /root/root.txt

COPY ./privacy_h4ck3r /opt/privacy_h4ck3r

RUN useradd -m -s /bin/bash priv && \
    echo "priv ALL=(ALL) NOPASSWD: /opt/privacy_h4ck3r/build_and_run.sh"  > /etc/sudoers.d/custom

USER priv
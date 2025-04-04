FROM ubuntu:24.04
ARG USERNAME=ubuntu

# Install dependencies
RUN apt update \
    && apt upgrade -y \
    && DEBIAN_FRONTEND=noninteractive apt install -y cmake ninja-build \
        llvm-15 clang-15 lld-15 git lsb-release curl sudo clang-format \
        vim wget inotify-tools npm libnss3-dev libatk1.0-0 \
        libatk-bridge2.0-0 libxdamage1 libxkbcommon-x11-0 libpangocairo-1.0-0 \
        libcups2 libasound2-dev pandoc

# Build the artifact directory
RUN mkdir artifact \
    && chown $USERNAME artifact

# switch to non-root user to build artifact
USER $USERNAME
WORKDIR /artifact

# ========================
# Installation finished
# ========================

# Build FrankenScript
RUN git clone https://github.com/fxpl/frankenscript.git \
    && ln -s ./frankenscript/examples/ ./examples \
    && cd frankenscript \
    && git checkout f62016b \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-15 -DCMAKE_CXX_COMPILER=clang++-15 .. \
    && ninja

# Copy instructions into the container
COPY README.md README.md
COPY fs-html.bash fs-html.bash
COPY fs.bash fs.bash
COPY all-examples-html.bash all-examples-html.bash
COPY all-examples.bash all-examples.bash
COPY all-figures-html.bash all-figures-html.bash
COPY all-figures.bash all-figures.bash
COPY watch.bash watch.bash
COPY res/head.txt res/head.txt
COPY res/foot.txt res/foot.txt
COPY figures/ figures/

# Start bash for user input
ENV SHELL=/bin/bash
CMD ["/bin/bash"]

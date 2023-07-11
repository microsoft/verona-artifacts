FROM ubuntu:22.04
ARG USERNAME=verona

# Install dependencies
RUN apt update \
    && apt upgrade -y \
    && DEBIAN_FRONTEND=noninteractive apt install -y cmake ninja-build \
        llvm-12 clang-12 lld-12 git lsb-release curl sudo pip cloc \
        linux-tools-5.15.0-76 libc6-dbg 

# Install Python dependencies
RUN pip install psutil plotly pandas kaleido

# Create a non-root user
RUN useradd -m $USERNAME \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Build the artifact directory
RUN mkdir artifact \
    && chown $USERNAME artifact

# Installation finished - switch to non-root user to build artifact
USER $USERNAME
WORKDIR artifact

# Build Verona Runtime benchmark
RUN git clone https://github.com/Microsoft/verona-rt.git \
    && cd verona-rt \
    # Note use Commit hash to avoid malleablity
    && git checkout 19c01dff5e6abc60d8c56998c66fbfa8f2f9be81 \
    && mkdir build \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-12 \
            -DCMAKE_CXX_COMPILER=clang++-12 .. \
    && ninja test/perf-con-dining_phil

# Build Verona Savina benchmarks
RUN git clone https://github.com/ic-slurp/verona-benchmarks.git \
    && cd verona-benchmarks \
    && mkdir build \
    # Note use Commit hash to avoid malleablity
    && git checkout ba849ca2cbe714b1bf79b39f3b8c80384682a326 \
    && cd build \
    && cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang-12 -DCMAKE_CXX_COMPILER=clang++-12 .. \
    && ninja

# Install PonyUp
RUN export SHELL=/bin/bash \
    && sh -c "$(curl --proto '=https' --tlsv1.2 -sSf https://raw.githubusercontent.com/ponylang/ponyup/latest-release/ponyup-init.sh)"

# Install Pony
RUN $HOME/.local/share/ponyup/bin/ponyup update ponyc release-0.53.0

# Build Pony Savina
RUN export CC=/usr/bin/clang-12 \
    && git clone https://github.com/sblessing/pony-savina.git \
    && cd pony-savina \ 
    # Note use Commit hash to avoid malleablity
    && git checkout f53a3c608538dcaa7ec3ce6dbf425b15e37a3d21 \ 
    && cd savina-pony \
    && $HOME/.local/share/ponyup/bin/ponyc

# Put the instructions into the container.
COPY README.md README.md

# Put the existing results into the container.
COPY results results

ENV SHELL /bin/bash
CMD /bin/bash

FROM ubuntu:24.04

# Install apt dependencies
ARG DEBIAN_FRONTEND=noninteractive
ARG NODE_MAJOR=20
RUN apt-get update -qq \
    && apt-get install -y \
    curl \
    cmake \
    ccache \
    libsdl2-dev \
    g++ \
    gdb \
    git \
    libpng-dev \
    ninja-build \
    sudo \
    python3-pip \
    python3-venv \
    && rm -rf /var/cache/apt/* /var/lib/apt/lists/*

# Install nodejs based dependencies
RUN curl -sL https://deb.nodesource.com/setup_${NODE_MAJOR}.x -o nodesource_setup.sh \
    && bash nodesource_setup.sh \
    && apt-get update -qq \
    && apt-get install -y nodejs \
    && npm install -g \
    lv_font_conv@1.5.2 \
    && rm nodesource_setup.sh \
    && rm -rf /var/cache/apt/* /var/lib/apt/lists/*

# Install Python dependencies
RUN pip install --break-system-packages \
    wheel \
    Pillow \
    && pip cache purge

# The user ubuntu already exists set its password to ubuntu and add to sudo group for developing in devcontainer
RUN usermod -aG sudo ubuntu \
    && echo "ubuntu:ubuntu" | chpasswd 

# Persist bash history across container rebuilds
# Reference: https://code.visualstudio.com/remote/advancedcontainers/persist-bash-history
ARG USERNAME=ubuntu
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc"

USER ubuntu

# Section for interactive compilation during docker run

WORKDIR /sources
# Directory of InfiniTime source code
ENV INFINITIME_DIR="/sources/InfiniTime"
# Passed to CMake generate step
ENV GENERATE_ARGS=""
# Passed to CMake build step
ENV BUILD_ARGS=""
# Build directory
ENV BUILD_DIRECTORY="build"

CMD ["bash", "-c", "cmake -S . -B ${BUILD_DIRECTORY} -G Ninja -DInfiniTime_DIR=${InfiniTime_DIR} ${GENERATE_ARGS} \
    && cmake --build ${BUILD_DIRECTORY} ${BUILD_ARGS}"]

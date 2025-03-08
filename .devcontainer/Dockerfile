FROM ubuntu:22.04

# Install dependencies
ARG DEBIAN_FRONTEND=noninteractive
ARG NODE_MAJOR=20
RUN apt-get update -qq && \
    apt-get install -y \
    curl \
    cmake \
    ccache \
    libsdl2-dev \
    g++ \
    git \
    libpng-dev \
    ninja-build \
    sudo \
    python3-pip \
    python3-venv \
    && rm -rf /var/cache/apt/* /var/lib/apt/lists/* \
    && curl -sL https://deb.nodesource.com/setup_${NODE_MAJOR}.x -o nodesource_setup.sh \
    && bash nodesource_setup.sh \
    && apt-get install -y nodejs \
    && npm install -g lv_font_conv@1.5.2 \
    && pip install wheel Pillow

# Add the infinitime user with sudo password "it" for developing in devcontainer
RUN adduser infinitime && echo "infinitime:it" | chpasswd && usermod -aG sudo infinitime

# Persist bash history across container rebuilds
# Reference: https://code.visualstudio.com/remote/advancedcontainers/persist-bash-history
ARG USERNAME=infinitime
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc"

USER infinitime

# Section for interactive compilation during docker run

WORKDIR /sources
# Directory if InfiniTime source code
ENV INFITIME_DIR="/sources/InfiniTime"
# Passed to CMake generate step
ENV GENERATE_ARGS=""
# Passed to CMake build step
ENV BUILD_ARGS=""
# Build directory
ENV BUILD_DIRECTORY="build"

CMD ["bash", "-c", "cmake -S . -B ${BUILD_DIRECTORY} -G Ninja -DInfiniTime_DIR=${INFITIME_DIR} ${GENERATE_ARGS} && cmake --build ${BUILD_DIRECTORY} ${BUILD_ARGS}"]

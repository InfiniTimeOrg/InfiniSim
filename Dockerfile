FROM ubuntu:22.04

# Install dependencies
ARG DEBIAN_FRONTEND=noninteractive
ARG NODE_MAJOR=20
RUN apt-get update -qq && \
    apt-get install -y \
    curl \
    cmake \
    libsdl2-dev \
    g++ \
    libpng-dev \
    python3-pip \
    python3-venv \
    && rm -rf /var/cache/apt/* /var/lib/apt/lists/* \
    && curl -sL https://deb.nodesource.com/setup_${NODE_MAJOR}.x -o nodesource_setup.sh \
    && bash nodesource_setup.sh \
    && apt-get install -y nodejs \
    && npm install -g lv_font_conv@1.5.2 \
    && pip install wheel Pillow

WORKDIR /sources
# Build configuration environment variables
ENV INFITIME_DIR="/sources/InfiniTime"
ENV BUILD_FLAGS=""

CMD ["bash", "-c", "cmake -S . -B build -DInfiniTime_DIR=${INFITIME_DIR} ${BUILD_FLAGS} && cmake --build build -j4"]

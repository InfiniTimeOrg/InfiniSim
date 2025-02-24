FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y \
    curl \
    cmake \
    libsdl2-dev \
    g++ \
    python3-pip \
    python3-venv \
    libpng-dev \
    git \
    && rm -rf /var/lib/apt/lists/* \
    && curl -sL https://deb.nodesource.com/setup_20.x -o nodesource_setup.sh \
    && bash nodesource_setup.sh \
    && apt-get install -y nodejs \
    && npm install -g lv_font_conv@1.5.2 \
    && pip install wheel Pillow


WORKDIR /sources

# Define build configuration environment variables with default values
ENV INFITIME_DIR="/sources/InfiniTime"
ENV BUILD_FLAGS=""

CMD ["bash", "-c", "cmake -S . -B build -DInfiniTime_DIR=${INFITIME_DIR} ${BUILD_FLAGS} && cmake --build build -j4"]

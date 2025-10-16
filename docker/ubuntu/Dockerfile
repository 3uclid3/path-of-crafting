FROM ubuntu:24.04

ARG GCC_VERSION=14
ARG LLVM_VERSION=21
ARG REFRESH_TS=0

LABEL org.opencontainers.image.created=${REFRESH_TS}

ENV DEBIAN_FRONTEND=noninteractive
# Prefer Ninja so CMake/xmake don't whine about "CMAKE_MAKE_PROGRAM"
ENV CMAKE_GENERATOR=Ninja
# Let xmake run as root in CI
ENV XMAKE_ROOT=y

# Prefer Clang toolchain by default for builds
ENV CC=clang \
    CXX=clang++ \
    AR=llvm-ar \
    NM=llvm-nm \
    RANLIB=llvm-ranlib

# Make pipx drop shims in a PATH dir that survives CI shells
ENV PIPX_HOME=/opt/pipx \
    PIPX_BIN_DIR=/usr/local/bin

# Base deps + compilers + missing build tools + pkg-config
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    wget \
    gnupg \
    curl \
    # archive utils
    7zip \
    zip unzip tar bzip2 xz-utils \
    # VCS + TLS
    git \
    ca-certificates \
    # compilers & build tools
    gcc-${GCC_VERSION} g++-${GCC_VERSION} \
    libstdc++-${GCC_VERSION}-dev libstdc++6 \
    cmake \
    xmake \
    ccache \
    ninja-build \
    make \
    pkg-config \
    # coverage helpers (gcovr/lcov are fine to have even if you use llvm-cov)
    gcovr \
    lcov \
    # python + pipx (+ venv is REQUIRED for pipx on Debian/Ubuntu)
    python3 python3-pip python3-venv pipx \
    # SDL3 typical native deps (headers) so xmake/SDL don’t build everything from source
    libx11-dev libxext-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev libxfixes-dev \
    libxkbcommon-dev libwayland-dev \
    libudev-dev \
    libdrm-dev libgbm-dev \
    libgl1-mesa-dev mesa-common-dev libegl1-mesa-dev libgles2-mesa-dev \
    libasound2-dev libpulse-dev \
    libdbus-1-dev \
    # prefer system expat to avoid xmake trying to cmake-build it
    libexpat1-dev \
 && rm -rf /var/lib/apt/lists/*

# Make sure python is always available as both python3 and python
RUN ln -sf /usr/bin/python3 /usr/bin/python

# Install coverage tools with pipx into /usr/local/bin
RUN pipx install coverage \
 && pipx install codecov-cli \
 && pipx install lcov-cobertura \
 && python3 -V \
 && which coverage && coverage --version \
 && which codecov-cli && codecov-cli --version \
 && (which lcov_cobertura || which lcov-cobertura || python3 -c "import lcov_cobertura") \
 && (lcov_cobertura --help >/dev/null 2>&1 || true) \
 && (lcov-cobertura --help >/dev/null 2>&1 || true)

# LLVM install script (21)
RUN wget -q https://apt.llvm.org/llvm.sh -O /tmp/llvm.sh \
 && chmod +x /tmp/llvm.sh \
 && /tmp/llvm.sh ${LLVM_VERSION} all \
 && rm -f /tmp/llvm.sh

# Alternatives for GCC/Clang/LLVM tools
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} 100 \
 && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-${GCC_VERSION} 100 \
 && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-ar llvm-ar /usr/bin/llvm-ar-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-nm llvm-nm /usr/bin/llvm-nm-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-ranlib llvm-ranlib /usr/bin/llvm-ranlib-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-objdump llvm-objdump /usr/bin/llvm-objdump-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-objcopy llvm-objcopy /usr/bin/llvm-objcopy-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-size llvm-size /usr/bin/llvm-size-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-profdata llvm-profdata /usr/bin/llvm-profdata-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-cxxfilt llvm-cxxfilt /usr/bin/llvm-cxxfilt-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-strip llvm-strip /usr/bin/llvm-strip-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-as llvm-as /usr/bin/llvm-as-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-link llvm-link /usr/bin/llvm-link-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-symbolizer llvm-symbolizer /usr/bin/llvm-symbolizer-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-tblgen llvm-tblgen /usr/bin/llvm-tblgen-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-extract llvm-extract /usr/bin/llvm-extract-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-addr2line llvm-addr2line /usr/bin/llvm-addr2line-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-dwarfdump llvm-dwarfdump /usr/bin/llvm-dwarfdump-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-mc llvm-mc /usr/bin/llvm-mc-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llvm-dis llvm-dis /usr/bin/llvm-dis-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/lld lld /usr/bin/lld-${LLVM_VERSION} 100 \
 && update-alternatives --install /usr/bin/llc llc /usr/bin/llc-${LLVM_VERSION} 100
 
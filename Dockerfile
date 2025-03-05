    # Base image with dependencies (for reuse across builds)
    FROM ubuntu:22.04 AS base

    # Enable universe repo, update package lists, and install dependencies
    RUN apt update \
        && apt install -y software-properties-common \
        && add-apt-repository universe \
        && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
        && apt update \
        && apt install -y g++-13 cmake make ninja-build git libssl-dev \
        && rm -rf /var/lib/apt/lists/*  # Clean up to reduce image size

    # Set C and C++ compilers explicitly
    ENV CC=gcc-13
    ENV CXX=g++-13

    # --- Stage 1: Build Poco Separately ---
    FROM base AS poco-builder

    WORKDIR /Dependencies
    COPY Dependencies /Dependencies

    # Debugging step to ensure the files are copied
    RUN ls -la /Dependencies

    # Remove any previous cache files before building
    RUN rm -rf CMAKE_BUILD && mkdir CMAKE_BUILD

    # Run cmake from /Dependencies
    RUN cmake -B CMAKE_BUILD -DCMAKE_BUILD_TYPE=Release -S . 
    RUN cmake --build CMAKE_BUILD --config Release --target Poco

    # --- Stage 2: Build SimilarStrings Using Prebuilt Poco ---
    FROM base AS similar-strings-builder

    COPY . .

    RUN rm -rf CMAKE_BUILD && mkdir CMAKE_BUILD

    COPY --from=poco-builder /Dependencies/CMAKE_BUILD/install Dependencies/CMAKE_BUILD/install
    COPY --from=poco-builder /Dependencies/CMAKE_BUILD/Poco Dependencies/CMAKE_BUILD/Poco

    ENV PocoFoundation_DIR=/Dependencies/CMAKE_BUILD/install/Release/lib/cmake/Poco
    ENV CMAKE_PREFIX_PATH=/Dependencies/CMAKE_BUILD/install/Release/lib/cmake/Poco

    RUN ls -lah /Dependencies/CMAKE_BUILD/install/Release/lib/

    RUN cmake -B CMAKE_BUILD -DCMAKE_BUILD_TYPE=Release -DPoco_DIR=/Dependencies/CMAKE_BUILD/install/Release/lib/cmake/Poco

    RUN cmake --build CMAKE_BUILD --config Release

    # --- Stage 3: Create Final Runtime Image ---
    FROM ubuntu:22.04

    # Ensure universe repository is enabled in the final runtime image
    RUN apt update \
        && apt install -y software-properties-common \
        && add-apt-repository universe \
        && apt update \
        && add-apt-repository ppa:ubuntu-toolchain-r/test \
        && apt update \
        && apt install -y libstdc++6 \
        && rm -rf /var/lib/apt/lists/*  # Clean up

    WORKDIR /app
    COPY --from=similar-strings-builder build /app
    COPY --from=poco-builder /Dependencies/CMAKE_BUILD/install/Release/lib /usr/local/lib
    COPY words_clean.txt words_clean.txt

    RUN ldconfig

    EXPOSE 8000

    CMD ["/app/SimilarStrings", "/app/words_clean.txt"]


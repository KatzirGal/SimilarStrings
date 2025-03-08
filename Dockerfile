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

# Define build arguments
ARG BUILD_TYPE=Release
ARG PRODUCTION=ON
ENV PRODUCTION=$PRODUCTION

RUN echo "Build type is $BUILD_TYPE"
RUN echo "PRODUCTION is set to: $PRODUCTION"

# If NOT in production, install debugging tools
RUN if [ "$PRODUCTION" = "OFF" ]; then \
        apt-get update && apt-get install -y gdb; \
    fi

# --- Stage 1: Build Poco Separately ---
FROM base AS poco-builder

WORKDIR /Dependencies
COPY Dependencies /Dependencies

# Debugging step to ensure the files are copied
RUN ls -la /Dependencies

# Remove any previous cache files before building
RUN rm -rf CMAKE_BUILD && mkdir CMAKE_BUILD

# Use the build argument in the CMake command
RUN cmake -B CMAKE_BUILD -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -S . 
RUN cmake --build CMAKE_BUILD --config ${BUILD_TYPE} --target Poco

# --- Stage 2: Build SimilarStrings Using Prebuilt Poco ---
FROM base AS similar-strings-builder

ARG BUILD_TYPE=Release
ARG PRODUCTION=ON
ENV PRODUCTION=$PRODUCTION

RUN echo "Build type is $BUILD_TYPE"
RUN echo "PRODUCTION is set to: $PRODUCTION"

COPY . .

RUN rm -rf CMAKE_BUILD && mkdir CMAKE_BUILD

COPY --from=poco-builder /Dependencies/CMAKE_BUILD/install Dependencies/CMAKE_BUILD/install
COPY --from=poco-builder /Dependencies/CMAKE_BUILD/Poco Dependencies/CMAKE_BUILD/Poco

ENV PocoFoundation_DIR=/Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib/cmake/Poco
ENV CMAKE_PREFIX_PATH=/Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib/cmake/Poco

RUN ls -lah /Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib/

# Adjust compilation flags for debugging if needed
RUN if [ "$PRODUCTION" = "OFF" ]; then \
        cmake -B CMAKE_BUILD -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_CXX_FLAGS="-g -O2 -no-pie" -DPoco_DIR=/Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib/cmake/Poco; \
    else \
        cmake -B CMAKE_BUILD -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DPoco_DIR=/Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib/cmake/Poco; \
    fi

RUN cmake --build CMAKE_BUILD --config ${BUILD_TYPE}

# --- Stage 3: Create Final Runtime Image ---
FROM ubuntu:22.04

ARG BUILD_TYPE=Release
ARG PRODUCTION=ON
ENV PRODUCTION=$PRODUCTION

RUN echo "Build type is $BUILD_TYPE"
RUN echo "PRODUCTION is set to: $PRODUCTION"

# Ensure universe repository is enabled in the final runtime image
RUN apt update \
    && apt install -y software-properties-common \
    && add-apt-repository universe \
    && apt update \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt update \
    && apt install -y libstdc++6 \
    && rm -rf /var/lib/apt/lists/*  # Clean up

# Install gdb if debugging is enabled
RUN if [ "$PRODUCTION" = "OFF" ]; then \
        apt-get update && apt-get install -y gdbserver; \
        apt-get update && apt-get install -y gdb; \
    fi

WORKDIR /app
COPY --from=similar-strings-builder build /app
COPY --from=poco-builder /Dependencies/CMAKE_BUILD/install/${BUILD_TYPE}/lib /usr/local/lib
COPY words_clean.txt words_clean.txt
COPY /src /src

RUN ldconfig

EXPOSE 8000
EXPOSE 1234

# Ensure execution permissions
RUN chmod +x /app/SimilarStrings

# Default command for production, override in debug mode
# CMD ["/app/SimilarStrings", "/app/words_clean.txt"]
# Start application normally in production, but use gdbserver in debug mode
# CMD if [ "$PRODUCTION" = "OFF" ]; then \
#         gdbserver :1234 /app/SimilarStrings /app/words_clean.txt; \
#     else \
#         /app/SimilarStrings /app/words_clean.txt; \
#     fi

CMD ["/bin/bash", "-c", "echo 'PRODUCTION at runtime is:' $PRODUCTION; pkill -f gdbserver; if [[ \"$PRODUCTION\" == \"OFF\" ]]; then gdbserver 0.0.0.0:1234 /app/SimilarStrings /app/words_clean.txt & fi; exec /app/SimilarStrings /app/words_clean.txt"]


# CMD ["/bin/sh", "-c", "if [ \"$PRODUCTION\" = \"OFF\" ]; then exec gdbserver 0.0.0.0:1234 /app/SimilarStrings /app/words_clean.txt; else exec /app/SimilarStrings /app/words_clean.txt; fi"]

# CMD ["gdbserver", ":1234", "/app/SimilarStrings", "/app/words_clean.txt"]



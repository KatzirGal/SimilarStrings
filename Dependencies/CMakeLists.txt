cmake_minimum_required(VERSION 3.20)
project(Dependencies)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Restrict configurations to Debug, Release, and RelWithDebInfo
if(MSVC OR CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Set build type for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type: Debug, Release, or RelWithDebInfo" FORCE)
endif()

include(ExternalProject)

# Set Poco installation directory
set(POCO_INSTALL_DIR ${CMAKE_BINARY_DIR}/install)

# --- Build Poco Separately ---
ExternalProject_Add(
    Poco
    GIT_REPOSITORY https://github.com/pocoproject/poco.git
    GIT_TAG main
    PREFIX ${CMAKE_BINARY_DIR}/Poco
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install/${CMAKE_BUILD_TYPE}
        -DCMAKE_BUILD_RPATH=${CMAKE_BINARY_DIR}/install/${CMAKE_BUILD_TYPE}/lib
        -DCMAKE_INSTALL_RPATH=${CMAKE_BINARY_DIR}/install/${CMAKE_BUILD_TYPE}/lib
        -DCMAKE_INSTALL_RPATH_USE_LINK_PATH=ON
        -DPOCO_STATIC=OFF
        -DBUILD_SHARED_LIBS=ON
        -DPOCO_UNBUNDLED=OFF
        -DENABLE_NET=ON
        $<$<NOT:$<PLATFORM_ID:Windows>>:-DENABLE_NETSSL=ON>
        $<$<PLATFORM_ID:Windows>:-DENABLE_NETSSL=OFF>
        -DENABLE_NETHTTP=ON
        -DENABLE_WEBSOCKET=ON
        -DENABLE_UTIL=ON
        -DENABLE_JSON=ON
        -DENABLE_XML=ON
        -DENABLE_FOUNDATION=ON
        -DENABLE_CPPPARSER=OFF
        -DENABLE_TESTS=OFF
        -DENABLE_SAMPLES=OFF
)

message(STATUS "Poco will be installed in: ${POCO_INSTALL_DIR}")

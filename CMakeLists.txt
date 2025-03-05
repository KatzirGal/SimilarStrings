cmake_minimum_required(VERSION 3.20)
project(SimilarStrings LANGUAGES CXX)

# For single-configuration generators (Makefiles, Ninja, etc.)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === SEPARATE BUILD OUTPUT FROM CMAKE FILES ===
if (WIN32)
    set(BUILD_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/x64")
else()
    set(BUILD_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")
endif()

# Set output directories for all configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_OUTPUT_DIR}")

# Ensure multi-config generators (Visual Studio, Xcode) respect this setting
foreach(OUTPUTCONFIG Debug Release RelWithDebInfo MinSizeRel)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${BUILD_OUTPUT_DIR}/${OUTPUTCONFIG}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${BUILD_OUTPUT_DIR}/${OUTPUTCONFIG}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${BUILD_OUTPUT_DIR}/${OUTPUTCONFIG}")
endforeach()


# Restrict configurations to Debug, Release, and RelWithDebInfo
if(MSVC OR CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Set build type for single-config generators
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose build type: Debug, Release, or RelWithDebInfo" FORCE)
endif()

# Include file_list.cmake
include(${CMAKE_SOURCE_DIR}/file_list.cmake)
if(UNIX)
    include("${CMAKE_SOURCE_DIR}/Dependencies/CMAKE_BUILD/install/${CMAKE_BUILD_TYPE}/lib/cmake/Poco/PocoNetSSLConfig.cmake")
endif()

set(POCO_INSTALL_DIR ${CMAKE_SOURCE_DIR}/Dependencies/CMAKE_BUILD/install)

# Tell CMake to search Poco in this directory
set(CMAKE_PREFIX_PATH ${POCO_INSTALL_DIR})

find_package(Poco REQUIRED Foundation Net Util JSON XML PATHS ${POCO_INSTALL_DIR} NO_DEFAULT_PATH)

if(NOT Poco_FOUND)
    message(FATAL_ERROR "Poco not found in ${POCO_INSTALL_DIR}. Build it first using Dependencies.")
endif()

# --- Define Poco Libraries ---
macro(add_poco_library LIB_NAME LIB_FILE)
    if(WIN32)
        set_target_properties(Poco::${LIB_NAME} PROPERTIES
            IMPORTED_IMPLIB_DEBUG ${POCO_INSTALL_DIR}/Debug/lib/Poco${LIB_FILE}d.lib
            IMPORTED_IMPLIB_RELEASE ${POCO_INSTALL_DIR}/Release/lib/Poco${LIB_FILE}.lib
            IMPORTED_LOCATION_DEBUG ${POCO_INSTALL_DIR}/Debug/bin/Poco${LIB_FILE}d.dll
            IMPORTED_LOCATION_RELEASE ${POCO_INSTALL_DIR}/Release/bin/Poco${LIB_FILE}.dll
            IMPORTED_LOCATION_RELWITHDEBINFO ${POCO_INSTALL_DIR}/RelWithDebInfo/bin/Poco${LIB_FILE}.dll
            IMPORTED_IMPLIB_RELWITHDEBINFO ${POCO_INSTALL_DIR}/RelWithDebInfo/lib/Poco${LIB_FILE}.lib
        )
    else()
        set_target_properties(Poco::${LIB_NAME} PROPERTIES
            IMPORTED_LOCATION ${POCO_INSTALL_DIR}/${CMAKE_BUILD_TYPE}/lib/libPoco${LIB_FILE}.so
        )
    endif()
endmacro()


add_poco_library(Foundation Foundation)
add_poco_library(Net Net)
if(UNIX)
    add_poco_library(NetSSL NetSSL)
endif()
add_poco_library(Util Util)
add_poco_library(JSON JSON)
add_poco_library(XML XML)

# --- Main Project ---
add_executable(SimilarStrings ${SOURCE_FILES} ${HEADER_FILES})

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(TARGET SimilarStrings PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_SOURCE_DIR}/words_clean.txt")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SimilarStrings)
endif()


# Include Poco headers
target_include_directories(SimilarStrings PRIVATE
    ${POCO_INSTALL_DIR}/${CMAKE_BUILD_TYPE}/include
)

# Link Poco Libraries
target_link_libraries(SimilarStrings 
    Poco::Foundation
    Poco::Net
    $<$<PLATFORM_ID:Unix>:Poco::NetSSL>
    Poco::Util
    Poco::JSON
    Poco::XML
)

add_custom_command(TARGET SimilarStrings POST_BUILD
    # === Foundation ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoFoundationd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoFoundation.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoFoundation.so>
    $<TARGET_FILE_DIR:SimilarStrings>

    # === Net ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoNetd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoNet.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoNet.so>
    $<TARGET_FILE_DIR:SimilarStrings>

    # === Util ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoUtild.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoUtil.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoUtil.so>
    $<TARGET_FILE_DIR:SimilarStrings>

    # === JSON ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoJSONd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoJSON.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoJSON.so>
    $<TARGET_FILE_DIR:SimilarStrings>

    # === XML ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoXMLd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoXML.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoXML.so>
    $<TARGET_FILE_DIR:SimilarStrings>
)

if(UNIX)
    add_custom_command(TARGET SimilarStrings POST_BUILD
        # === NetSSL ===
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoNetSSL.so
            $<TARGET_FILE_DIR:SimilarStrings>
    )
endif()

# --- Test Project ---
add_subdirectory(Test)

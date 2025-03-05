cmake_minimum_required(VERSION 3.20)
project(MyTest LANGUAGES CXX)

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

find_package(Poco REQUIRED Foundation Net Util JSON XML PATHS ${POCO_INSTALL_DIR} NO_DEFAULT_PATH)

if(NOT Poco_FOUND)
    message(FATAL_ERROR "Poco not found in ${POCO_INSTALL_DIR}. Build it first using Dependencies.")
endif()

add_executable(MyTest main.cpp)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(TARGET SimilarStrings PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${CMAKE_SOURCE_DIR}/words_clean.txt")

else()
    add_custom_target(MyTarget
        COMMAND SimilarString "\"${CMAKE_SOURCE_DIR}/words_clean.txt\""
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()



# Use ${CMAKE_CFG_INTDIR} to select the correct include directory
target_include_directories(MyTest PRIVATE
    ${POCO_INSTALL_DIR}/$<CONFIG>/include
)

# Link Poco Libraries
target_link_libraries(MyTest
    Poco::Foundation
    Poco::Net
    $<$<PLATFORM_ID:Unix>:Poco::NetSSL>
    Poco::Util
    Poco::JSON
    Poco::XML
)

add_custom_command(TARGET MyTest POST_BUILD
    # === Foundation ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoFoundationd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoFoundation.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoFoundation.so>
    $<TARGET_FILE_DIR:MyTest>

    # === Net ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoNetd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoNet.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoNet.so>
    $<TARGET_FILE_DIR:MyTest>

    # === Util ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoUtild.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoUtil.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoUtil.so>
    $<TARGET_FILE_DIR:MyTest>

    # === JSON ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoJSONd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoJSON.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoJSON.so>
    $<TARGET_FILE_DIR:MyTest>

    # === XML ===
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:${POCO_INSTALL_DIR}/Debug/bin/PocoXMLd.dll>
    $<$<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug>>>:${POCO_INSTALL_DIR}/$<CONFIG>/bin/PocoXML.dll>
    $<$<NOT:$<PLATFORM_ID:Windows>>:${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoXML.so>
    $<TARGET_FILE_DIR:MyTest>
)

if(UNIX)
    add_custom_command(TARGET MyTest POST_BUILD
        # === NetSSL ===
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${POCO_INSTALL_DIR}/$<CONFIG>/lib/libPocoNetSSL.so
            $<TARGET_FILE_DIR:MyTest>
    )
endif()

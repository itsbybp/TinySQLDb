cmake_minimum_required(VERSION 3.15)
project(tinyserver VERSION 0.1 LANGUAGES CXX)

option(TINYDB_ENABLE_SSL "Enable SSL (requires OpenSSL)" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source discovery
set(SERVER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    "${SERVER_SRC_DIR}/*.cpp"
    "${SERVER_SRC_DIR}/*.cxx"
)

list(LENGTH SERVER_SOURCES SERVER_SOURCES_COUNT)
if (SERVER_SOURCES_COUNT EQUAL 0)
    message(FATAL_ERROR "No server sources found under ${SERVER_SRC_DIR}. Add your .cpp files there.")
endif()

# Include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# Create executable
add_executable(tinyserver ${SERVER_SOURCES})

# Put binaries in predictable folders per-configuration
set_target_properties(tinyserver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/Release"
)

# Link OpenSSL if requested and available
if (TINYDB_ENABLE_SSL)
    find_package(OpenSSL QUIET)
    if (OpenSSL_FOUND)
        message(STATUS "OpenSSL found, enabling SSL support.")
        target_compile_definitions(tinyserver PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)
        target_include_directories(tinyserver PRIVATE ${OPENSSL_INCLUDE_DIR})
        target_link_libraries(tinyserver PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    else()
        message(WARNING "TINYDB_ENABLE_SSL is ON but OpenSSL was not found. SSL support will be disabled.")
    endif()
endif()

# Compiler warnings and options
if (MSVC)
    target_compile_options(tinyserver PRIVATE /W4 /permissive- /Zc:__cplusplus /utf-8)
else()
    target_compile_options(tinyserver PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-parameter)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(tinyserver PRIVATE -Wno-psabi)
    endif()
endif()

# Helpful status info
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Source dir: ${SERVER_SRC_DIR}")
message(STATUS "Discovered ${SERVER_SOURCES_COUNT} server source file(s)")
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(canfestival VERSION 3.0.0 LANGUAGES C CXX)

# --- Project Configuration Options ---

set(CANFESTIVAL_DEFAULT_BUILD_TYPE "Release"
    CACHE STRING "default build type used when CMAKE_BUILD_TYPE is not specified")

option(CANOPEN_BIG_ENDIAN "" ON)
set(MAX_CAN_BUS_ID "" CACHE STRING "" FORCE)
set(SDO_MAX_LENGTH_TRANSFER "" CACHE STRING "" FORCE)
set(SDO_BLOCK_SIZE "" CACHE STRING "" FORCE)
set(SDO_MAX_SIMULTANEOUS_TRANSFERS "1" CACHE STRING "" FORCE)
set(NMT_MAX_NODE_ID "2" CACHE STRING "" FORCE)
set(SDO_TIMEOUT_MS "" CACHE STRING "" FORCE)
set(MAX_NB_TIMER "" CACHE STRING "" FORCE)
set(US_TO_TIMEVAL_FACTOR "" CACHE STRING "" FORCE)
set(TIMEVAL "" CACHE STRING "" FORCE)
set(TIMEVAL_MAX "" CACHE STRING "" FORCE)
set(RTCAN_SOCKET "" CACHE STRING "" FORCE)
set(EMCY_MAX_ERRORS "3" CACHE STRING "" FORCE)
set(LSS_TIMEOUT_MS "" CACHE STRING "" FORCE)
set(LSS_FS_TIMEOUT_MS "" CACHE STRING "" FORCE)

option(SDO_DYNAMIC_BUFFER_ALLOCATION "" OFF)
set(SDO_DYNAMIC_BUFFER_ALLOCATION_SIZE "" CACHE STRING "" FORCE)

# Always enable generation of compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE BOOL "Generate compile commands database for clang tooling"
    FORCE)

# --- Inclusions ---
# Extra finders
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/find)
# Extra functions
include(cmake/repeat_string.cmake)
include(cmake/ensure_build_type.cmake)

# --- Build Type ---
# Make sure to set CMAKE_BUILD_TYPE when none has been specified by the user.
ensure_build_type(${CANFESTIVAL_DEFAULT_BUILD_TYPE})

# WIP: config.h still under development
# Configure header
foreach(macro
        SDO_MAX_SIMULTANEOUS_TRANSFERS
        NMT_MAX_NODE_ID
        EMCY_MAX_ERRORS)
  repeat_string("repeat" "${${macro}}" " " result)
  set(${macro}_REPEAT_STRING "${result}" CACHE STRING "" FORCE)
endforeach()

configure_file(include/config.h.in ${CMAKE_BINARY_DIR}/include/config.h)

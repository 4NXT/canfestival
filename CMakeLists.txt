cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(canfestival VERSION 3.0.0 LANGUAGES C CXX)

# --- Inclusions ---

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/find)
include(cmake/repeat_string.cmake)
include(cmake/ensure_build_type.cmake)

# --- Build configuration ---

set(CANFESTIVAL_DEFAULT_BUILD_TYPE "Release"
    CACHE STRING "default build type used when CMAKE_BUILD_TYPE is not specified")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON
    CACHE BOOL "Generate compile commands database for clang tooling"
    FORCE)

# --- Target platform configuration ---

set(CANFESTIVAL_SUPPORTED_PLATFORMS unix win32 hcs12 none)
set(CANFESTIVAL_TARGET_PLATFORM "unix" CACHE STRING "") # TODO: detect default platform
set_property(CACHE CANFESTIVAL_TARGET_PLATFORM PROPERTY STRINGS ${CANFESTIVAL_SUPPORTED_PLATFORMS})

# --- Timers driver configuration ---

set(CANFESTIVAL_SUPPORTED_TIMERS unix xeno rtai kernel kernel_xeno)
set(CANFESTIVAL_TIMERS "unix" CACHE STRING "") # TODO: detect default timers
set_property(CACHE CANFESTIVAL_TIMERS PROPERTY STRINGS ${CANFESTIVAL_SUPPORTED_TIMERS})

# --- Canfestival configuration ---

option(CANOPEN_BIG_ENDIAN "" OFF)
option(CO_ENABLE_LSS "" OFF)
option(CO_ENABLE_LSS_FS "" OFF)

set(MAX_CAN_BUS_ID "1" CACHE STRING "" FORCE)
set(SDO_MAX_LENGTH_TRANSFER "32" CACHE STRING "" FORCE)
set(SDO_BLOCK_SIZE "16" CACHE STRING "" FORCE)
set(SDO_MAX_SIMULTANEOUS_TRANSFERS "1" CACHE STRING "" FORCE)
set(NMT_MAX_NODE_ID "128" CACHE STRING "" FORCE)
set(SDO_TIMEOUT_MS "3000" CACHE STRING "" FORCE)
set(MAX_NB_TIMER "32" CACHE STRING "" FORCE)
set(US_TO_TIMEVAL_FACTOR "" CACHE STRING "" FORCE)
set(TIMEVAL "" CACHE STRING "" FORCE)
set(TIMEVAL_MAX "" CACHE STRING "" FORCE)
set(RTCAN_SOCKET "" CACHE STRING "" FORCE)
set(EMCY_MAX_ERRORS "8" CACHE STRING "" FORCE)
set(LSS_TIMEOUT_MS "1000" CACHE STRING "" FORCE)
set(LSS_FS_TIMEOUT_MS "100" CACHE STRING "" FORCE)

option(SDO_DYNAMIC_BUFFER_ALLOCATION "" OFF)
set(SDO_DYNAMIC_BUFFER_ALLOCATION_SIZE "131072" CACHE STRING "" FORCE)

# --- Build Type ---

# Make sure to set CMAKE_BUILD_TYPE when none has been specified by the user.
ensure_build_type(${CANFESTIVAL_DEFAULT_BUILD_TYPE})

# --- Targets ---

# Target: canfestival::platform
#
foreach(_platform ${CANFESTIVAL_SUPPORTED_PLATFORMS})
  set(_target platform_${_platform})
  add_library(${_target} INTERFACE)
  target_include_directories(${_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/${_platform})
endforeach()

add_library(canfestival::platform ALIAS platform_${CANFESTIVAL_TARGET_PLATFORM})

# Target: canfestival::timers
#
foreach(_timers ${CANFESTIVAL_SUPPORTED_TIMERS})
  set(_target timers_${_timers})
  add_library(${_target} INTERFACE)
  target_include_directories(${_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/timers_${_timers})
endforeach()

add_library(canfestival::timers ALIAS timers_${CANFESTIVAL_TIMERS})

# Target: canfestival::config
# This target provides all configuration files
# and headers (e.g.: config.h).
foreach(macro
        SDO_MAX_SIMULTANEOUS_TRANSFERS
        NMT_MAX_NODE_ID
        EMCY_MAX_ERRORS)
  repeat_string("repeat" "${${macro}}" " " result)
  set(${macro}_REPEAT_STRING "${result}" CACHE STRING "" FORCE)
endforeach()

set(config_h_in ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in)
set(gen_include_dir ${CMAKE_BINARY_DIR}/include)
set(config_h ${gen_include_dir}/config.h)

configure_file(${config_h_in} ${config_h})

add_library(config INTERFACE)
target_include_directories(config INTERFACE ${gen_include_dir})
target_sources(config INTERFACE ${config_h})
add_library(canfestival::config ALIAS config)

# Target: canfestival::canopen
#
set(_canopen_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/src/timer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/states.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pdo.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sdo.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/nmtMaster.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/emcy.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/objacces.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/dcf.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lifegrd.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lss.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sync.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/nmtSlave.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/symbols.c)

add_library(canopen ${_canopen_sources})
target_include_directories(canopen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_link_libraries(canopen
                      PUBLIC canfestival::config
                             canfestival::platform
                             canfestival::timers)
add_library(canfestival::canopen ALIAS canopen)

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

# --- Interface (header-only) Targets ---

# Target: canfestival::platform
# Platform-specific headers.
foreach(_platform ${CANFESTIVAL_SUPPORTED_PLATFORMS})
  set(_target platform_${_platform})
  add_library(${_target} INTERFACE)
  target_include_directories(${_target} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/${_platform})
endforeach()

add_library(canfestival::platform ALIAS platform_${CANFESTIVAL_TARGET_PLATFORM})

# Target: canfestival::timers
# The selected timer driver headers.
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

# Target: canfestival::can_headers
# Header-only target that contains all the canopen
# header files.
set(_can_headers_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(_can_headers
  ${_can_headers_dir}/dcf.h
  ${_can_headers_dir}/def.h
  ${_can_headers_dir}/emcy.h
  ${_can_headers_dir}/lifegrd.h
  ${_can_headers_dir}/lss.h
  ${_can_headers_dir}/objdictdef.h
  ${_can_headers_dir}/sdo.h
  ${_can_headers_dir}/states.h
  ${_can_headers_dir}/sync.h
  ${_can_headers_dir}/sysdep.h
  ${_can_headers_dir}/can_driver.h
  ${_can_headers_dir}/data.h
  ${_can_headers_dir}/nmtMaster.h
  ${_can_headers_dir}/nmtSlave.h
  ${_can_headers_dir}/objacces.h
  ${_can_headers_dir}/pdo.h
  ${_can_headers_dir}/timer.h
  ${_can_headers_dir}/timers_driver.h
  ${_can_headers_dir}/can.h)

add_library(can_headers INTERFACE)
target_sources(can_headers INTERFACE ${_can_headers})
target_include_directories(can_headers INTERFACE ${_can_headers_dir})
target_link_libraries(can_headers
                      INTERFACE canfestival::config
                                canfestival::platform
                                canfestival::timers)
add_library(canfestival::headers ALIAS can_headers)

# --- Drivers ---

# Target: canfestival::drivers::virtual
add_library(driver_virtual
            ${CMAKE_CURRENT_SOURCE_DIR}/drivers/can_virtual/can_virtual.c)
target_link_libraries(driver_virtual PUBLIC canfestival::headers)
add_library(canfestival::drivers::virtual ALIAS driver_virtual)

# --- Canfestival library ---

# Target: canfestival::canfestival
#
set(_can_sources_dir ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(_can_sources
  ${_can_sources_dir}/timer.c
  ${_can_sources_dir}/states.c
  ${_can_sources_dir}/pdo.c
  ${_can_sources_dir}/sdo.c
  ${_can_sources_dir}/nmtMaster.c
  ${_can_sources_dir}/emcy.c
  ${_can_sources_dir}/objacces.c
  ${_can_sources_dir}/dcf.c
  ${_can_sources_dir}/lifegrd.c
  ${_can_sources_dir}/lss.c
  ${_can_sources_dir}/sync.c
  # ${_can_sources_dir}/symbols.c # TODO: needed only with configure option USE_KERNEL_TIMER
  ${_can_sources_dir}/nmtSlave.c)

add_library(canfestival ${_can_sources})
target_link_libraries(canfestival
                      PUBLIC canfestival::drivers::virtual # TODO: driver selection
                             canfestival::headers
                             canfestival::config
                             canfestival::platform
                             canfestival::timers)
add_library(canfestival::canfestival ALIAS canfestival)

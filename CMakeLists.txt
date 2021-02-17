cmake_minimum_required(VERSION 3.19)

# Vcpkg as a submodule
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(PoeIndexer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(PoeIndexer "")

##### Compilation options
# C++20
target_compile_features(PoeIndexer PUBLIC cxx_std_20)
set_target_properties(PoeIndexer PROPERTIES CXX_EXTENSIONS OFF)
if (MSVC)
    target_compile_options(PoeIndexer PUBLIC /Zc:__cplusplus)
endif()
# Enable Link Time Optimization if available
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    set_target_properties(PoeIndexer PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "Enabled Link Time optimization")
endif()
# Warnings
if(MSVC)
    target_compile_options(PoeIndexer PRIVATE /W4)
else()
    target_compile_options(PoeIndexer PRIVATE -Wall -Wextra -Wconversion -Wsign-conversion)
endif()
# CCache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Enabled CCache")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CUDA_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

# Add source files
target_sources(PoeIndexer PRIVATE
  main.cpp
  fwd_spdlog.hh
  sizeunit.hh
  apitypes.hh
  fetcherpsapi.hh
  fetcherpsapi.cpp
  parserpsapi.hh
  parserpsapi.cpp
  dbupdater.hh
  dbupdater.cpp
  ratelimiter.hh
  ratelimiter.cc
)

##### Libraries
# Include
add_subdirectory(libs)
find_package(spdlog CONFIG REQUIRED)
find_package(simdjson CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

# Link
target_link_libraries(PoeIndexer PRIVATE
    spdlog::spdlog spdlog::spdlog_header_only
    simdjson::simdjson simdjson::simdjson-flags simdjson::simdjson-headers
    cpr
)

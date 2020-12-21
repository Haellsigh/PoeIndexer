cmake_minimum_required(VERSION 3.16)

project(PoeIndexer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(PoeIndexer "")

# LTO support
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
  set_target_properties(PoeIndexer PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Compilation options
target_compile_features(PoeIndexer PUBLIC cxx_std_20)
set_target_properties(PoeIndexer PROPERTIES CXX_EXTENSIONS OFF)

# Add source files
target_sources(PoeIndexer PRIVATE
  main.cpp
  sizeunit.hh
  apitypes.hh
  fetcherpsapi.hh
  fetcherpsapi.cpp
  parserpsapi.hh
  parserpsapi.cpp
  dbupdater.hh
  dbupdater.cpp
)

# Load libraries
add_subdirectory(libs)
find_package(spdlog CONFIG REQUIRED)
# Link libraries
target_link_libraries(PoeIndexer PRIVATE spdlog::spdlog spdlog::spdlog_header_only)

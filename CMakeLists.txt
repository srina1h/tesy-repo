# Minimum required version of CMake
cmake_minimum_required(VERSION 3.4.3)

# Project name
project(CSC512CourseProject)

set(LT_LLVM_INSTALL_DIR "/opt/homebrew/opt/llvm" CACHE PATH "LLVM installation directory")
list(APPEND CMAKE_PREFIX_PATH "${LT_LLVM_INSTALL_DIR}/lib/cmake/llvm/")

set(CMAKE_CXX_STANDARD 17)
find_package(LLVM REQUIRED CONFIG)

include(AddLLVM)

# Find and include LLV

# Add LLVM's definitions and include directories
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

# Set the output directory for LLVM bitcode files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Define the LLVM passes
add_library(BranchPointerProfiling MODULE src/BranchPointerProfiling.cpp)
add_library(SeminalInputAnalysis MODULE src/SeminalInputAnalysis.cpp)

# Link the LLVM libraries with your passes
target_link_libraries(BranchPointerProfiling PRIVATE LLVM)
target_link_libraries(SeminalInputAnalysis PRIVATE LLVM)

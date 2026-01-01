cmake_minimum_required(VERSION 3.10)  # Updated to a newer version to avoid deprecation warnings
project(database LANGUAGES C)         # Explicitly specify the language as C

set(CMAKE_C_STANDARD 99)              # Use C99 standard
set(CMAKE_C_STANDARD_REQUIRED ON)     # Enforce the C99 standard
set(CMAKE_C_EXTENSIONS OFF)           # Disable compiler-specific extensions

# Enable POSIX functions like strdup and strtok_r
add_definitions(-D_POSIX_C_SOURCE=200809L)  

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCE_FILES
    src/main.c
    src/table.c
    src/record.c
    src/hashmap.c
    src/rbtree.c
    src/bplustree.c
    src/command.c
    src/utils.c
    src/file_io.c  # Ensure file_io.c is included
)

# Create executable
add_executable(database ${SOURCE_FILES})
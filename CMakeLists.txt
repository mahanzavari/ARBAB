cmake_minimum_required(VERSION 3.10)  # Updated to a newer version to avoid deprecation warnings
project(database LANGUAGES C)         # Explicitly specify the language as C

set(CMAKE_C_STANDARD 99)              # Use C99 standard
set(CMAKE_C_STANDARD_REQUIRED ON)     # Enforce the C99 standard
set(CMAKE_C_EXTENSIONS OFF)           # Disable compiler-specific extensions

# Source files
set(SOURCES
    src/command.c
    src/hashmap.c
    src/rbtree.c
    src/record.c
    src/table.c
    src/utils.c
    src/file_io.c
    src/main.c
)

# Create executable
add_executable(database ${SOURCES})

# Include directories (modern CMake practice)
target_include_directories(database PRIVATE include)
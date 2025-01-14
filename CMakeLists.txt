cmake_minimum_required(VERSION 3.10)
project(SimpleDatabase VERSION 1.0)

# Set C standard and enable warnings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Disable unsafe function warnings for MSVC
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Include directories
include_directories(include include/cJSON)

# Add cJSON as a subdirectory
add_subdirectory(include/cJSON)

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

# Add the executable
add_executable(SimpleDatabase ${SOURCES})

# Enable warnings for the target
if(MSVC)
    target_compile_options(SimpleDatabase PRIVATE /W4 /WX /wd4210)  # /W4 enables most warnings, /WX treats warnings as errors, /wd4210 suppresses nonstandard extension warnings
else()
    target_compile_options(SimpleDatabase PRIVATE -Wall -Wextra -Werror)
endif()

# Link cJSON library
target_link_libraries(SimpleDatabase PRIVATE cjson)

# Install the executable (optional)
install(TARGETS SimpleDatabase DESTINATION bin)
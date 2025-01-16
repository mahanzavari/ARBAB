cmake_minimum_required(VERSION 3.10)
project(CoreLogic)

# Set the C standard
set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/c/command.c
    src/c/file_io.c
    src/c/hashmap.c
    src/c/rbtree.c
    src/c/record.c
    src/c/table.c
    src/c/utils.c
)

# Create a shared library
add_library(corelogic SHARED ${SOURCES})

# Set the output directory for the shared library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Ensure the library is named correctly for Java JNI
if(WIN32)
    set_target_properties(corelogic PROPERTIES SUFFIX ".dll")
elseif(UNIX)
    set_target_properties(corelogic PROPERTIES SUFFIX ".so")
endif()

# Install the shared library to the lib directory
install(TARGETS corelogic DESTINATION lib)
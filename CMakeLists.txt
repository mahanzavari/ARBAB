# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyMixedProject VERSION 1.0 LANGUAGES C Java)

# Set the source and header directories
set(C_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/C")       # C source files
set(JAVA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/java") # Java source files
set(HEADER_DIR "${CMAKE_SOURCE_DIR}/include")       # Header files

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_JAVA_CLASS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/classes")

# Find the Java Development Kit (JDK)
find_package(Java REQUIRED COMPONENTS Development)
include(UseJava)

# Compile the Java source files
add_jar(MyJavaProject
    SOURCES "${JAVA_SOURCE_DIR}/Main.java"
    OUTPUT_DIR "${CMAKE_JAVA_CLASS_OUTPUT_PATH}"
)

# Set the C source files
set(C_SOURCES
    "${C_SOURCE_DIR}/command.c"
    "${C_SOURCE_DIR}/file_io.c"
    "${C_SOURCE_DIR}/hashmap.c"
    # "${C_SOURCE_DIR}/main.c"
    "${C_SOURCE_DIR}/rbtree.c"
    "${C_SOURCE_DIR}/record.c"
    "${C_SOURCE_DIR}/table.c"
    "${C_SOURCE_DIR}/utils.c"
)

# Add the C executable
add_executable(my_c_executable ${C_SOURCES})

# Include the header directory
target_include_directories(my_c_executable PRIVATE "${HEADER_DIR}")

# Install the C executable
install(TARGETS my_c_executable
    RUNTIME DESTINATION bin
)

# Install the Java classes
install(DIRECTORY "${CMAKE_JAVA_CLASS_OUTPUT_PATH}"
    DESTINATION classes
)
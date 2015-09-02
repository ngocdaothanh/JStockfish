cmake_minimum_required(VERSION 3.3)
project(jstockfish)

# http://www.cmake.org/cmake/help/v3.0/module/FindJava.html
find_package(Java REQUIRED)
if (Java_FOUND)
    message (STATUS "Java_JAVAH_EXECUTABLE=${Java_JAVAH_EXECUTABLE}")
endif()

# http://www.cmake.org/cmake/help/v3.0/module/FindJNI.html
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
endif()

# Generate jstockfish_Uci.h
execute_process(
    COMMAND ${Java_JAVAH_EXECUTABLE} -verbose -jni
        -cp ${CMAKE_CURRENT_LIST_DIR}/src/main/java
        -d  ${CMAKE_CURRENT_LIST_DIR}/src/main/cpp
        jstockfish.Uci
)

# See Stockfish Makefile
file(GLOB SOURCES "../src/*.cpp" "../src/syzygy/tbprobe.cpp" "src/main/cpp/*.cpp")

set(CMAKE_CXX_STANDARD 11)
include_directories(${JNI_INCLUDE_DIRS} "../src" "src/main/cpp")
add_library(jstockfish SHARED ${SOURCES} ${JNI_LIBRARIES})

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

include_directories(${JNI_INCLUDE_DIRS} "../src" "src/main/cpp")

# Exclude misc_sync_cout.cpp because we'll implement uci_out to
# redirect Stockfish output to Java side
file(GLOB SOURCES "../src/*.cpp" "../src/syzygy/tbprobe.cpp" "src/main/cpp/*.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_LIST_DIR}/../src/misc_sync_cout.cpp")

set(CMAKE_CXX_STANDARD 11)

set(STOCKFISH_FLAGS, "-Wall -Wcast-qual -fno-exceptions -fno-rtti -pedantic -Wextra -Wshadow -DNDEBUG -O3 -mdynamic-no-pic -msse -DUSE_BSFQ -msse3 -mpopcnt -DUSE_POPCNT -flto")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${STOCKFISH_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STOCKFISH_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${STOCKFISH_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${STOCKFISH_FLAGS}")

add_library(jstockfish SHARED ${SOURCES} ${JNI_LIBRARIES})

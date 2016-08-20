cmake_minimum_required(VERSION 3.3)
project(Zeus)

include(conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

find_package(GTEST)
if(GTEST_NOT_FOUND)
    message(FATAL_ERROR "gtest not found, test won't success!")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${PROJECT_SOURCE_DIR})

FILE(GLOB_RECURSE SRC_LIST RELATIVE ${PROJECT_SOURCE_DIR} "src/*.c*")
FILE(GLOB_RECURSE TEST_SRC_LIST RELATIVE ${PROJECT_SOURCE_DIR} "test/*.c*")
FILE(GLOB_RECURSE SRC_INCLUDE_LIST RELATIVE ${PROJECT_SOURCE_DIR} "src/*.h*")

add_executable(Zeus main.cpp ${SRC_LIST} ${SRC_INCLUDE_LIST})
target_link_libraries(Zeus ${CONAN_LIBS})

add_executable(Z_test ${SRC_LIST} ${TEST_SRC_LIST} test/future2.cpp test/future1.cpp src/common/EventBase.cpp src/common/EventBase.h src/common/FileDesc.h test/filedescription.cpp src/net/Connect.h src/net/TcpServer.cpp src/net/TcpServer.h src/net/Acceptor.h src/common/Epoll.h)
target_link_libraries(Z_test gtest ${GTEST_MAIN_LIBRARIES} ${CONAN_LIBS})

cmake_minimum_required(VERSION 3.31)
project(Logistika)

set(CMAKE_CXX_STANDARD 20)

add_executable(Logistika
        main.cpp
        Menu.cpp
        Menu.h
        Cargo.cpp
        Cargo.h)

add_executable(Testai
        TestaiMain.cpp
        Testai.cpp
        Testai.h
        Cargo.h)

if(WIN32)
    target_link_libraries(Logistika PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(Testai   PRIVATE -static-libgcc -static-libstdc++)
endif()
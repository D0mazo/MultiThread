cmake_minimum_required(VERSION 3.31)
project(Logistika)

set(CMAKE_CXX_STANDARD 20)

# Pagrindinė programa
add_executable(Logistika
        main.cpp
        Cargo.cpp
        Cargo.h)

# Testai
add_executable(Testai
        TestaiMain.cpp
        Testai.cpp
        Testai.h
        Cargo.h)

# Windows threads
if(WIN32)
    target_link_libraries(Logistika PRIVATE -static-libgcc -static-libstdc++)
    target_link_libraries(Testai   PRIVATE -static-libgcc -static-libstdc++)
endif()

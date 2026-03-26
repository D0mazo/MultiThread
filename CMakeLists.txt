cmake_minimum_required(VERSION 3.10)
project(MultiThread)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(MultiThread
        main.cpp
        Calculations/Cargo.cpp
        Input/InputData.cpp
        Menu.cpp
        Functions/Benchmark.cpp
)

target_include_directories(MultiThread PRIVATE . Functions)
target_link_libraries(MultiThread pthread)

# DataGenerator atskiras executable
add_executable(DataGenerator
        DataGenerator/generate_data.cpp
)

set_target_properties(DataGenerator PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/DataGenerator"
)
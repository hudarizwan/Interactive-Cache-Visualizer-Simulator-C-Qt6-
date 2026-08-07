cmake_minimum_required(VERSION 3.21)

project(CacheSimulatorLab VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(CacheSimulatorLab
    src/main.cpp
    src/MainWindow.cpp
    src/CacheTypes.cpp
    src/TraceParser.cpp
    src/Memory.cpp
    src/CacheLine.cpp
    src/CacheSet.cpp
    src/ReplacementPolicy.cpp
    src/AddressDecoder.cpp
    src/CacheEngine.cpp
)

target_include_directories(CacheSimulatorLab PRIVATE src)

target_link_libraries(CacheSimulatorLab PRIVATE Qt6::Widgets)


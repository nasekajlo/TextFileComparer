cmake_minimum_required(VERSION 3.10)

project(TextFileComparer)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)
find_package(ICU REQUIRED)

add_executable(TextFileComparer
    src/file_parser.cpp
    src/comparer.cpp
    src/matcher.cpp
    src/gui.cpp
    src/merger.cpp
)

target_link_libraries(TextFileComparer
    Qt5::Widgets
    Boost::Boost
    ICU::ICU
)

enable_testing()
add_subdirectory(tests)

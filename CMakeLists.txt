cmake_minimum_required(VERSION 3.10)

project(TextFileComparer)

# Qt6
set(Qt6_DIR "C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Boost
set(Boost_ROOT "C:/vcpkg/installed/x64-windows")
find_package(Boost REQUIRED)

# ICU
set(ICU_ROOT "C:/vcpkg/installed/x64-windows")
find_package(ICU REQUIRED)

add_executable(TextFileComparer
    src/file_parser.cpp
    src/comparer.cpp
    src/matcher.cpp
    src/gui.cpp
    src/merger.cpp
)

target_link_libraries(TextFileComparer
    Qt6::Widgets
    Boost::Boost
    ICU::ICU
)

enable_testing()
add_subdirectory(tests)

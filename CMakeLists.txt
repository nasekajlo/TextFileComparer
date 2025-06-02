cmake_minimum_required(VERSION 3.10)

project(TextFileComparer)

cmake_policy(SET CMP0074 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Boost REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(ICU_INCLUDE_DIR /usr/include)
set(ICU_LIBRARY /usr/lib/x86_64-linux-gnu/libicuuc.so)
set(ICU_I18N_LIBRARY /usr/lib/x86_64-linux-gnu/libicui18n.so)
find_package(ICU REQUIRED COMPONENTS i18n uc)

enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
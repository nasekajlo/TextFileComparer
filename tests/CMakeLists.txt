add_executable(runTests
    Test_fileparser.cc
    Test_diff.cc
    Test_merger.cc
)

target_include_directories(runTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(runTests PRIVATE textfilecomparer_core GTest::GTest GTest::Main Threads::Threads)
enable_testing()
add_test(NAME AllTests COMMAND runTests)

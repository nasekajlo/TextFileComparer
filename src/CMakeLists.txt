add_library(textfilecomparer_core
    Document.h
    FileParser.h FileParser.cc
    Diff.h Diff.cc
    Merger.h Merger.cc
)

set(CMAKE_AUTOMOC ON)

target_link_libraries(textfilecomparer_core PUBLIC ICU::uc ICU::i18n)

add_executable(TextFileComparer
    Main.cc
    Mainwindow.h Mainwindow.cc
)

target_link_libraries(TextFileComparer PRIVATE textfilecomparer_core Qt6::Widgets)

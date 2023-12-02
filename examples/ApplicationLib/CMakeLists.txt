add_library(ApplicationLib
    src/CircularBuffer.cpp
    src/EventDispatcher.cpp
    src/hello.c
    src/Printer.cpp
)

target_include_directories(ApplicationLib
    PUBLIC
        include
)

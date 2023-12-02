add_executable(ExampleTests
    src/AllTests.cpp
    src/CircularBufferTest.cpp
    src/EventDispatcherTest.cpp
    src/FEDemoTest.cpp
    src/HelloTest.cpp
    src/MockDocumentationTest.cpp
    src/PrinterTest.cpp
)

target_include_directories(ExampleTests
    PRIVATE
        include
)

target_compile_options(ExampleTests
    PRIVATE $<$<BOOL:${MSVC}>:/wd4723>
)

target_link_libraries(ExampleTests
    PRIVATE
        ApplicationLib
        CppUTest::CppUTestExt
)

if(CPPUTEST_TEST_DISCOVERY OR NOT DEFINED CPPUTEST_TEST_DISCOVERY)
    include(CppUTest)
    cpputest_discover_tests(ExampleTests)
endif()

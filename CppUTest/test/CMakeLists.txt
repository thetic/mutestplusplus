add_executable(CppUTestTests
    src/AllTests.cpp
    src/CheatSheetTest.cpp
    src/CommandLineArgumentsTest.cpp
    src/CommandLineTestRunnerTest.cpp
    src/CompatabilityTests.cpp
    src/JUnitOutputTest.cpp
    src/PluginTest.cpp
    src/PreprocessorTest.cpp
    src/SetPluginTest.cpp
    src/SimpleStringCacheTest.cpp
    src/SimpleStringTest.cpp
    src/TeamCityOutputTest.cpp
    src/TestFailureNaNTest.cpp
    src/TestFailureTest.cpp
    src/TestFilterTest.cpp
    src/TestHarnessTest.cpp
    src/TestHarnessTest.c
    src/TestInstallerTest.cpp
    src/TestMemoryAllocatorTest.cpp
    src/TestOutputTest.cpp
    src/TestRegistryTest.cpp
    src/TestResultTest.cpp
    src/TestUTestMacro.cpp
    src/TestUTestStringMacro.cpp
    src/UtestTest.cpp
)

target_include_directories(CppUTestTests
    PRIVATE include
)

target_link_libraries(CppUTestTests
    PRIVATE CppUTest
)

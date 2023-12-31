add_library(CppUTestExt STATIC
    src/IEEE754ExceptionsPlugin.cpp
    src/MockFailure.cpp
    src/MockSupportPlugin.cpp
    src/MockActualCall.cpp
    src/MockSupport_c.cpp
    src/MockExpectedCall.cpp
    src/MockNamedValue.cpp
    src/OrderedTest.cpp
    src/MockExpectedCallsList.cpp
    src/MockSupport.cpp
)

target_compile_features(CppUTest
    PUBLIC
        cxx_std_11
        c_std_99
)

target_link_libraries(CppUTestExt PUBLIC CppUTest)

#[[Arrange for the include directory to be added to the include paths of any CMake target depending on CppUTestExt.]]
target_include_directories(CppUTestExt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(CppUTest::CppUTestExt ALIAS CppUTestExt)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(test)
endif()

if(PROJECT_IS_TOP_LEVEL)
    install(
        TARGETS CppUTestExt
        EXPORT CppUTestTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/CppUTestExt"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()

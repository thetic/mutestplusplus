set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

include(CMakePackageConfigHelpers)

set(CPPUTEST_CONFIG_DEST "${LIB_INSTALL_DIR}/CppUTest/cmake")

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CppUTestConfig.cmake.install.in
    ${CMAKE_CURRENT_BINARY_DIR}/install/CppUTestConfig.cmake
    INSTALL_DESTINATION "${CPPUTEST_CONFIG_DEST}"
    PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/install/CppUTestConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/install/CppUTestConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/install/CppUTestConfigVersion.cmake
    DESTINATION "${CPPUTEST_CONFIG_DEST}"
)
install(
    EXPORT CppUTestTargets
    NAMESPACE CppUTest::
    DESTINATION "${CPPUTEST_CONFIG_DEST}"
)
install(
    DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/Modules"
    DESTINATION "${CPPUTEST_CONFIG_DEST}"
)
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/CppUTestConfig.cmake.build.in
    ${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfig.cmake
    INSTALL_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    PATH_VARS INCLUDE_DIR CMAKE_CURRENT_BINARY_DIR
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CppUTestConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
)

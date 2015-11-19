cmake_minimum_required(VERSION 3.1) # We need CMAKE_C_STANDARD variable
project(chizu)

# Add custom cmake modules to find SDL
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

add_subdirectory(src)


# create engine-config.cmake stuff
set(CMAKE_CONFIG_INSTALL_DIR "lib/cmake/${PROJECT_NAME}" CACHE STRING "Where ${PROJECT_NAME}Config.cmake and companions will be installed")


set(INCLUDE_INSTALL_DIR include/chizu)
set(LIB_INSTALL_DIR lib)
set(CHIZU_VERSION "1.0")

export(PACKAGE Chizu)

include(CMakePackageConfigHelpers)
configure_package_config_file(
        cmake-src/${PROJECT_NAME}-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/config-tmp
        INSTALL_DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        VERSION ${CHIZU_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/config-tmp
        RENAME ${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        )

install(EXPORT ChizuTargets DESTINATION ${CMAKE_CONFIG_INSTALL_DIR})

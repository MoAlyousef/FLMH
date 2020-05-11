cmake_minimum_required(VERSION 3.0)
project(flmh VERSION 0.1.0 LANGUAGES CXX)

add_library(flmh INTERFACE)
add_library(flmh::flmh ALIAS flmh)

target_compile_features(flmh INTERFACE cxx_std_17)

target_include_directories(
    flmh
    INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Install
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/flmhConfigVersion.cmake" 
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion)

install(TARGETS flmh
    EXPORT flmhTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/flmhConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/flmhConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flmh
)

install(EXPORT flmhTargets NAMESPACE flmh:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flmh)
install(FILES "${PROJECT_BINARY_DIR}/flmhConfig.cmake" "${PROJECT_BINARY_DIR}/flmhConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flmh)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/include" DESTINATION .)

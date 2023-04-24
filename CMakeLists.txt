cmake_minimum_required(VERSION 3.0.0)
project(RDF_Shredder VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest)
find_package(UniversalLibrary REQUIRED)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(version 0.1.0)

include(GoogleTest)
enable_testing()

add_library(RDF_Shredder RDF_Shredder.cpp)
target_link_libraries(RDF_Shredder pugixml)
set_target_properties(RDF_Shredder PROPERTIES PUBLIC_HEADER "RDF_Shredder.hpp")
target_include_directories(RDF_Shredder PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_executable(BehaviorTests RDF_ShredderTest.cpp)
target_link_libraries(BehaviorTests RDF_Shredder pugixml GTest::Main)
gtest_discover_tests(BehaviorTests)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/RDF_ShredderConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RDF_Shredder
)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/RDF_ShredderConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/RDF_ShredderConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RDF_Shredder
)

install(
    TARGETS RDF_Shredder
    EXPORT RDF_Shredder
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/Stilt_Fox/RDF_Shredder
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/Stilt_Fox/RDF_Shredder
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/Stilt_Fox/RDF_Shredder
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Stilt_Fox/RDF_Shredder
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Stilt_Fox/RDF_Shredder
)

install(EXPORT RDF_Shredder
        FILE RDF_ShredderTargets.cmake
        NAMESPACE RDF_Shredder::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/RDF_Shredder
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/RDF_ShredderConfigVersion.cmake"
  VERSION "${version}"
  COMPATIBILITY AnyNewerVersion
)
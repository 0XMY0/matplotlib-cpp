cmake_minimum_required(VERSION 3.8)
project(matplotlib_cpp)
message("Install path is ${CMAKE_INSTALL_PREFIX}")
message("Build type is ${CMAKE_BUILD_TYPE}")

install(FILES ${CMAKE_SOURCE_DIR}/matplotlibcpp.h DESTINATION include)

set(matplotlib_cpp_include_dirs ${CMAKE_INSTALL_PREFIX}/include)
configure_file(${CMAKE_SOURCE_DIR}/cmake/matplotlib_cppConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/matplotlib_cppConfig.cmake" @ONLY)
install(FILES 
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/matplotlib_cppConfig.cmake" 
    DESTINATION share/matplotlib_cpp/cmake)

set(BUILD_TESTING 1)
if(BUILD_TESTING)
  message("Build examples")
  # include(${CMAKE_SOURCE_DIR}/cmake/examples.cmake)
  include(${CMAKE_SOURCE_DIR}/chz/chz.cmake)
endif()
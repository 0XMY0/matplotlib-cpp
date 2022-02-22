find_package(matplotlib_cpp REQUIRED)
add_compile_options(/MT)

link_libraries(${matplotlib_LIBS})
message("${matplotlib_LIBS}")

# Examples
add_executable(chzplotdat chz/chzplotdat.cpp)
set_target_properties(chzplotdat PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
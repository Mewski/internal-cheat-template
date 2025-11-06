add_executable(stub-executable
  src/main.cpp
)

set_target_properties(stub-executable PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

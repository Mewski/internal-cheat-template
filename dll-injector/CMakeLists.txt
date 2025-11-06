add_executable(dll-injector
  src/main.cpp
)

set_target_properties(dll-injector PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(dll-injector PRIVATE
  ng-log::ng-log
)

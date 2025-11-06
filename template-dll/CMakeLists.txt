add_library(template-dll SHARED
  src/dllmain.cpp
)

set_target_properties(template-dll PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_link_libraries(template-dll PRIVATE
  ng-log::ng-log
  PolyHook_2
)

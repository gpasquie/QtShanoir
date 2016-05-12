macro(set_lib_install_rules
  project_name
  )

################################################################################
#
# Usage: set_lib_install_rules(target)
# set rules for the library designed by the target
#
################################################################################

set_target_properties(${project_name} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/
  )

install(TARGETS ${project_name}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

endmacro()


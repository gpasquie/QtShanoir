macro(set_exe_install_rules
  target
  )

################################################################################
#
# Usage: set_exe_install_rules(target)
# set rules for the executable designed by the target
#
################################################################################

set_target_properties(${target} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
  )

install(TARGETS ${target}
  RUNTIME DESTINATION bin
  BUNDLE  DESTINATION bin
  ARCHIVE DESTINATION bin
  )

endmacro()


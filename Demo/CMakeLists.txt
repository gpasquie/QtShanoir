set(TARGET_NAME qtshanoir-demo)

find_package(QuaZip REQUIRED)

## #############################################################################
## List Sources
## #############################################################################

list_source_files(${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

## #############################################################################
## include directorie.
## #############################################################################

include_directories(${${TARGET_NAME}_INCLUDE_DIRS} ${QuaZip_INCLUDE_DIRS} ${QtShanoirDAO_INCLUDE_DIRS} ${QtShanoirGUI_INCLUDE_DIRS} )

## #############################################################################
## Translate.
## #############################################################################



## #############################################################################
## Lib definition.
## #############################################################################

add_executable(${TARGET_NAME}
  ${${TARGET_NAME}_CFILES}
  ${${TARGET_NAME}_UI}
  ${qm_files}
  )

target_link_libraries(${TARGET_NAME}
  ${QT_LIBRARIES}
  QtShanoirDAO
  QtShanoirGUI
  )

set_exe_install_rules(${TARGET_NAME})

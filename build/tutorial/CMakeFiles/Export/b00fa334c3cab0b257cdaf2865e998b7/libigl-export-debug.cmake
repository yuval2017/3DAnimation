#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "igl::core" for configuration "Debug"
set_property(TARGET igl::core APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(igl::core PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libigl.a"
  )

list(APPEND _cmake_import_check_targets igl::core )
list(APPEND _cmake_import_check_files_for_igl::core "${_IMPORT_PREFIX}/lib/libigl.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

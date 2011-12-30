if(NOT QT4_FOUND)
  find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
  message("QT_USE_FILE=${QT_USE_FILE}")
  include(${QT_USE_FILE})
endif(NOT QT4_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/SuperBuild)

#-----------------------------------------------------------------------------
# Define Superbuild global variables
#-----------------------------------------------------------------------------

# This variable will contain the list of CMake variable specific to each external project
# that should passed to ${CMAKE_PROJECT_NAME}.
# The item of this list should have the following form: <EP_VAR>:<TYPE>
# where '<EP_VAR>' is an external project variable and TYPE is either BOOL, STRING, PATH or FILEPATH.
# TODO Variable appended to this list will be automatically exported in BRAINSToolsConfig.cmake,
# prefix 'BRAINSTools_' will be prepended if it applies.
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS)

# The macro '_expand_external_project_vars' can be used to expand the list of <EP_VAR>.
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS) # List of CMake args to configure BRAINS
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES) # List of CMake variable names

# Convenient macro allowing to expand the list of EP_VAR listed in ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
# The expanded arguments will be appended to the list ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
# Similarly the name of the EP_VARs will be appended to the list ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES.
macro(_expand_external_project_vars)
  foreach(arg ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS})
    string(REPLACE ":" ";" varname_and_vartype ${arg})
    set(target_info_list ${target_info_list})
    list(GET varname_and_vartype 0 _varname)
    list(GET varname_and_vartype 1 _vartype)
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS -D${_varname}:${_vartype}=${${_varname}})
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES ${_varname})
  endforeach()
endmacro()

#-----------------------------------------------------------------------------
# Common external projects CMake variables
#-----------------------------------------------------------------------------
list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
  CMAKE_BUILD_TYPE:PATH
  MAKECOMMAND:STRING
  CMAKE_SKIP_RPATH:BOOL
  CMAKE_BUILD_TYPE:STRING
  CMAKE_CXX_COMPILER:PATH
  CMAKE_CXX_FLAGS_RELEASE:STRING
  CMAKE_CXX_FLAGS_DEBUG:STRING
  CMAKE_CXX_FLAGS:STRING
  CMAKE_C_COMPILER:PATH
  CMAKE_C_FLAGS_RELEASE:STRING
  CMAKE_C_FLAGS_DEBUG:STRING
  CMAKE_C_FLAGS:STRING
  CMAKE_SHARED_LINKER_FLAGS:STRING
  CMAKE_EXE_LINKER_FLAGS:STRING
  CMAKE_MODULE_LINKER_FLAGS:STRING
  CMAKE_GENERATOR:STRING
  CMAKE_EXTRA_GENERATOR:STRING
  CMAKE_INSTALL_PREFIX:PATH
  CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH
  CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH
  CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH
  CMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH
  CTEST_NEW_FORMAT:BOOL
  MEMORYCHECK_COMMAND_OPTIONS:STRING
  MEMORYCHECK_COMMAND:PATH
  CMAKE_SHARED_LINKER_FLAGS:STRING
  CMAKE_EXE_LINKER_FLAGS:STRING
  CMAKE_MODULE_LINKER_FLAGS:STRING
  SITE:STRING
  BUILDNAME:STRING
  QT_QMAKE_EXECUTABLE:FILEPATH
  )
#-----------------------------------------------------------------------------
# Set CMake OSX variable to pass down the external project
#-----------------------------------------------------------------------------
if(APPLE)
  list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
    CMAKE_OSX_ARCHITECTURES:STRING
    CMAKE_OSX_SYSROOT:STRING
    CMAKE_OSX_DEPLOYMENT_TARGET:STRING)
endif()

_expand_external_project_vars()

include(External_ITKv4)
include(External_VTK)

#------------------------------------------------------------------------------
# Configure and build
#------------------------------------------------------------------------------
set(proj VV)
ExternalProject_Add(${proj}
  DEPENDS ${VV_DEPENDENCIES}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR VV-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    --no-warn-unused-cli # HACK Only expected variables should be passed down.
    ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS}
    -DVV_SUPERBUILD:BOOL=OFF
    -DITK_DIR:PATH=${ITK_DIR}
    -DVTK_DIR:PATH=${VTK_DIR}
  INSTALL_COMMAND ""
  DEPENDS ITK VTK
  )


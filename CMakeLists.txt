cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

## NOTE THERE SHOULD BE NO PROJECT STATEMENT HERE!
## This file acts as a simple switch to initiate
## two completely independant CMake build environments.

#-----------------------------------------------------------------------------
# Superbuild Option - Enabled by default
#                   Phase I:  VV_SUPERBUILD is set to ON, and the
#                             supporting packages defined in "SuperBuild.cmake"
#                             are built.  The last package in "SuperBuild.cmake"
#                             to be built is a recursive call to this
#                             file with VV_SUPERBUILD explicitly
#                             set to "OFF" to initiate Phase II
#
#                   Phase II: Build the VV, referencing the support
#                             packages built in Phase I.
#-----------------------------------------------------------------------------
option(VV_SUPERBUILD "Build VV and the projects it depends on via SuperBuild.cmake." ON)
mark_as_advanced(VV_SUPERBUILD)

include(ExternalProject)

#-----------------------------------------------------------------------------
# Superbuild script
#-----------------------------------------------------------------------------
if(VV_SUPERBUILD)
  project(SuperBuild_VV)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
else()
  project(VV)
  include("${CMAKE_CURRENT_SOURCE_DIR}/VV.cmake")
  return()
endif()

message(FATAL_ERROR "You should never reach this point !")

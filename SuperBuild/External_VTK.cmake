
set(VTK_QT_ARGS)
if(NOT APPLE)
  set(VTK_QT_ARGS
    -DVTK_USE_QT:BOOL=ON
    -DDESIRED_QT_VERSION:STRING=4
    -DVTK_USE_GUISUPPORT:BOOL=ON
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DVTK_USE_RENDERING:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    -DVTK_USE_PARALLEL:BOOL=ON
    )
else()
  set(VTK_QT_ARGS
    -DVTK_REQUIRED_OBJCXX_FLAGS:STRING=""
    -DVTK_USE_QT:BOOL=ON
    -DVTK_USE_CARBON:BOOL=OFF
    -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
    -DVTK_USE_X:BOOL=OFF
    -DVTK_USE_RPATH:BOOL=ON
    -DDESIRED_QT_VERSION:STRING=4
    -DVTK_USE_GUISUPPORT:BOOL=ON
    -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
    -DVTK_USE_RENDERING:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    -DVTK_USE_PARALLEL:BOOL=ON
    )
endif()

set(proj VTK)

set(VTK_DIR ${CMAKE_CURRENT_BINARY_DIR}/${proj}-build)

ExternalProject_Add(${proj}
  SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
  BINARY_DIR ${VTK_DIR}
  GIT_REPOSITORY "git://vtk.org/VTK.git"
  GIT_TAG "v6.1.0"
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
  CMAKE_ARGS
  ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS}
  -DBUILD_EXAMPLES:BOOL=OFF
  -DBUILD_TESTING:BOOL=OFF
  -DVTK_LEGACY_REMOVE:BOOL=ON
  ${VTK_QT_ARGS}
  )

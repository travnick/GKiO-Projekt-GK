cmake_minimum_required(VERSION 2.8)
project(RayTracer)

set(Debug Debug)
set(RelWithDebInfo RelWithDebInfo)
set(Profile Profile)
set(Release Release)
set(x86 x86)
set(Native Native)

set(CMAKE_BUILD_TYPE ${Debug} CACHE STRING "Set build type: ${Debug} | ${RelWithDebInfo} | ${Profile} | ${Release}")
set(PLATFORM_TARGET ${Native} CACHE STRING "Set target platform: ${x86} | ${Native}")

include(CmakeIncludes/CompilerOptions.cmake)
include(CmakeIncludes/Directories.cmake)
include(CmakeIncludes/VSFolders.cmake)
include(CmakeIncludes/Libraries.cmake)

include(CmakeIncludes/Model.cmake)
add_library(Model STATIC ${HDRS_Model} ${SRCS_Model})

include(CmakeIncludes/Controller.cmake)

include(CmakeIncludes/View.cmake)

add_executable(Main WIN32 ${HDRS_Controller} ${SRCS_Controller} ${FORMS_HEADERS_RayTracer} ${MOC_HEADERS_RayTracer} ${HDRS_View} ${SRCS_View})

if(UNIX)
  target_link_libraries(
    Main
    Model
    rt
    ${QT_LIBRARIES}
  )
else()
  target_link_libraries(
    Main
    Model
    ${QT_LIBRARIES}
  )
endif()

cmake_minimum_required(VERSION 2.8)
project(RayTracer)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug|Release)")

include(includes/CompilerOptions.cmake)
include(includes/Directories.cmake)
include(includes/VSFolders.cmake)
include(includes/Libraries.cmake)

include(includes/Model.cmake)
add_library(Model STATIC ${HDRS_Model} ${SRCS_Model})

include(includes/Controller.cmake)

include(includes/View.cmake)

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
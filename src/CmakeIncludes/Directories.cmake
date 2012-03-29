set(MAIN_DIR ${RayTracer_SOURCE_DIR}/..)

set(SOURCE_DIR ${RayTracer_SOURCE_DIR})
set(PROJECT_SOURCE_DIR ${SOURCE_DIR})
set(SOURCE_VIEW_DIR ${SOURCE_DIR}/View)

include_directories (${SOURCE_DIR})

if(UNIX)
  set(BIN_DIR ${MAIN_DIR}/bin/linux)
elseif(WIN32)
  set(BIN_DIR ${MAIN_DIR}/bin/windows)
endif()

# Paths for executables.
if(UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}/${CMAKE_BUILD_TYPE})
elseif(WIN32)
  # Visual Studio adds Debug/Release folders by itself.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
endif()

 #Paths for .a and .so (.dll) libs.
if(UNIX)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MAIN_DIR}/lib/linux/${CMAKE_BUILD_TYPE})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MAIN_DIR}/lib/linux/${CMAKE_BUILD_TYPE})
elseif(WIN32)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MAIN_DIR}/lib/windows)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MAIN_DIR}/lib/windows)
endif()

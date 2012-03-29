include(CmakeIncludes/Macros.cmake)

set(HDRS_View
  ${SOURCE_DIR}/View/ImageViewer.h
)
set(SRCS_View
  ${SOURCE_DIR}/View/ImageViewer.cpp
)

# Ui files.
set(FORMS_RayTracer
  ${SOURCE_VIEW_DIR}/MainWindow.ui
)


QT4_WRAP_CPP_CUSTOM(MOC_HEADERS_RayTracer ${HDRS_Controller})

#workaround for changing destination dir
#QT4_WRAP_UI's using CMAKE_CURRENT_BINARY_DIR as destination dir
#it may change in future wersion of cmake !!
set (tmp ${CMAKE_CURRENT_BINARY_DIR})

#setting destination for UI source and header files
set (CMAKE_CURRENT_BINARY_DIR ${SOURCE_VIEW_DIR})

QT4_WRAP_UI(FORMS_HEADERS_RayTracer ${FORMS_RayTracer})

# message (${FORMS_HEADERS_RayTracer})
# message (${CMAKE_CURRENT_BINARY_DIR})
#getting back current dir
set (CMAKE_CURRENT_BINARY_DIR tmp)

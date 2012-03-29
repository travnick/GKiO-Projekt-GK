#for all systems
find_package (Qt4 REQUIRED QtCore QtGui QtXml)

include(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

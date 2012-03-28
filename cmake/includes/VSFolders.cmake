
# Model.
SOURCE_GROUP("Header Files" FILES
  ${SOURCE_DIR}/Model/Camera.h
  ${SOURCE_DIR}/Model/Color.h
  ${SOURCE_DIR}/Model/Light.h
  ${SOURCE_DIR}/Model/Material.h
  ${SOURCE_DIR}/Model/ModelDefines.h
  ${SOURCE_DIR}/Model/Object.h
  ${SOURCE_DIR}/Model/Point.h
  ${SOURCE_DIR}/Model/Point2D.h
  ${SOURCE_DIR}/Model/PointAndVectorOperations.h
  ${SOURCE_DIR}/Model/Ray.h
  ${SOURCE_DIR}/Model/RenderTileData.h
  ${SOURCE_DIR}/Model/Renderer.h
  ${SOURCE_DIR}/Model/Scene.h
  ${SOURCE_DIR}/Model/SceneFileManager.h
  ${SOURCE_DIR}/Model/Sphere.h
  ${SOURCE_DIR}/Model/Vector.h
  ${SOURCE_DIR}/Model/VisibleObject.h
  ${SOURCE_DIR}/Model/VpCommon.h
)
SOURCE_GROUP("Source Files" FILES
  ${SOURCE_DIR}/Model/Camera.cpp
  ${SOURCE_DIR}/Model/Material.cpp
  ${SOURCE_DIR}/Model/Object.cpp
  ${SOURCE_DIR}/Model/Renderer.cpp
  ${SOURCE_DIR}/Model/Scene.cpp
  ${SOURCE_DIR}/Model/SceneFileManager.cpp
  ${SOURCE_DIR}/Model/Sphere.cpp
)

# Controller.
SOURCE_GROUP("Header Files" FILES
  ${SOURCE_DIR}/Controller/GlobalDefines.h
  ${SOURCE_DIR}/Controller/MainWindow.h
  ${SOURCE_DIR}/Controller/RendererThread.h
  ${SOURCE_DIR}/Controller/ThreadRunner.h
)
SOURCE_GROUP("Source Files" FILES
  ${SOURCE_DIR}/Controller/MainWindow.cpp
  ${SOURCE_DIR}/Controller/RendererThread.cpp
  ${SOURCE_DIR}/Controller/ThreadRunner.cpp
  ${SOURCE_DIR}/Controller/main.cpp
)

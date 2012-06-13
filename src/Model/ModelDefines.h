/// @file Model/ModelDefines.h

#pragma once

#include <cmath>
#include <QSharedPointer>
#include <QString>

// Functions -->
/**
 *
 * @param string
 * @return
 */
inline QString toLowerCase (const char* string)
{
  return QString::fromStdString(string).toLower();
}
// <-- Functions

//Constants -->
const float PI = 3.14159265;

#define FLOAT_PRECISION 6

const float FLOAT_EPSILON = 0.01f;
// <-- Constants

//Macros -->
/**Converts degree to radian
 *
 * @param x angle in degree
 * @return angle in radian
 */
#define RAD(x) (x*(PI/180))

/**Converts a macro argument into a string constant
 *
 * @param x argument to stringify
 * @return string constant
 */
#define STRINGIFY(x) #x

/**Converts a macro argument into a lower case QString
 *
 * @param x argument to stringify
 * @return lower case string
 */
#define LCSTRING(x) toLowerCase(STRINGIFY(x))
// <--Macros

//Templates -->
template <typename T>
T SQRT (T value)
{
  return sqrt(value);
}

//inline is important because of "multiple definition of" error
template <>
inline float SQRT (float value)
{
  return sqrtf(value);
}
// <--Templates

namespace Model
{
  //Defines world unit for all project
  typedef float worldUnit;

  namespace Objects
  {
    /**Available object types
     *
     */
    enum ObjectType
    {
      None, Camera, Light, Sphere, Plane
    };

    /**String representation of object types
     *
     */
    const QString CAMERA_NAME = LCSTRING(Camera);
    const QString LIGHT_NAME = LCSTRING(Light);
    const QString OBJECTS_NAME = LCSTRING(Objects);
    const QString PLANE_NAME = LCSTRING(Plane);
    const QString SPHERE_NAME = LCSTRING(Sphere);

  }

  namespace Cameras
  {
    /**String representation of camera types
     *
     */
    const QString ORTHOGONAL = LCSTRING(ORTHOGONAL);
    const QString CONIC = LCSTRING(CONIC);
  }

  namespace Materials
  {
    /**String representation of materials section name in scene file
     *
     */
    const QString MATERIALS_NAME = LCSTRING(Materials);
  }

  namespace Lights
  {
    /**String representation of light section name in scene file
     *
     */
    const QString LIGHTS_NAME = LCSTRING(Lights);
  }

  /**Returns rounded value of given value
   *
   * Its only for non negative values
   *
   * @param value value to round
   * @return rounded value
   */
  template <typename T>
  inline T uRound (float value)
  {
    return static_cast <T>(value + 0.5f);
  }

  /**Movement directions
   *
   */
  enum Direction
  {
    Forward, Backward, Left, Right, Up, Down
  };

  //Forward declarations -->
  class Camera;
  class Light;
  class Material;
  class Renderer;
  class RenderTileData;
  class Scene;
  class VisibleObject;
  // <-- Forward declarations

  typedef QSharedPointer <Camera> CameraPtr;
  typedef QSharedPointer <Light> LightPtr;
  typedef QSharedPointer <Material> MaterialPtr;
  typedef QSharedPointer <Scene> ScenePtr;
  typedef QSharedPointer <VisibleObject> ObjectPtr;
} //namespace Model

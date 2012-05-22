/// @file Model/ModelDefines.h

#pragma once

#include <QString>
#include <cmath>

/**
 *
 * @param string
 * @return
 */
inline QString toLowerCase (const char* string){
  return QString::fromStdString(string).toLower();
}

//Constants -->
const float PI = 3.14159265;

#define FLOAT_PRECISION 6

const float DEFAULT_INTERSECTION_ERROR_VALUE = 0.1f;
// <-- Constants

//Macros -->
/**Converts degree to radian
 *
 * @param value angle in degree
 * @return angle in radian
 */
#define RAD(x) (x*(PI/180))

/**Converts a macro argument into a string constant
 *
 * @param value argument to stringify
 * @return string constant
 */
#define STRINGIFY(x) #x

/**Converts a macro argument into a lower case QString
 *
 * @param value argument to stringify
 * @return lower case string
 */
#define LCSTRING(x) toLowerCase(STRINGIFY(x))
// <--Macros

//Templates -->
template <typename T>
T SQRT (T value){
  return sqrt(value);
}

//inline is important because of "multiple definition of" error
template <>
inline float SQRT (float value){
  return sqrtf(value);
}
// <--Templates

// Functions -->

QString toLowerCase (const char* string);

// <-- Functions

namespace Model {
  //Defines world unit for all project
  typedef float worldUnit;

  namespace Objects {
    /**Available object types
     *
     */
    enum ObjectType {
      None, Camera, Light, Sphere, Plane
    };

    /**String representation of object types
     *
     */
    const QString OBJECTS_NAME = LCSTRING(Objects);
    const QString CAMERA_NAME = LCSTRING(Camera);
    const QString LIGHT_NAME = LCSTRING(Light);
    const QString SPHERE_NAME = LCSTRING(Sphere);
    const QString PLANE_NAME = LCSTRING(Plane);

  }

  namespace Cameras {
    /**String representation of camera types
     *
     */
    const QString ORTHOGONAL = LCSTRING(ORTHOGONAL);
    const QString CONIC = LCSTRING(CONIC);
  }

  namespace Materials {
    /**String representation of materials section name in scene file
     *
     */
    const QString MATERIALS_NAME = LCSTRING(Materials);
  }

  namespace Lights {
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
  inline T uRound (float value){
    return static_cast <T>(value + 0.5f);
  }

  /**Movement directions
   *
   */
  enum Direction {
    Forward, Backward, Left, Right, Up, Down
  };
}

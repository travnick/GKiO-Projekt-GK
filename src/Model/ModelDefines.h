/// @file Main/Model/ModelDefines.h
/// @date 22-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QString>
#include <cmath>

//Constants -->
#define PI 3.14159265

#define FLOAT_PRECISION 6
// <-- Constants

//Macros -->
/**Converts degrees to radians
 *
 * @param value angle in degrees
 * @return angle in radians
 */
#define RAD(x) (x*(PI/180))

/**Converts a macro argument into a string constant
 *
 * @param value argument to stringify
 * @return string constant
 */
#define STRINGIFY(x) #x

/**Converts a macro argument into a lowercase std::string
 *
 * @param value argument to stringify
 * @return lowercase string
 */
#define LCSTRING(x) stringifytolowercase(STRINGIFY(x))
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

QString stringifytolowercase (const char* string);

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
}

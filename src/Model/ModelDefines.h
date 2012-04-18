/// @file Main/Model/ModelDefines.h
/// @date 22-12-2011
/// @author Mikołaj Milej

#pragma once

#include <string>
#include <cmath>

//Constants -->
#define PI 3.14159265

#define INTERSECTION_ERROR_TOLERANCE 0.01f
// <-- Constants

//Macros -->
#define RAD(x) (x*(PI/180))
// <--Macros

//Template -->
template <typename T>
T SQRT (T value){
  return sqrt(value);
}

//inline is important because of "multiple definition of" error
template <>
inline float SQRT (float value){
  return sqrtf(value);
}
// <--Template

namespace Model {
  //Defines world unit for all project
  typedef float worldUnit;

  namespace Objects {
    /**Available object types
     *
     */
    enum ObjectType {
      None, Camera, Light, Sphere
    };

    /**String representation of object types
     *
     */
    const std::string OBJECTS_NAME = "objects";
    const std::string CAMERA_NAME = "camera";
    const std::string LIGHT_NAME = "light";
    const std::string SPHERE_NAME = "sphere";
  }

  namespace Cameras {
    /**String representation of camera types
     *
     */
    const std::string ORTHOGONAL = "orthogonal";
    const std::string CONIC = "conic";
  }

  namespace Materials {
    /**String representation of materials section name in scene file
     *
     */
    const std::string MATERIALS_NAME = "materials";
  }

  namespace Lights {
    /**String representation of light section name in scene file
     *
     */
    const std::string LIGHTS_NAME = "lights";
  }
}

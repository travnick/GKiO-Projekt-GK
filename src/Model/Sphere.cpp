/// @file Main/Model/Sphere.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <cmath>

#include "Model/Sphere.h"
#include "Model/Ray.h"
#include "Model/PointAndVectorOperations.h"

namespace Model {

  Sphere::Sphere (worldUnit radius){
    size = radius;
    squareRadius = radius * radius;
  }

  Sphere::Sphere (const Sphere &other){
    squareRadius = other.squareRadius;
    VisibleObject::operator=(other);
  }

  bool Sphere::checkRay (const Ray &ray, worldUnit &range, Vector &e) const{
    PVOperations::diff(position.coords, ray.getStart().coords, e.coords);

    worldUnit a = PVOperations::dotProduct(ray.getDir().coords, e.coords);

    if ((a - size - INTERSECTION_ERROR_TOLERANCE) > range)
    {
      return false;
    }

    worldUnit D = squareRadius - PVOperations::dotProduct(e.coords) + a * a;

    //    if (D < 0.0f)
    //    {
    //      return false;
    //    }

    worldUnit t = SQRT(D);

    if (e.coords [PW] >= squareRadius)
    //We are outside sphere
    {
      t = a - t;
      if ((t > INTERSECTION_ERROR_TOLERANCE) && (t < range))
      {
        range = t;
        return true;
      }
    }
    else
    //We are inside sphere
    {
      t += a;

      if ((t > INTERSECTION_ERROR_TOLERANCE) && (t < range))
      {
        range = t;
        return true;
      }
    }

    return false;
  }

}

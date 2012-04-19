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

  void Sphere::getNormal (const Point& point, Vector &normalAtPoint) const{
    PVOperations::diff(point.coords, getPosition().coords, normalAtPoint.coords);
    normalAtPoint.normalize();
  }

  bool Sphere::checkRay (const Ray& ray, worldUnit& range, Vector& e) const{
    PVOperations::diff(position.coords, ray.getStart().coords, e.coords);
    worldUnit a = PVOperations::dotProduct(ray.getDir().coords, e.coords);
    if ((a - size) > range)
    {
      return false;
    }
    worldUnit D = squareRadius - PVOperations::dotProduct(e.coords) + a * a;

    worldUnit t = SQRT(D);

    if (e.coords [PW] >= squareRadius)
    //We are outside sphere
    {
      t = a - t;
      if ((t > 0.f) && (t < range))
      {
        range = t;
        return true;
      }
    }
    else
    //We are inside sphere
    {
      t += a;

      if ((t > 0.f) && (t < range))
      {
        range = t;
        return true;
      }
    }

    return false;
  }

}

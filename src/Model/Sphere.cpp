/// @file Model/Sphere.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <cmath>

#include "Model/Sphere.h"
#include "Model/Ray.h"

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
    point.data.diff(getPosition().data, normalAtPoint.data);
    normalAtPoint.normalize();
  }

  bool Sphere::checkRay (const Ray& ray, worldUnit& range, Vector& e) const{
    position.data.diff(ray.getStart().data, e.data);

    worldUnit a = ray.getDir().dotProduct(e.data);

    worldUnit D = squareRadius - e.dotProduct() + a * a;

    //There is no intersection with sphere if D < 0
    if (D - DEFAULT_INTERSECTION_ERROR_VALUE < 0)
    {
      return false;
    }

    worldUnit t = SQRT(D);

    if (e.squareLength >= squareRadius)
    //We are outside sphere
    {
      t = a - t;
    }
    else
    //We are inside sphere
    {
      t += a;
    }

    if ((t > 0.f) && (t < range))
    {
      range = t;
      return true;
    }

    return false;
  }

}

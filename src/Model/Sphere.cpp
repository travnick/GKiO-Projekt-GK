/// @file Model/Sphere.cpp

#include <cmath>

#include "Model/Ray.h"
#include "Model/Sphere.h"

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

  bool Sphere::checkRay (const Ray& ray, worldUnit& range, Vector& dist) const{
    position.data.diff(ray.getStart().data, dist.data);

    worldUnit a = ray.getDir().dotProduct(dist.data);

    worldUnit D = squareRadius - dist.dotProduct() + a * a;

    //There is no intersection with sphere if D < 0
    if (D - DEFAULT_INTERSECTION_ERROR_VALUE < 0)
    {
      return false;
    }

    worldUnit t = SQRT(D);

    if (dist.squareLength >= squareRadius)
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

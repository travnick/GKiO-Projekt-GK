/// @file Model/Sphere.cpp

#include <cmath>

#include "Model/Ray.h"
#include "Model/Sphere.h"

namespace Model
{

  Sphere::Sphere (worldUnit radius)
  {
    size = radius;
    squareRadius = radius * radius;
  }

  Sphere::Sphere (const Sphere &other)
  {
    squareRadius = other.squareRadius;
    VisibleObject::operator=(other);
  }

  void Sphere::getNormal (const Point& point, Vector &normalAtPoint) const
  {
    normalAtPoint = point.diff(getPosition());
    normalAtPoint.normalize();
  }

  bool Sphere::checkRay (const Ray& ray, worldUnit& range, Vector& dist) const
  {
    dist = position.diff(ray.getStart());

    worldUnit a = ray.getDir().dotProduct(dist);

    auto squareLength = dist.dotProduct();
    worldUnit D = squareRadius - squareLength + a * a;

    //There is no intersection with sphere if D < 0
    if (D < 0.f)
    {
      return false;
    }

    worldUnit t = SQRT(D);

    if (squareLength >= squareRadius)
    { //We are outside sphere
      a -= t;
    }
    else
    { //We are inside sphere
      a += t;
    }

    if ( (a > 0.0f) && (a < range))
    {
      range = a;
      return true;
    }

    return false;
  }

}

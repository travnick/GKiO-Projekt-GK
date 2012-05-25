/// @file Plane.cpp

#include "Model/Plane.h"
#include "Model/Ray.h"

const float ANGLE_ERROR_VALUE = 0.0001f;

using Model::Plane;

Plane::Plane ()
{
}

Plane::Plane (const Vector &newAngles)
    : angles(newAngles)
{
}

Plane::Plane (const Plane& other)
{
  angles = other.angles;
  normal = other.normal;
}

bool Plane::checkRay (const Ray& ray, worldUnit& range, Vector&) const
{

  worldUnit rayNormalDot = ray.getDir().dotProduct(normal.data);

  if (rayNormalDot > ANGLE_ERROR_VALUE || rayNormalDot < -ANGLE_ERROR_VALUE)
  //We are in front/back of plane
  {
    worldUnit raystartNormalDot = ray.getStart().data.dotProduct(normal.data);
    worldUnit t = (normal.length - raystartNormalDot) / rayNormalDot;

    if ( (t > 0.0f) && (t < range))
    {
      range = t;
      return true;
    }
  }

  return false;
}

void Plane::setAngles (const Vector &newAngles)
{
  angles = newAngles;
  calculateNormal();
}

void Plane::setNormal (const Vector &newNormal)
{
  normal = newNormal;
}

void Plane::calculateNormal ()
{
  normal.set(0, 1, 0);
  normal.rotate(angles);

  normal.normalize();

  normal.length = angles.length;
}

void Plane::getNormal (const Point &point, Vector &normalAtPoint) const
{
  normalAtPoint = normal;

  if (normal.data.dotProduct(point.data) - normal.length < 0)
  {
    normalAtPoint.data.negate();
  }
}

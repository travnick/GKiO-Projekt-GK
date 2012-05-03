/// @file Plane.cpp
/// @date 18-04-2012
/// @author Mikołaj Milej

#include "Model/Plane.h"
#include "Model/Ray.h"

using Model::Plane;

Plane::Plane (){
}

Plane::Plane (Vector newAngles)
    : angles(newAngles){
}

Plane::Plane (const Plane& other){
  angles = other.angles;
  normal = other.normal;
}

bool Plane::checkRay (const Ray& ray, worldUnit& range, Vector&) const{

  worldUnit rayNormalDot = ray.getDir().dotProduct(normal.data);

  if (rayNormalDot != 0)
  //We are in front/back of plane
  {

    worldUnit raystartNormalDot = ray.getStart().data.dotProduct(normal.data);
    worldUnit t = (normal.length - raystartNormalDot) / rayNormalDot;

    if ((t > 0.0f) && (t < range))
    {
      range = t;
      return true;
    }
  }

  return false;
}

void Plane::setAngles (Vector newAngles){
  angles = newAngles;
  calculateNormal();
}

void Plane::setNormal (Vector newNormal){
  normal = newNormal;
}

void Plane::calculateNormal (){
  normal = angles;
}

void Plane::getNormal (const Point&, Vector &normalAtPoint) const{
  normalAtPoint = normal;
}

/* namespace Model */

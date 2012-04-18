/// @file Plane.cpp
/// @date 18-04-2012
/// @author Mikołaj Milej

#include "Model/Plane.h"
#include "Model/Ray.h"
#include "Model/PointAndVectorOperations.h"

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

bool Plane::checkRay (const Ray& ray, worldUnit& range, Vector& tmpDist) const{

  worldUnit dn = PVOperations::dotProduct(ray.getDir().coords, normal.coords);

  if (dn < 0)
  //We are in front of plane
  {
    worldUnit t = (normal.coords [D]
        - PVOperations::dotProduct(ray.getStart().coords, normal.coords)) / dn;

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

void Plane::calculateNormal (){
  normal = angles;
}

/* namespace Model */

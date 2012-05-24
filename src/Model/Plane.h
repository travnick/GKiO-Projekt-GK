/// @file Plane.h

#pragma once

#include <Model/VisibleObject.h>
#include <Model/Vector.h>

namespace Model {

  /*Plane class
   *
   */
  class Plane: public Model::VisibleObject {
    public:

      /**Creates plane
       *
       */
      Plane ();

      /**Creates copy of another plane
       *
       * @param newAngles
       */
      Plane (const Vector &newAngles);

      /**Creates new plane and copies data from other
       *
       * @param other plane to copy data from
       */
      Plane (const Plane &other);

      /**Destructor of the plane
       *
       */
      inline virtual ~Plane () {
      }

      /**Set angles of the plane
       * It calculates normal of the plane
       *
       * @param newAngles angles of the plane
       */
      void setAngles (const Vector &newAngles);

      /**Set normal vector of the plane
       *
       * @param newNormal normal of the plane
       */
      void setNormal (const Vector &newNormal);

      /**Calculates normal vector at given point
       *
       * @param point point to calculate normal at
       * @param normalAtPoint vector to calculate normal in
       */
      virtual void getNormal (const Point& point, Vector &normalAtPoint) const;

      /**Checks if given ray intersects with plane
       *
       * @param ray ray to check intersection with
       * @param range range of given ray
       * @param tmpDist temporary ray for calculations
       * @return true if ray intersects with plane and plane is in ray range, otherwise false
       */
      virtual bool checkRay (const Ray &ray,
                             worldUnit &range,
                             Vector &tmpDist) const;

    private:
      Vector normal;
      Vector angles;

      /**Calculates normal vector of the plane
       *
       */
      void calculateNormal ();
  };

} /* namespace Model */

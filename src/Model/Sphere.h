/// @file Main/Model/Sphere.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/VisibleObject.h"

namespace Model {

  /**Sphere class
   *
   */
  class Sphere: public VisibleObject {
    public:
      /**Creates sphere with given radius.
       * Calculates square radius for future optimization
       *
       * @param radius radius of sphere
       */
      Sphere (worldUnit radius);

      /**Creates new sphere and copies data from other sphere
       *
       * @param other sphere to copy data from
       */
      Sphere (const Sphere &other);

      inline virtual ~Sphere (){
      }

      /**Checks if given ray intersects with sphere
       *
       * @param ray ray to check intersection with
       * @param range range of given ray
       * @param tmpDist temporary ray for calculations
       * @return true if ray intersects with sphere and sphere is in ray range, otherwise false
       */
      virtual bool checkRay (const Ray &ray, worldUnit &range, Vector &tmpDist) const;

    private:
      worldUnit squareRadius;

      /**Prevent creation of sphere without radius
       *
       */
      Sphere (){
      }
  };
}

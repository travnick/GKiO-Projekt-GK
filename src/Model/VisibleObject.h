/// @file Model/VisibleObject.h

#pragma once

#include "Model/Object.h"

namespace Model
{
  //Forward declarations -->
  class Point;
  class Ray;
  class Vector;
  // <-- Forward declarations

  /**Visible object class
   * Class for object which is visible on rendered image
   *
   */
  class VisibleObject: public Object
  {
    private:
      const Material *material;

    public:

      inline virtual ~VisibleObject ()
      {
      }

      /**Returns id of material that object has
       *
       * @return material id
       */
      inline const Material &getMaterial () const
      {
        return *material;
      }

      /**Returns approximated size of object
       *
       * @return size of object
       */
      inline worldUnit getSize () const
      {
        return size;
      }

      /**Calculates normal vector at given point
       *
       * @param point point to calculate normal at
       * @param normalAtPoint vector to calculate normal in
       */
      virtual void getNormal (const Point& point,
                              Vector &normalAtPoint) const = 0;

      /**Sets material of the object
       *
       * @param newMaterialId id of material to set
       */
      inline void setMaterial (const Material *newMaterialId)
      {
        material = newMaterialId;
      }

      /**All visible objects have to implement this function
       *
       * @param ray ray to check intersection with
       * @param range range of given ray
       * @param tmpDist temporary ray for calculations
       * @return true if ray intersects with object and object is in ray range, otherwise false
       */
      virtual bool checkRay (const Ray &ray,
                             worldUnit &range,
                             Vector &tmpDist) const = 0;

    protected:
      worldUnit size;
  };

} /* namespace Model */

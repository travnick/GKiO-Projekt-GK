/// @file Main/Model/VisibleObject.h
/// @date 27-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/Object.h"

namespace Model {
  //Forward declarations -->
  class Ray;
  class Vector;
  class Point;
  // <-- Forward declarations

  /**Visible object class
   * Class for object which is visible on rendered image
   *
   */
  class VisibleObject: public Object {
    public:
      inline virtual ~VisibleObject (){
      }

      /**Returns id of material that object has
       *
       * @return material id
       */
      inline const uint &getMaterial () const{
        return materialId;
      }

      /**Returns aproximated size of object
       *
       * @return size of object
       */
      inline const worldUnit &getSize () const{
        return size;
      }

      /**Calculates normal vector at given point
       *
       * @param point point to calculate normal at
       * @param normalAtPoint vector to calculate normal in
       */
      virtual void getNormal (const Point& point, Vector &normalAtPoint) const = 0;

      /**Sets material of the object
       *
       * @param materialId id of material to set
       */
      inline void setMaterial (uint newMaterialId){
        materialId = newMaterialId;
      }

      /**All visible objects have to implement this function
       *
       * @param ray ray to check intersection with
       * @param range range of given ray
       * @param tmpDist temporary ray for calculations
       * @return true if ray intersects with object and object is in ray range, otherwise false
       */
      virtual bool checkRay (const Ray &ray, worldUnit &range, Vector &tmpDist) const = 0;

    protected:
      worldUnit size;

    private:
      uint materialId;
  };

} /* namespace Model */

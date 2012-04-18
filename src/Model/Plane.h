/// @file Plane.h
/// @date 18-04-2012
/// @author Mikołaj Milej

#ifndef PLANE_H_
#define PLANE_H_

#include <VisibleObject.h>

namespace Model {

  /*
   *
   */
  class Plane: public Model::VisibleObject {
    public:
      enum normalParams {
        A = 0, B = 1, C = 2, D = 3
      };

      Plane ();

      Plane (Vector newAngles);

      /**Creates new plane and copies data from other
       *
       * @param other plane to copy data from
       */
      Plane (const Plane &other);

      inline virtual ~Plane (){
      }

      void setAngles (Vector newAngles);

      void setNormal (Vector newAngle);

      /**Checks if given ray intersects with plane
       *
       * @param ray ray to check intersection with
       * @param range range of given ray
       * @param tmpDist temporary ray for calculations
       * @return true if ray intersects with plane and plane is in ray range, otherwise false
       */
      virtual bool checkRay (const Ray &ray, worldUnit &range, Vector &tmpDist) const;

    private:
      Vector normal;
      Vector angles;

      void calculateNormal ();
  };

} /* namespace Model */
#endif /* PLANE_H_ */

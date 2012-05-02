/// @file Model/Object.h
/// @date 22-12-2011
/// @author Mikołaj Milej

#pragma once

#include <string>

#include "Model/ModelDefines.h"
#include "Model/Point.h"

namespace Model {

  /**Common object class
   *
   */
  class Object {
    public:
      /**Sets position of object in 3D space
       * Position is center of the object
       *
       * @param x position at x axis
       * @param y position at y axis
       * @param z position at z axis
       */
      inline void setPosition (Point::dataType x, Point::dataType y, Point::dataType z){
        position.set(x, y, z);
      }

      /**Copies position from other point in 3D space
       *
       * @param position other point in 3D space
       */
      inline void setPosition (const Point &newPosition){
        position = newPosition;
      }

      /**Returns position of the object
       *
       * @return object position
       */
      inline const Point & getPosition () const{
        return position;
      }

      /**Returns type of object given by name
       * If there's no such type then Objects::None is returned
       *
       * @param typeName type name
       * @return obejct type
       */
      static Objects::ObjectType getObjectType (const QString &typeName);

      /**Returns string representation of given object type
       * If type couldn't be resolved then empty string is returned
       *
       * @param object type
       * @return string representation of type
       */
      static const QString &getObjectTypeName (Objects::ObjectType);

    protected:
      Point position;
  };

} /* namespace Model */

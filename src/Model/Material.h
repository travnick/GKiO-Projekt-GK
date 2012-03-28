/// @file Main/Model/Material.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <QSharedPointer>
//Needed for worldUnit
#include "Model/ModelDefines.h"

namespace Model {
  //Forward declarations -->
  class Color;
  // <-- Forward declarations

  /**Material class
   *
   */
  class Material {
    public:
      /**Available types of material
       *
       */
      enum Type {
        Diffuse
      };

      /**Initializes color field
       *
       */
      Material ();

      inline const QSharedPointer <Color> &getColor () const{
        return diffuse;
      }

      inline const worldUnit &getReflection () const{
        return reflection;
      }

      inline void setReflection (const worldUnit &newReflection){
        if (newReflection > 1)
        {
          reflection = 1;
        }
        else
        {
          reflection = newReflection;
        }
      }

    private:
      QSharedPointer <Color> diffuse;
      worldUnit reflection;
      Type type;
  };
}

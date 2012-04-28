/// @file Main/Model/Material.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/Color.h"
//Needed for worldUnit
#include "Model/ModelDefines.h"

namespace Model {
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
      inline Material (){

      }

      inline const Color &getColor () const{
        return diffuse;
      }

      inline void setColor (const Color &other){
        diffuse = other;
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

      static void* operator new (size_t rozmiar){
        return _mm_malloc(rozmiar, 16);
      }

    private:
      Color diffuse;
      worldUnit reflection;
      Type type;
  };
}

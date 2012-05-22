/// @file Model/Material.h

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

      worldUnit getIOR () const{
        return ior;
      }

      void setIOR (worldUnit val){
        this->ior = val;
      }

      worldUnit getTransparency () const{
        return transparency;
      }

      void setTransparency (worldUnit val){
        this->transparency = val;
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
      Color diffuse;
      worldUnit reflection;
      worldUnit transparency;
      worldUnit ior;
      Type type;
  };
}

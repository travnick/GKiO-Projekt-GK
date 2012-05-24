/// @file Model/Material.h

#pragma once

#include "Model/Color.h"

namespace Model {
  /**Material class
   *
   */
  class Material {
    public:
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

      inline const Color &getSpecularColor () const{
        return specularColor;
      }

      inline void setSpecularColor (const Color &color){
        this->specularColor = color;
      }

      inline float getSpecularPower () const{
        return specularPower;
      }

      inline void setSpecularPower (float val){
        this->specularPower = val;
      }

      inline float getReflection () const{
        return reflection;
      }

      float getIOR () const{
        return ior;
      }

      void setIOR (float val){
        this->ior = val;
      }

      float getTransparency () const{
        return transparency;
      }

      void setTransparency (float val){
        this->transparency = val;
      }

      inline void setReflection (float val){
        reflection = val > 1 ? 1 : val;
      }

    private:
      Color diffuse;
      Color specularColor;
      float ior;
      float reflection;
      float specularPower;
      float transparency;
  };
}

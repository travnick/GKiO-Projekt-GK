/// @file Model/Material.h

#pragma once

#include "Model/Color.h"

namespace Model
{
  /**Material class
   *
   */
  class Material
  {
    public:
      inline Material ()
      {
      }

      /** Returns diffuse color of the material
       *
       * @return diffuse color
       */
      inline const Color &getColor () const
      {
        return diffuse;
      }

      /** Sets diffuse color of the material
       *
       * @param other diffuse color
       */
      inline void setColor (const Color &other)
      {
        diffuse = other;
      }

      /** Returns specular color of the material
       *
       * @return specular color
       */
      inline const Color &getSpecularColor () const
      {
        return specularColor;
      }


      /** Sets specular color of the material
       *
       * @param color specular color
       */
      inline void setSpecularColor (const Color &color)
      {
        this->specularColor = color;
      }

      /** Returns power of specular color of the material
       *
       * @return power of specular color
       */
      inline float getSpecularPower () const
      {
        return specularPower;
      }


      /** Sets power of specular color of the material
       *
       * @param val power of specular color
       */
      inline void setSpecularPower (float val)
      {
        this->specularPower = val;
      }

      /** Returns the index of refraction
       *
       * @return index of refraction
       */
      float getIOR () const
      {
        return ior;
      }

      /** Sets the index of refraction
       *
       * @param val index of refraction
       */
      void setIOR (float val)
      {
        this->ior = val;
      }

      /** Returns the transparency factor of the material
       *
       * @return transparency
       */
      float getTransparency () const
      {
        return transparency;
      }

      /** Sets a transparency of material
       *
       * @param val transparency of material
       */
      void setTransparency (float val)
      {
        this->transparency = val;
      }

      /** Returns the reflection factor of the material
       *
       * @return reflection
       */
      inline float getReflection () const
      {
        return reflection;
      }

      /** Sets a reflection factor of material
         *
         * @param val transparency of material
         */
      inline void setReflection (float val)
      {
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

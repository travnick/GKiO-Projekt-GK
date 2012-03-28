/// @file Main/Model/Light.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include "Model/Object.h"
#include "Model/Color.h"

namespace Model {

  /**Light class
   *
   */
  class Light: public Object, public Color {
    public:
      Light (){
      }

      /**Sets color of light
       *
       * @return this
       */
      inline Light &operator= (const Color &color){
        Color::operator =(color);
        return *this;
      }

      /**Returns color of light
       *
       * @return color of light
       */
      inline const Color &getColor () const{
        return *this;
      }
  };
}

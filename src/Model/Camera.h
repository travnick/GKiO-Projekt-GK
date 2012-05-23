/// @file Model/Camera.h

#pragma once

#include "Model/Vector.h"
#include "Model/Object.h"

#define MAX_VIEW_ANGLE 180

namespace Model {
  //Forward declarations -->
  class Point;
  // <-- Forward declarations

  /**Camera class
   *
   */
  class Camera: public Object {
    public:
      typedef double unitType;

      /**Available camera types
       *
       */
      enum Type {
        None, Orthogonal, Conic
      };

      /**Initializes fields
       *
       */
      Camera ();

      /**Returns current camera type
       *
       * @return current camera type
       */
      inline const Camera::Type & getType () const{
        return type;
      }

      /**Returns current camera direction
       *
       * @return current camera direction
       */
      inline const Vector &getDirection () const{
        return eyeDirection;
      }

      /**Returns direction from camera origin to given point
       *
       * @param point point in 3D space
       * @param direction direction from camera origin to given point
       */
      inline void getDirection (const Point &point,
                                Vector &directionToPoint) const{
        Q_ASSERT(screenImageRatio > 0);

        point.data.diff(origin.data, directionToPoint.data);

        directionToPoint.normalize();
      }

      /**Returns ratio of screen width to image width
       * "screenWidth/imageWidth"
       *
       * @return ratio of screen width to image width
       */
      inline const worldUnit &getScreenImageWidthRatio () const{
        return screenImageRatio;
      }

      /**Returns top left corner of camera screen in 3D space
       *
       * @return top left corner of camera screen
       */
      inline const Point &getScreenTopLeft () const{
        return screenTopLeft;
      }

      /**Returns camera screen height
       *
       * @return camera screen height
       */
      inline const worldUnit &getScreenHeight () const{
        return screenHeight;
      }

      /**Returns camera screen width
       *
       * @return camera screen width
       */
      inline const worldUnit &getScreenWidth () const{
        return screenWidth;
      }

      /**Returns current camera view distance
       *
       * @return camera view distance
       */
      inline const worldUnit &getViewDistance () const{
        return viewDistance;
      }

      /**Sets Field of View
       * FOV should be in range 0 < FOV < 180
       * FOV is decremented by 180 until FOV < 180
       * If FOV < 0 then FOV = 0
       *
       * @param FOV new Field of View
       */
      inline void setFOV (unitType newFOV){
        if (newFOV < 0)
        {
          newFOV = 0;
        }
        //No angle >= MAX_VIEW_ANGLE
        while (newFOV >= MAX_VIEW_ANGLE)
        {
          newFOV -= MAX_VIEW_ANGLE;
        }
        FOV = newFOV;
      }

      /**Returns FOV of camera
       *
       * @return FOV
       */
      inline unitType getFOV (){
        return FOV;
      }

      /**Calibrates camera
       * It calculates screen position and dimensions.
       * Also calculates position of camera origin.
       *
       * It has to be called after setting image width and FOV.
       *
       */
      void calibrate ();

      /**Sets screen width
       *
       * @param screenWidth new screen width
       */
      inline void setScreenWidth (worldUnit newScreenWidth){
        screenWidth = newScreenWidth;
      }

      /**Sets image width
       *
       * @param imageWidth new image width
       */
      inline void setImageWidth (imageUnit newImageWidth){
        imageWidth = newImageWidth;
      }

      /**Sets image height
       *
       * @param imageHeight
       */
      inline void setImageHeight (imageUnit newImageHeight){
        imageHeight = newImageHeight;
      }

      /**Sets camera type
       *
       * @param typeName new camera type
       */
      inline void setType (Type typeName){
        type = typeName;
      }

      /**Sets camera type
       * It takes camera name represented by QString
       *
       * @param typeName camera type name
       */
      void setType (const QString &typeName);

      /**Sets camera direction
       *
       * @param vector camera direction
       */
      void setDirection (const Vector &vector);

      /**Sets camera rotation parameters
       *
       * @param vector
       */
      void setAngles (const Vector &vector);

      /**Updates camera rotation
       *
       */
      void updateRotation ();

      const Vector &getAngles () const{
        return angles;
      }

      Vector &getAngles (){
        return angles;
      }

      /**Sets camera view distance
       *
       * @param viewDistance new view distance
       */
      inline void setViewDistance (const worldUnit &newViewDistance){
        viewDistance = newViewDistance;
      }

      void move (Axis direction, float speed);

      void rotate (Axis axis, float angle);

      Vector screenWidthDelta;
      Vector screenHeightDelta;
    private:
      unitType FOV;
      worldUnit viewDistance;
      imageUnit imageWidth;
      imageUnit imageHeight;
      worldUnit screenWidth;
      worldUnit screenHeight;
      worldUnit screenImageRatio;
      Type type;
      Vector eyeDirection;
      Vector angles;
      Point origin;
      Point screenTopLeft;
      Point screenTopRight;
      Point screenBottomLeft;
  };
}


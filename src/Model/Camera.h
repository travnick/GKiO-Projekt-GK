/// @file Model/Camera.h

#pragma once

#include "Model/Object.h"
#include "Model/Vector.h"

#define MAX_VIEW_ANGLE 180

namespace Model
{

  /**Camera class
   *
   */
  class Camera: public Object
  {
    public:
      typedef double unitType;

      /**Available camera types
       *
       */
      enum Type
      {
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
      inline const Camera::Type & getType () const
      {
        return type;
      }

      /**Returns current camera direction
       *
       * @return current camera direction
       */
      inline const Vector &getDirection () const
      {
        return eyeDirection;
      }

      /**Returns direction from camera origin to given point
       * on the projection plane
       *
       * @param point point in 3D space
       * @param directionToPoint direction from camera origin to given point
       */
      inline void getDirection (const Point &point,
                                Vector &directionToPoint) const
      {
        Q_ASSERT(screenImageRatio > 0);

        point.data.diff(origin.data, directionToPoint.data);

        directionToPoint.normalize();
      }

      /**Returns ratio of screen width to image width
       * "screenWidth/imageWidth"
       *
       * @return ratio of screen width to image width
       */
      inline const worldUnit &getScreenImageWidthRatio () const
      {
        return screenImageRatio;
      }

      /**Returns top left corner of camera screen in 3D space
       *
       * @return top left corner of camera screen
       */
      inline const Point &getScreenTopLeft () const
      {
        return screenTopLeft;
      }

      /**Returns camera screen height
       *
       * @return camera screen height
       */
      inline const worldUnit &getScreenHeight () const
      {
        return screenHeight;
      }

      /**Returns camera screen width
       *
       * @return camera screen width
       */
      inline const worldUnit &getScreenWidth () const
      {
        return screenWidth;
      }

      /**Returns current camera view distance
       *
       * @return camera view distance
       */
      inline const worldUnit &getViewDistance () const
      {
        return viewDistance;
      }

      /**Sets Field of View
       * FOV should be in range 0 < FOV < 180
       * FOV is decremented by 180 until FOV < 180
       * If FOV < 0 then FOV = 0
       *
       * @param newFOV new Field of View
       */
      inline void setFOV (unitType newFOV)
      {
        //FOV has to be <= MAX_VIEW_ANGLE
        while (newFOV >= MAX_VIEW_ANGLE)
        {
          newFOV -= MAX_VIEW_ANGLE;
        }

        //FOV has to be > 0
        FOV = newFOV < 0 ? 0 : newFOV;
      }

      /**Returns FOV of camera
       *
       * @return FOV
       */
      inline unitType getFOV ()
      {
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
       * @param newScreenWidth new screen width
       */
      inline void setScreenWidth (worldUnit newScreenWidth)
      {
        screenWidth = newScreenWidth;
      }

      /**Sets image width
       *
       * @param newImageWidth new image width
       */
      inline void setImageWidth (imageUnit newImageWidth)
      {
        imageWidth = newImageWidth;
      }

      /**Sets image height
       *
       * @param newImageHeight
       */
      inline void setImageHeight (imageUnit newImageHeight)
      {
        imageHeight = newImageHeight;
      }

      /**Sets camera type
       *
       * @param typeName new camera type
       */
      inline void setType (Type typeName)
      {
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


      /** Return angles of camera
       *
       * @return angles of camera
       */
      const Vector &getAngles () const
      {
        return angles;
      }

      /** Return angles of camera
       *
       * @return angles of camera
       */
      Vector &getAngles ()
      {
        return angles;
      }

      /**Sets camera view distance
       *
       * @param newViewDistance new view distance
       */
      inline void setViewDistance (const worldUnit &newViewDistance)
      {
        viewDistance = newViewDistance;
      }

      /**
       *  Moves camera along specified axis by "speed" distance
       *
       *  @param direction direction of movement
       *  @param speed distance that camera will travel
       */
      void move (Axis direction, float speed);

      /**
       * Rotates camera along specified axis by angle
       *
       * @param axis axis of rotation
       * @param angle angle of rotation
       */
      void rotate (Axis axis, float angle);

      //Vectors used for moving on the projection plane
      Vector screenHeightDelta;
      Vector screenWidthDelta;
    private:
      /**Rotation angles
       *
       */
      Vector angles;
      Vector eyeDirection;
      Point origin;
      Point screenTopLeft;
      Point screenTopRight;
      Point screenBottomLeft;
      unitType FOV;
      imageUnit imageHeight;
      imageUnit imageWidth;
      worldUnit screenHeight;
      worldUnit screenWidth;
      worldUnit screenImageRatio;
      worldUnit viewDistance;
      Type type;
  };
}


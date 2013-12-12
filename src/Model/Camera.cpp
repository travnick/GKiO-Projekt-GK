/// @file Model/Camera.cpp

#include <cmath>
#include <map>

#include "Model/Camera.h"
#include "Model/Vector.h"

namespace Model
{

  typedef std::map <QString, Camera::Type> cameraTypesMap;

  Camera::Camera ()
  {
    screenImageRatio = 0;
    imageWidth = 0;
    screenWidth = 0;
  }

  void Camera::calibrate ()
  {
    Q_ASSERT(imageWidth > 0);

    //Calculate focal length of camera
    worldUnit angle = (180 - FOV) / 2.0f;
    worldUnit focalLength = tan(RAD(angle)) * screenWidth / 2;

    //Calculate origin of camera
    eyeDirection.normalize();
    origin = position;
    Vector tmpVector = eyeDirection.multiply(focalLength);
    origin = origin.negMove(tmpVector);

    screenImageRatio = screenWidth / imageWidth;
    screenHeight = imageHeight * screenImageRatio;

    unitType halfHeight = screenHeight / 2;
    unitType halfWidth = screenWidth / 2;

    screenTopLeft.set(-halfWidth, halfHeight, 0);
    screenTopRight.set(halfWidth, halfHeight, 0);
    screenBottomLeft.set(-halfWidth, -halfHeight, 0);

    screenTopLeft.rotate(angles);
    screenTopRight.rotate(angles);
    screenBottomLeft.rotate(angles);

    screenTopLeft += position;
    screenTopRight += position;
    screenBottomLeft += position;

    //Vectors used for moving on the projection plane
    screenWidthDelta = screenTopRight - screenTopLeft;
    screenHeightDelta = screenBottomLeft - screenTopLeft;

    screenWidthDelta *= 1.0f / imageWidth;
    screenHeightDelta *= 1.0f / imageHeight;
  }

  void Camera::setType (const QString & typeName)
  {
    static cameraTypesMap cameraTypes;
    cameraTypesMap::iterator iterator;
    type = Camera::None;

    if (cameraTypes.empty())
    {
      cameraTypes [Cameras::CONIC] = Camera::Conic;
      cameraTypes [Cameras::ORTHOGONAL] = Camera::Orthogonal;
    }

    iterator = cameraTypes.find(typeName);

    if (iterator != cameraTypes.end())
    {
      type = iterator->second;
    }
  }

  void Camera::setDirection (const Vector &vector)
  {
    eyeDirection = vector;
  }

  void Camera::setAngles (const Vector& vector)
  {
    angles = vector;
    updateRotation();
  }

  void Camera::updateRotation ()
  {
    Point positionOrig(position);

    position.set(0, 0, 0);

    //look at the z+ axis
    eyeDirection.set(0, 0, 1);
    eyeDirection.rotate(angles);

    position = positionOrig;
  }

  void Camera::move (Axis direction, float speed)
  {
    switch (direction)
    {
      case X:
        position += screenWidthDelta.toNormal() * speed;
        break;

      case Y:
        position += screenHeightDelta.toNormal() * speed;
        break;

      case Z:
        position += eyeDirection.toNormal() * speed;
        break;

      case W:
        break;
    }
  }

  void Camera::rotate (Axis axis, float angle)
  {
    switch (axis)
    {
      case X:
        angles [X] += angle;
        break;

      case Y:
        angles [Y] += angle;
        break;

      case Z:
        angles [Z] += angle;
        break;

      case W:
        break;
    }

    updateRotation();
  }
}

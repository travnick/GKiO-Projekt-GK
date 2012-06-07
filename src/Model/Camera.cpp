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
    Vector tmpVector;
    QMatrix4x4 transformMatrix;

    worldUnit angle = (180 - FOV) / 2.0f;
    worldUnit focalLength = tan(RAD(angle)) * screenWidth / 2;

    eyeDirection.normalize();
    origin = position;
    eyeDirection.data.multiply(focalLength, tmpVector.data);
    origin.data.negMove(tmpVector.data, origin.data);

    screenImageRatio = screenWidth / imageWidth;
    screenHeight = imageHeight * screenImageRatio;

    unitType halfHeight = screenHeight / 2;
    unitType halfWidth = screenWidth / 2;

    screenTopLeft.set( -halfWidth, halfHeight, 0);
    screenTopRight.set(halfWidth, halfHeight, 0);
    screenBottomLeft.set( -halfWidth, -halfHeight, 0);

    screenTopLeft.rotate(angles);
    screenTopRight.rotate(angles);
    screenBottomLeft.rotate(angles);

    screenTopLeft.data += position;
    screenTopRight.data += position;
    screenBottomLeft.data += position;

    screenWidthDelta.data = screenTopRight.data - screenTopLeft.data;
    screenHeightDelta.data = screenBottomLeft.data - screenTopLeft.data;

    screenWidthDelta.data *= 1.0f / imageWidth;
    screenHeightDelta.data *= 1.0f / imageHeight;
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

    position.data = positionOrig.data;
  }

  void Camera::move (Axis direction, float speed)
  {
    switch (direction)
    {
      case X:
        position.data += screenWidthDelta.toNormal().data * speed;
        break;

      case Y:
        position.data += screenHeightDelta.toNormal().data * speed;
        break;

      case Z:
        position.data += eyeDirection.toNormal().data * speed;
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

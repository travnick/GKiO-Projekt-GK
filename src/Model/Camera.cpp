/// @file Model/Camera.cpp
/// @date 01-12-2011
/// @author Mikołaj Milej

#include <cmath>
#include <map>

#include "Model/Camera.h"
#include "Model/Vector.h"
#include "Model/PointAndVectorOperations.h"

namespace Model {

  typedef std::map <QString, Camera::Type> cameraTypesMap;

  Camera::Camera ()
      : direction(new Vector), origin(new Point), screenTopLeft(new Point), screenBottomRight(
          new Point){
    screenImageRatio = 0;
    imageWidth = 0;
    screenWidth = 0;
  }

  void Camera::calibrate (){
    Q_ASSERT(imageWidth > 0);
    Vector tmpVector;

    worldUnit angle = (180 - FOV) / 2.0f;
    worldUnit focalLength = tan(RAD(angle)) * screenWidth / 2;

    direction->normalize();
    origin.reset(new Point(position));
    PVOperations::multiply(focalLength, direction->data, tmpVector.data);
    PVOperations::negMove(origin->data, tmpVector.data, origin->data);

    screenImageRatio = screenWidth / imageWidth;
    screenHeight = imageHeight * screenImageRatio;

    unitType halfHeight = screenHeight / 2;
    unitType halfWidth = screenWidth / 2;

    screenTopLeft->set(position.data.x() - halfWidth, position.data.y() - halfHeight,
                       position.data.z());
    screenBottomRight->set(position.data.x() + halfWidth, position.data.y() + halfHeight,
                           position.data.z());
  }

  void Camera::setType (const QString & typeName){
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

  void Camera::setDirection (const Vector &vector){
    *direction = vector;
  }
}

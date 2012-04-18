/// @file UsableCpp/include/PointAndVectorOperations.h
/// @date 01-12-2011
/// @author Mikołaj Milej

#pragma once

#include <cmath>
#include <Model/ModelDefines.h>
#include <Model/VpCommon.h>

namespace Model {
  namespace PVOperations {
    /**Moves point by a vector and save new point in result
     * You should give here VPCommon::coords
     *
     * @param point worldUnit[4]
     * @param vector worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void move (const worldUnit *point, const worldUnit *vector, worldUnit *result){
      result [PX] = point [PX] + vector [PX];
      result [PY] = point [PY] + vector [PY];
      result [PZ] = point [PZ] + vector [PZ];
    }

    /**Moves point by a reversed vector and save new point in result
     * You should give here VPCommon::coords
     *
     * @param point worldUnit[4]
     * @param vector worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void negMove (const worldUnit *point, const worldUnit *vector, worldUnit *result){
      result [PX] = point [PX] - vector [PX];
      result [PY] = point [PY] - vector [PY];
      result [PZ] = point [PZ] - vector [PZ];
    }

    /**Calculates distance between two points
     * You should give here VPCommon::coords
     *
     * @param point1 worldUnit[4]
     * @param point2 worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void diff (const worldUnit *point1, const worldUnit *point2, worldUnit *result){
      result [PX] = point1 [PX] - point2 [PX];
      result [PY] = point1 [PY] - point2 [PY];
      result [PZ] = point1 [PZ] - point2 [PZ];
      result [PW] = -1;
    }

    /**Adds two vectors
     * You should give here VPCommon::coords
     *
     * @param vector1 worldUnit[4]
     * @param vector2 worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void addVectors (const worldUnit *vector1, const worldUnit *vector2, worldUnit *result){
      result [PX] = vector1 [PX] + vector2 [PX];
      result [PY] = vector1 [PY] + vector2 [PY];
      result [PZ] = vector1 [PZ] + vector2 [PZ];
      result [PW] = -1;
    }

    /**Subtracts two vectors
     * You should give here VPCommon::coords
     *
     * @param vector1 worldUnit[4]
     * @param vector2 worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void subVectors (const worldUnit *vector1, const worldUnit *vector2, worldUnit *result){
      result [PX] = vector1 [PX] - vector2 [PX];
      result [PY] = vector1 [PY] - vector2 [PY];
      result [PZ] = vector1 [PZ] - vector2 [PZ];
      result [PW] = -1;
    }

    /**Multiplies vector by a constant
     * You should give here VPCommon::coords
     *
     * @param constant worldUnit
     * @param vector worldUnit[4]
     * @param result worldUnit[4]
     */
    inline void multiply (const worldUnit &constant, const worldUnit *vector, worldUnit *result){
      result [PX] = vector [PX] * constant;
      result [PY] = vector [PY] * constant;
      result [PZ] = vector [PZ] * constant;
      result [PW] = -1;
    }

    /**Calculates dot product of two vectors
     * You should give here VPCommon::coords
     *
     * dotProduct = vector.x * vector2.x + vector.y * vector2.y + vector.z + vector2.z
     *
     * @param vector1 worldUnit[4]
     * @param vector2 worldUnit[4]
     * @return dotProduct
     */
    inline worldUnit dotProduct (const worldUnit *vector1, const worldUnit *vector2){
      return vector1 [PX] * vector2 [PX] + vector1 [PY] * vector2 [PY] + vector1 [PZ] * vector2 [PZ];
    }

    /**Calculates dot product of vector
     * You should give here VPCommon::coords
     *
     * dotProduct = vector.x * vector.x + vector.y * vector.y + vector.z + vector.z
     *
     * @param vector1 worldUnit[4]
     * @return dotProduct
     */
    inline worldUnit dotProduct (worldUnit *vector1){
      return vector1 [PW] = vector1 [PX] * vector1 [PX] + vector1 [PY] * vector1 [PY]
          + vector1 [PZ] * vector1 [PZ];
    }

    /**Calculate lenght of vector
     *
     * @param vector vector to calculate length of
     */
    inline worldUnit length (worldUnit *vector){
      return vector [PW] = SQRT(dotProduct(vector));
    }

    /**Normalizes vector
     *
     * @param vector vector to normalize
     */
    inline void normalize (worldUnit *vector){

      worldUnit constant = 1.0f / length(vector);

      vector [PX] *= constant;
      vector [PY] *= constant;
      vector [PZ] *= constant;
    }
  }
}

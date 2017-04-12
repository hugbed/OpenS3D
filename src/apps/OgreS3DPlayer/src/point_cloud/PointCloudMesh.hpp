//
// Created by jon on 29/03/17.
//

#ifndef POINT_CLOUD_POINTCLOUDMESH_H
#define POINT_CLOUD_POINTCLOUDMESH_H

#include <OgreHardwareVertexBuffer.h>

/** Class for visualisation of pointclouds.
 * The class can be intialized with a array of points organized like:
 * [0]=P1x [1]=P1y [2]=P1z
 * [3]=P2x [4]=P2y [5]=P2z ...
 */
class PointCloudMesh {
 public:
  PointCloudMesh(const std::string& name,
                 const std::string& resourcegroup,
                 const std::vector<float>& points,
                 const std::vector<float>& colors);

  /// Update a created pointcloud with size points.
  void updateVertexPositions(const std::vector<float>& points);

  /// Update vertex colours
  void updateVertexColors(const std::vector<float>& colors);

 private:
  Ogre::HardwareVertexBufferSharedPtr vbuf_;
  Ogre::HardwareVertexBufferSharedPtr cbuf_;
};

#endif  // POINT_CLOUD_POINTCLOUDMESH_H

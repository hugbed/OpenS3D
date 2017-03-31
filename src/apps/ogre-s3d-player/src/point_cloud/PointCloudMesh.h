//
// Created by jon on 29/03/17.
//

#ifndef OGRE_S3D_PLAYER_OVISEPOINTCLOUD_H
#define OGRE_S3D_PLAYER_OVISEPOINTCLOUD_H

#include <OgreHardwareVertexBuffer.h>

/** Class for visualisation of pointclouds.
 * The class can be intialized with a array of points organized like:
 * [0]=P1x [1]=P1y [2]=P1z
 * [3]=P2x [4]=P2y [5]=P2z ...
 */
class PointCloudMesh
{
public:
    PointCloudMesh(const std::string& name, const std::string& resourcegroup,
                    const std::vector<float> &points, const std::vector<float> &colors);

    /// Update a created pointcloud with size points.
    void updateVertexPositions(int size, const std::vector<float> &points);

    /// Update vertex colours
    void updateVertexColours(int size, const std::vector<float> &colors);

    virtual ~PointCloudMesh();

private:
    int mSize;
    Ogre::HardwareVertexBufferSharedPtr vbuf;
    Ogre::HardwareVertexBufferSharedPtr cbuf;
};


#endif //OGRE_S3D_PLAYER_OVISEPOINTCLOUD_H

//
// Created by jon on 29/03/17.
//

#include "PointCloudMesh.hpp"
#include "OgreMesh.h"
#include "OgreMeshManager.h"
#include "OgreRoot.h"
#include "OgreSubMesh.h"
#include <OgreHardwareBufferManager.h>

PointCloudMesh::PointCloudMesh(const std::string& /*name*/,
                               const std::string& resourcegroup,
                               const std::vector<float>& points,
                               const std::vector<float>& colors) {
  const auto numpoints = points.size() / 3;

  /// Create the mesh via the MeshManager
  Ogre::MeshPtr msh =
      Ogre::MeshManager::getSingleton().createManual("yobitch", resourcegroup);

  /// Create one submesh
  Ogre::SubMesh* sub = msh->createSubMesh();

  /// Create vertex data structure for vertices shared between submeshes
  msh->sharedVertexData = new Ogre::VertexData();
  msh->sharedVertexData->vertexCount = numpoints;

  /// Create declaration (memory format) of vertex data
  Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
  decl->addElement(0, 0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

  vbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
      Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3),
      msh->sharedVertexData->vertexCount,
      Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
  /// Upload the vertex data to the card
  vbuf->writeData(0, vbuf->getSizeInBytes(), &points[0], true);

  if (!colors.empty()) {
    // Create 2nd buffer for colors
    decl->addElement(1, 0, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
    cbuf = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
        Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR),
        msh->sharedVertexData->vertexCount,
        Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

    std::vector<Ogre::RGBA> colorsRGBA;
    colorsRGBA.resize(numpoints);

    for (int i = 0, k = 0; k < numpoints; i += 3, k++) {
      // Use render system to convert colour value since colour packing varies
      rs->convertColourValue(
          Ogre::ColourValue(colors[i], colors[i + 1], colors[i + 2]),
          &colorsRGBA[k]);
    }

    // Upload colour data
    cbuf->writeData(0, cbuf->getSizeInBytes(), &colorsRGBA[0], true);
  }

  /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
  Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
  bind->setBinding(0, vbuf);

  if (!colors.empty()) {
    // Set colour binding so buffer 1 is bound to colour buffer
    bind->setBinding(1, cbuf);
  }

  sub->useSharedVertices = true;
  sub->operationType = Ogre::RenderOperation::OT_POINT_LIST;

  // todo: set better bound than this
  msh->_setBounds(Ogre::AxisAlignedBox(-1, -1, -1, 1, 1, 1));
  msh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3 * 1 * 1));

  msh->load();
}

void PointCloudMesh::updateVertexPositions(int size,
                                           const std::vector<float>& points) {
  float* pPArray =
      static_cast<float*>(vbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  // std::copy(std::begin(points), std::end(points), std::begin(pPArray));
  for (int i = 0; i < size * 3; i += 3) {
    pPArray[i] = points[i];
    pPArray[i + 1] = points[i + 1];
    pPArray[i + 2] = points[i + 2];
  }
  vbuf->unlock();
}

void PointCloudMesh::updateVertexColors(int size,
                                        const std::vector<float>& colors) {
  float* pCArray =
      static_cast<float*>(cbuf->lock(Ogre::HardwareBuffer::HBL_DISCARD));

  // std::copy(std::begin(colors), std::end(colors), std::begin(pCArray));
  for (int i = 0; i < size * 3; i += 3) {
    pCArray[i] = colors[i];
    pCArray[i + 1] = colors[i + 1];
    pCArray[i + 2] = colors[i + 2];
  }
  cbuf->unlock();
}

PointCloudMesh::~PointCloudMesh() = default;

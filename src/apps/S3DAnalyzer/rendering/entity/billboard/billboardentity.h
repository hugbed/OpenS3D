#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>

#include "rendering/entity/entity.h"
#include "rendering/vertex.h"

#include <memory>
#include <vector>

class BillboardEntity : public Entity, QOpenGLFunctions {
 public:
  explicit BillboardEntity(const QSize& imageSize);
  ~BillboardEntity() override;

  void init();
  virtual void setHorizontalShift(float shift);
  void setAspectRatio(float ratio);
  virtual void setDisplayRange(float minX, float maxX, float minY, float maxY);
  void setPointToScreen(const QMatrix3x3 pointToScreen);
  void clear();

  void draw(QPaintDevice* /*paintDevice*/) override;

 protected:
  std::unique_ptr<QOpenGLShaderProgram> m_program;
  QOpenGLBuffer m_vertex;
  QOpenGLVertexArrayObject m_object;
  std::vector<Vertex> m_vertices;

  virtual void setDefaultUniforms();

 private:
  virtual void addShaders() = 0;
  virtual void setDefaultAttributeBuffers() = 0;

  void createShader();
  void createAndBindBuffer();
  void createVertexArrayObject();
  void releaseAll();

  // todo: necessary this image size?
  QSize m_imageSize;
};

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H

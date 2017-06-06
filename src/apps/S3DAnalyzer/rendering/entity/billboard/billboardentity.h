#ifndef RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H
#define RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "rendering/vertex.h"
#include "rendering/entity/entity.h"

#include <memory>
#include <vector>

class BillboardEntity : public Entity, QOpenGLFunctions {
 public:
  explicit BillboardEntity(const QSize& imageSize);
  ~BillboardEntity() override;

  void init();
  void setHorizontalShift(float shift);
  void setAspectRatio(float ratio);
  void setImageSize(const QSize& imageSize);
  void clear();

  void draw() override;

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

  QSize m_imageSize;
};

#endif  // RENDERING_ENTITY_BILLBOARD_BILLBOARDENTITY_H

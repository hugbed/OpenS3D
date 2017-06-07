#ifndef RENDERING_ENTITY_STEREO_RECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_RECTANGLEENTITY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "rendering/vertex.h"
#include "rendering/entity/entity.h"
#include "stereoimageentity.h"

#include <memory>

class RectangleEntity : public StereoImageEntity, QOpenGLFunctions {
 public:
  static const int NB_VERTICES = 6;

  RectangleEntity();
  ~RectangleEntity() override;

  void init() override;
  void setHorizontalShift(float shift) override;
  void setTextureLeft(QOpenGLTexture* texture) override;
  void setTextureRight(QOpenGLTexture* texture) override;
  void setAspectRatio(float ratio) override;
  void draw() override;

  virtual void addShaders();
  virtual void setDefaultUniforms();

  void init(float left, float top, float right, float bottom);
  void setCorners(float left, float top, float right, float bottom);
  void setTexture(int index, QOpenGLTexture* texture);

 protected:
  std::unique_ptr<QOpenGLShaderProgram> m_program;

 private:
  static std::vector<Vertex> fullscreenVertices();
  static std::vector<Vertex> cornersVertices(float left = -1.0f,
                                             float top = 1.0f,
                                             float right = 1.0f,
                                             float bottom = -1.0f);

  void init(const std::vector<Vertex>& vertices);
  void createShader();
  void createBuffer(const std::vector<Vertex>& vertices);
  void createVertexArrayObject();
  void releaseAll();

  // OpenGL State Information
  QOpenGLBuffer m_vertex;
  QOpenGLVertexArrayObject m_object;
  std::map<int, QOpenGLTexture*> m_textures;
  std::vector<Vertex> m_vertices;
};

#endif  // RENDERING_ENTITY_STEREO_RECTANGLEENTITY_H

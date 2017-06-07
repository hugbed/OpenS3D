#ifndef S3D_ANALYZER_RENDERING_VERTEX_H
#define S3D_ANALYZER_RENDERING_VERTEX_H

#include <QVector3D>

class Vertex {
 public:
  // Constructors
  Q_DECL_CONSTEXPR Vertex();
  Q_DECL_CONSTEXPR explicit Vertex(const QVector3D& position);
  Q_DECL_CONSTEXPR Vertex(const QVector3D& position, const QVector3D& color);
  Q_DECL_CONSTEXPR Vertex(const QVector3D& position,
                          const QVector3D& color,
                          const QVector2D& texCoord);
  Q_DECL_CONSTEXPR Vertex(const QVector3D& position, const QVector2D& texCoord);
  Q_DECL_CONSTEXPR Vertex(const QVector3D& position, float intensity);

  // Accessors / Mutators
  Q_DECL_CONSTEXPR const QVector3D& position() const;
  Q_DECL_CONSTEXPR const QVector3D& color() const;
  Q_DECL_CONSTEXPR const QVector2D& texCoord() const;
  Q_DECL_CONSTEXPR float intensity() const;
  void setPosition(const QVector3D& position);
  void setColor(const QVector3D& color);
  void setTexCoord(const QVector2D& texCoord);
  void setIntensity(float intensity);

  // OpenGL Helpers
  static const int PositionTupleSize = 3;
  static const int ColorTupleSize = 3;
  static const int TexCoordTupleSize = 3;
  static const int IntensitySize = 1;
  static Q_DECL_CONSTEXPR int positionOffset();
  static Q_DECL_CONSTEXPR int colorOffset();
  static Q_DECL_CONSTEXPR int texCoordOffset();
  static Q_DECL_CONSTEXPR int intensityOffset();
  static Q_DECL_CONSTEXPR int stride();

 private:
  QVector3D m_position;
  QVector3D m_color;
  QVector2D m_texCoord;
  float m_intensity{};
};

/*******************************************************************************
 * Inline Implementation
 ******************************************************************************/

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

// Constructors
Q_DECL_CONSTEXPR inline Vertex::Vertex() {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position) : m_position(position) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position, const QVector3D& color)
    : m_position(position), m_color(color) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position,
                                       const QVector3D& color,
                                       const QVector2D& texCoord)
    : m_position(position), m_color(color), m_texCoord(texCoord) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position, const QVector2D& texCoord)
    : m_position(position), m_texCoord(texCoord) {}
Q_DECL_CONSTEXPR inline Vertex::Vertex(const QVector3D& position, float intensity)
    : m_position(position), m_intensity(intensity) {}

// Accessors / Mutators
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::position() const {
  return m_position;
}
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::color() const {
  return m_color;
}
Q_DECL_CONSTEXPR inline const QVector2D& Vertex::texCoord() const {
  return m_texCoord;
}
Q_DECL_CONSTEXPR inline float Vertex::intensity() const {
  return m_intensity;
}
void inline Vertex::setPosition(const QVector3D& position) {
  m_position = position;
}
void inline Vertex::setColor(const QVector3D& color) {
  m_color = color;
}
void inline Vertex::setTexCoord(const QVector2D& texCoord) {
  m_texCoord = texCoord;
}
void inline Vertex::setIntensity(float intensity) {
  m_intensity = intensity;
}

// OpenGL Helpers
Q_DECL_CONSTEXPR inline int Vertex::positionOffset() {
  return offsetof(Vertex, m_position);
}
Q_DECL_CONSTEXPR inline int Vertex::colorOffset() {
  return offsetof(Vertex, m_color);
}
Q_DECL_CONSTEXPR inline int Vertex::texCoordOffset() {
  return offsetof(Vertex, m_texCoord);
}
Q_DECL_CONSTEXPR inline int Vertex::intensityOffset() {
  return offsetof(Vertex, m_intensity);
}
Q_DECL_CONSTEXPR inline int Vertex::stride() {
  return sizeof(Vertex);
}

#endif  // S3D_ANALYZER_RENDERING_VERTEX_H

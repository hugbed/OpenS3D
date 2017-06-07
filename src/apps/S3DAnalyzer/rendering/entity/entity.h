#ifndef S3D_ANALYZER_RENDERING_ENTITY_ENTITY_H
#define S3D_ANALYZER_RENDERING_ENTITY_ENTITY_H

class Entity {
 public:
  virtual void draw() = 0;
  virtual ~Entity() = default;
};

#endif  // S3D_ANALYZER_RENDERING_ENTITY_ENTITY_H

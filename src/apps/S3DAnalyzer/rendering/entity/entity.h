#ifndef RENDERING_ENTITY_ENTITY_H
#define RENDERING_ENTITY_ENTITY_H

class Entity {
 public:
  virtual void draw() = 0;
  virtual ~Entity() {}
};

#endif  // RENDERING_ENTITY_ENTITY_H

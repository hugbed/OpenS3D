#ifndef RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H

#include "rectangleentity.h"

class AnaglyphRectangleEntity : public RectangleEntity
{
public:
    AnaglyphRectangleEntity();
    void addShaders() override;
};

#endif // RENDERING_ENTITY_STEREO_ANAGLYPHRECTANGLEENTITY_H

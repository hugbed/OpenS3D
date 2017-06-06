#ifndef RENDERING_ENTITY_STEREO_RIGHTRECTANGLEENTITY_H
#define RENDERING_ENTITY_STEREO_RIGHTRECTANGLEENTITY_H

#include "rectangleentity.h"

class RightRectangleEntity : public RectangleEntity
{
public:
    RightRectangleEntity();
    void setTextureLeft(QOpenGLTexture */*texture*/) override;
    void setTextureRight(QOpenGLTexture *texture) override;
    void setHorizontalShift(float shift) override;
};

#endif // RENDERING_ENTITY_STEREO_RIGHTRECTANGLEENTITY_H

#ifndef RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H
#define RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H

#include "../entity.h"

#include <QOpenGLTexture>

class StereoImageEntity : public Entity {
public:
    virtual void init() = 0;
    virtual void setHorizontalShift(float shift) = 0;
    virtual void setAspectRatio(float ratio) = 0;
    virtual void setTextureLeft(QOpenGLTexture* texture) = 0;
    virtual void setTextureRight(QOpenGLTexture* texture) = 0;
};

#endif // RENDERING_ENTITY_STEREO_STEREOIMAGEENTITY_H

#include "leftrectangleentity.h"

LeftRectangleEntity::LeftRectangleEntity()
    : RectangleEntity()
{
}

void LeftRectangleEntity::setTextureRight(QOpenGLTexture */*texture*/)
{
}

void LeftRectangleEntity::setHorizontalShift(float shift)
{
    RectangleEntity::setHorizontalShift(shift/2.0f);
}

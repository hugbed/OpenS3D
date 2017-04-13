//
// Created by jon on 13/04/17.
//

#ifndef PROJECT_VIDEOPLAYER3DENTITYFACTORY_H
#define PROJECT_VIDEOPLAYER3DENTITYFACTORY_H

#include "VideoPlayerAnaglyphEntity.hpp"
#include "VideoPlayerSBSEntity.hpp"

namespace VideoPlayer3D {
enum class Mode { ANAGLYPH, SIDE_BY_SIDE };
}  // namespace VideoPlayer3D

class VideoPlayer3DEntityFactory {
 public:
  template <class... Args>
  static std::unique_ptr<Ogre::MovableObject> createVideoPlayer3DEntity(VideoPlayer3D::Mode mode,
                                                                        Args... args) {
    switch (mode) {
      case VideoPlayer3D::Mode::ANAGLYPH:
        return std::unique_ptr<Ogre::MovableObject>(
            std::make_unique<VideoPlayerAnaglyphEntity>(args...));
      case VideoPlayer3D::Mode::SIDE_BY_SIDE:
        return std::unique_ptr<Ogre::MovableObject>(
            std::make_unique<VideoPlayerSBSEntity>(args...));
    }
  }
};

#endif  // PROJECT_VIDEOPLAYER3DENTITYFACTORY_H

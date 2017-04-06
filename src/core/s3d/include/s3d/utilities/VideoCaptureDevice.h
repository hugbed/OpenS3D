//
// Created by jon on 05/04/17.
//

#ifndef PROJECT_FRAMEGRABBERPOLLING_H
#define PROJECT_FRAMEGRABBERPOLLING_H

namespace s3d {

class VideoCaptureDevice {
public:
    virtual void getImage(std::vector <uint8_t> &image);
};

}
#endif //PROJECT_FRAMEGRABBERPOLLING_H

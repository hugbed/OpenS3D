//
// Created by jon on 05/04/17.
//

#ifndef PROJECT_FRAMEGRABBERCALLBACK_H
#define PROJECT_FRAMEGRABBERCALLBACK_H

#include <vector>
#include <functional>

namespace s3d {

class FrameGrabberCallback {
public:
    using callback_type = std::function<void(const std::vector<uint8_t>&, const std::vector<uint8_t>&)>;

    FrameGrabberCallback(callback_type callback)
            : callback(callback) {}

protected:
    virtual void notifyNewFrame(std::vector<uint8_t> &leftFrameData, std::vector<uint8_t> &rightFrameData) {
        callback(frameData);
    }

private:
    callback_type callback;
};

}

#endif //PROJECT_FRAMEGRABBERCALLBACK_H

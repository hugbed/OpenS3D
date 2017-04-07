//
// Created by bedh2102 on 06/04/17.
//

#ifndef PROJECT_FILEVIDEOCAPTUREDEVICEFACTORY_H
#define PROJECT_FILEVIDEOCAPTUREDEVICEFACTORY_H

#include "VideoCaptureDeviceFactory.h"

class FileVideoCaptureDeviceFactory : VideoCaptureDeviceFactory {
public:
    virtual std::unique_ptr<VideoCaptureDevice> CreateDevice(
        const VideoCaptureDeviceDescriptor& deviceDescriptor
    ) override;
};

#endif //PROJECT_FILEVIDEOCAPTUREDEVICEFACTORY_H

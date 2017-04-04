//
// Created by jon on 22/03/17.
//

#ifndef S3D_UTILITIES_CONCURRENCY_PRODUCERBARRIERSYNC_H
#define S3D_UTILITIES_CONCURRENCY_PRODUCERBARRIERSYNC_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <algorithm>

namespace s3d {
namespace concurrency {

template <class T>
class ProducerBarrierSync {
public:
    ProducerBarrierSync(std::mutex &doneProducingMutex, std::condition_variable &shouldConsumeCV)
            : doneProducingCondition{false}, doneConsumingCondition{false}, shouldConsumeCV{shouldConsumeCV},
              doneProducingMutex{doneProducingMutex} {}

    virtual const T &getProduct() = 0;

    void startProducing() {
        while (!shouldStopProducing()) {
            waitUntilProductIsConsumed();
            produce();
            notifyDoneProducing();
        }
    }

    void acknowledgeDoneProducing()
    {
        doneProducingCondition = false;
    }

    bool isDoneProducing()
    {
        return doneProducingCondition;
    }

    void notifyDoneConsuming()
    {
        std::unique_lock<std::mutex> lk(doneConsumingMutex);
        doneConsumingCondition = true;
    }

    std::mutex &getDoneConsumingMutex()
    {
        return doneConsumingMutex;
    }

    void notifyShouldProduce()
    {
        shouldProduceCV.notify_one();
    }

    virtual bool shouldStopProducing()
    {
        return false;
    }

private:
    virtual void produce() = 0; // where the work happens : set product

    void notifyDoneProducing()
    {
        {
            std::unique_lock<std::mutex> lk(doneProducingMutex);
            doneProducingCondition = true;
        }
        shouldConsumeCV.notify_one();
    }

    void waitUntilProductIsConsumed()
    {
        // wait until doneConsuming
        std::unique_lock<std::mutex> lk(doneConsumingMutex);
        while (!(doneConsumingCondition)) shouldProduceCV.wait(lk, [this] { return doneConsumingCondition; });

        // we acknowledged it, set it back to default state
        doneConsumingCondition = false;
    }

    bool doneProducingCondition;
    bool doneConsumingCondition;
    std::condition_variable shouldProduceCV;
    std::condition_variable& shouldConsumeCV;
    std::mutex& doneProducingMutex;
    std::mutex doneConsumingMutex;
};

}
}

#endif //S3D_UTILITIES_CONCURRENCY_PRODUCERBARRIERSYNC_H

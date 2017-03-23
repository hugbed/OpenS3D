//
// Created by jon on 22/03/17.
//

#ifndef OGRE_S3D_PLAYER_SYNCCONSUMER_H
#define OGRE_S3D_PLAYER_SYNCCONSUMER_H

#include "SyncProducer.h"

#include <mutex>
#include <vector>

template <class T>
class SyncConsumer {
public:
    SyncConsumer(std::mutex &doneProducingMutex, std::condition_variable &shouldConsumeCV, const std::vector<SyncProducer<T>*> producers)
    : doneProducingMutex(doneProducingMutex)
    , shouldConsumeCV(shouldConsumeCV)
    , producers(producers)
    {}

    void startConsuming()
    {
        while (!shouldStopConsuming()) {
//            auto t1 = std::chrono::high_resolution_clock::now();
            waitToConsume();
            consume();
//            auto t2 = std::chrono::high_resolution_clock::now();
//            std::cout << "Consumption time : "
//                      << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count()
//                      << std::endl;

            notifyDoneConsuming();
        }
    }

    virtual bool shouldStopConsuming()
    {
        return false;
    }

private:
    virtual void consume() = 0; // use producer->product

    bool allAreDoneProducing()
    {
        return std::all_of(std::begin(producers), std::end(producers), [](auto &p) { return p->doneProducingCondition; });
    }

    void waitToConsume()
    {
        // wait until all producers are done producing
        std::unique_lock<std::mutex> lk(doneProducingMutex);
        while (!allAreDoneProducing()) {
            shouldConsumeCV.wait(lk, [this] {
                return allAreDoneProducing();
            });
        }

        // we acknowledged it, set it back to default state
        for (auto& producer : producers)
            producer->doneProducingCondition = false;
    }

    void notifyDoneConsuming()
    {
        for (auto &producer : producers)
        {
            std::unique_lock<std::mutex> lk(producer->doneConsumingMutex);
            producer->doneConsumingCondition = true;
        }
        for (auto &producer : producers)
        {
            producer->shouldProduceCV.notify_one();
        }
    }

    std::mutex &doneProducingMutex;
    std::condition_variable &shouldConsumeCV;

protected:
    std::vector<SyncProducer<T>*> producers;
};


#endif //OGRE_S3D_PLAYER_SYNCCONSUMER_H

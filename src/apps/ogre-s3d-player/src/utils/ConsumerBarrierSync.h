//
// Created by jon on 22/03/17.
//

#ifndef OGRE_S3D_PLAYER_SYNCCONSUMER_H
#define OGRE_S3D_PLAYER_SYNCCONSUMER_H

#include "ProducerBarrierSync.h"

#include <mutex>
#include <vector>

template <class T>
class ConsumerBarrierSync {
public:
    ConsumerBarrierSync(std::mutex &doneProducingMutex, std::condition_variable &shouldConsumeCV, const std::vector<ProducerBarrierSync<T>*> producers)
    : doneProducingMutex(doneProducingMutex)
    , shouldConsumeCV(shouldConsumeCV)
    , producers(producers)
    {}

    void startConsuming()
    {
        while (!shouldStopConsuming()) {
            notifyShouldProduce();
            waitToConsume();
            consume();
        }
    }

protected:
    const std::vector<ProducerBarrierSync<T>*>& getProducers()
    {
        return producers;
    }

private:
    virtual void consume() = 0; // use producer->getProduct()

    virtual bool shouldStopConsuming()
    {
        return std::any_of(std::begin(producers), std::end(producers), [](const auto& producer){
            return producer->shouldStopProducing();
        });
    }

    bool allAreDoneProducing()
    {
        return std::all_of(std::begin(producers), std::end(producers), [](auto &p) { return p->isDoneProducing(); });
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

        // acknowledge done producing, set it back to default state
        for (auto& producer : producers)
            producer->acknowledgeDoneProducing();
    }

    void notifyShouldProduce()
    {
        for (auto &producer : producers)
        {
            producer->notifyDoneConsuming();
        }
        for (auto &producer : producers)
        {
            producer->notifyShouldProduce();
        }
    }

    std::mutex &doneProducingMutex;
    std::condition_variable &shouldConsumeCV;
    std::vector<ProducerBarrierSync<T>*> producers;
};


#endif //OGRE_S3D_PLAYER_SYNCCONSUMER_H

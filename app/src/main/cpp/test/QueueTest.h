//
// Created by ky611 on 2020/6/21.
//

#ifndef XPLAY_QUEUETEST_H
#define XPLAY_QUEUETEST_H

#include <string>
#include "../XThread.h"
#include "../queue/PacketQueue.h"

class QueueTest : public XThread {
public:
    PacketQueue *queue;

    std::string name;

    QueueTest(PacketQueue *queue, const std::string &name);

    void Main() override;
};


#endif //XPLAY_QUEUETEST_H

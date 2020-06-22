//
// Created by ky611 on 2020/6/21.
//

#include "QueueTest.h"
#include "../XLog.h"


QueueTest::QueueTest(PacketQueue *queue, const std::string &name) : queue(queue), name(name) {


}

void QueueTest::Main() {

    std::__thread_id id = std::this_thread::get_id();

    LOGI("this thread is %s", name.c_str());

    for (int i = 0; i < 100; ++i) {
        queue->pushPacket(new AVPacket());
        LOGI("queue->pushPacket success      %s size= %d",
             name.c_str(),
             queue->getPacketSize());
    }
    LOGI("  thread   %s  执行完毕", name.c_str());

}

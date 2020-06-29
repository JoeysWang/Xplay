//
// Created by ky611 on 2020/6/21.
//

#include "QueueTest.h"
#include "../XLog.h"
#include "../data/XData.h"

QueueTest::QueueTest() {}


QueueTest::QueueTest(PacketQueue *queue, const std::string &name) : queue(queue), name(name) {


}

XData &maxx(XData &data1, XData &data2) {
    LOGI("data1 address=%p , data1 address=%p ", &data1, &data2);
    return data2;
}


void QueueTest::Main() {
    XData *d1 = new XData();
    XData *d2 = new XData();
    LOGI("d1 address=%p , d2 address=%p ", d1, d2);

    XData &d = maxx(*d1, *d2);
    LOGI("d  address=%p   ", &d);


}


//
// Created by 王越 on 2020/4/29.
//

#include "IObserver.h"

void IObserver::addObserver(IObserver *observer) {

    if (!observer)
        return;
    mutex.lock();
    observers.push_back(observer);
    mutex.unlock();

}

void IObserver::notify(XData *data) {
    mutex.lock();
    for (int i = 0; i < observers.size(); i++) {
        observers[i]->update(data);
    }
    mutex.unlock();
}

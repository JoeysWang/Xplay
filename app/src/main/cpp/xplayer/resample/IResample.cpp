//
// Created by ky611 on 2020/6/14.
//

#include "IResample.h"
#include "../../XLog.h"

void IResample::update(XData data) {
    XData d = resample(data);
    if (d.size > 0) {
        notify(d);
    }
}

IResample::IResample(PlayerState *playerState) : playerState(playerState) {
    LOGI("IResample constructor playerState=%p", playerState);

}

IResample::~IResample() {

}

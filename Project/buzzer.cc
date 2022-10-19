#include "buzzer.h"

namespace Project {

    void Buzzer::init(uint16_t prescaler, uint32_t period, uint32_t pulse) {
        auto fullCBFn = [](void *arg) {
            if (arg == nullptr) return;
            auto buzzer = (Buzzer *) arg;
            buzzer->cnt++;
            if (buzzer->cnt < buzzer->nPulse) return;
            buzzer->stop();
            buzzer->cnt = 0;
        };
        pwm.init(prescaler, period, pulse, channel, nullptr, nullptr, fullCBFn, this);
    }

}

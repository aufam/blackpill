#include "audio.h"

namespace Project {

    void Audio::init() {
        i2s.init(
                [](void *arg) {
                    auto &audio = *(Audio *)arg;
                    audio.event << EVENT_HALF;
                }, this,
                [](void *arg) {
                    auto &audio = *(Audio *)arg;
                    audio.event << EVENT_FULL;
                }, this);
        event.init();
        speakerDisable();
    }

    void Audio::read(Audio::BufferMono &buffer) {
        if (event.getCount() > 0) event.clear();

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        if (evt == EVENT_CLEAR) return;

        auto *stereo = evt == EVENT_HALF ? i2s.rxBuffer.begin() : i2s.rxBuffer.half();
        for (size_t i = 0; i < Buffer::halfLen(); i++) buffer[i] = stereo[i].left;
    }

    void Audio::read(Audio::BufferStereo &buffer) {
        if (event.getCount() > 0) event.clear();

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        if (evt == EVENT_CLEAR) return;

        auto *stereo = evt == EVENT_HALF ? i2s.rxBuffer.begin() : i2s.rxBuffer.half();
        for (size_t i = 0; i < Buffer::halfLen(); i++) buffer[i] = stereo[i];
    }

    void Audio::write(Audio::BufferMono &buffer) {
        if (event.getCount() > 0) event.clear();

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        if (evt == EVENT_CLEAR) return;

        auto *stereo = evt == EVENT_HALF ? i2s.txBuffer.half() : i2s.txBuffer.begin();
        for (size_t i = 0; i < Buffer::halfLen(); i++) stereo[i].left = buffer[i];
    }

    void Audio::write(Audio::BufferStereo &buffer) {
        if (event.getCount() > 0) event.clear();

        int evt = EVENT_CLEAR;
        event.pop(evt, osWaitForever);
        if (evt == EVENT_CLEAR) return;

        auto *stereo = evt == EVENT_HALF ? i2s.txBuffer.half() : i2s.txBuffer.begin();
        for (size_t i = 0; i < Buffer::halfLen(); i++) stereo[i] = buffer[i];
    }

    void Audio::speakerEnable() const {
        HAL_GPIO_WritePin(speakerEnPort, speakerEnPin, GPIO_PIN_SET);
    }

    void Audio::speakerDisable() const {
        HAL_GPIO_WritePin(speakerEnPort, speakerEnPin, GPIO_PIN_RESET);
    }

    bool Audio::isSpeakerEnabled() const {
        return HAL_GPIO_ReadPin(speakerEnPort, speakerEnPin);
    }
}
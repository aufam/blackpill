#ifndef PROJECT_AUDIO_H
#define PROJECT_AUDIO_H

#include "periph/i2s.h"
#include "etl/queue.h"
#include "periph/gpio.h"

namespace Project {

    struct Audio{
        using I2S    = Periph::I2S;
        using Buffer = I2S::Buffer;
        using Mono   = I2S::Mono;
        using Stereo = I2S::Stereo;
        using BufferStereo = DSP::Buffer<Stereo, Buffer::halfSize()>;
        using BufferMono = DSP::Buffer<Mono, Buffer::halfSize()>;
        using Event = etl::Queue<int, 1>;

        enum { EVENT_CLEAR, EVENT_HALF, EVENT_FULL };
        static const uint32_t eventTimeout = 100;

        I2S &i2s;
        Event event;
        Periph::GPIO speaker;

        constexpr explicit Audio(I2S &i2s, GPIO_TypeDef *speakerEnPort, uint16_t speakerEnPin)
        : i2s(i2s)
        , event()
        , speaker(speakerEnPort, speakerEnPin, Periph::GPIO::activeHigh)
        {}

        /// init i2s, init event
        void init() {
            auto halfCBFn = [](void *arg) { ((Audio *) arg)->event << EVENT_HALF; };
            auto fullCBFn = [](void *arg) { ((Audio *) arg)->event << EVENT_FULL; };
            i2s.init(halfCBFn, this, fullCBFn, this);
            event.init();
            speaker.off();
        }

        /// deinit i2s, deinit event
        void deinit() {
            i2s.deinit();
            event.deinit();
            speaker.off();
        }

        /// read blocking
        /// @param buffer[out] buffer to store audio data (mono)
        /// @param leftOrRight false: left (default), true: right
        /// @retval osStatus_t
        osStatus_t read(BufferMono &buffer, bool leftOrRight = false) {
            if (event) event.clear();

            int evt = EVENT_CLEAR;
            auto res = event.pop(evt, eventTimeout);
            if (evt != osOK) return res;

            auto *stereo = evt == EVENT_HALF ? i2s.rxBuffer.begin() : i2s.rxBuffer.half();
            if (leftOrRight)
                for (size_t i = 0; i < Buffer::halfSize(); i++) buffer[i] = stereo[i].right;
            else
                for (size_t i = 0; i < Buffer::halfSize(); i++) buffer[i] = stereo[i].left;
            return res;
        }

        /// read blocking
        /// @param buffer[out] buffer to store audio data (stereo)
        /// @retval osStatus_t
        osStatus_t read(BufferStereo &buffer) {
            if (event) event.clear();

            int evt = EVENT_CLEAR;
            auto res = event.pop(evt, eventTimeout);
            if (res != osOK) return res;

            auto *stereo = evt == EVENT_HALF ? i2s.rxBuffer.begin() : i2s.rxBuffer.half();
            for (size_t i = 0; i < Buffer::halfSize(); i++) buffer[i] = stereo[i];
            return res;
        }

        /// write blocking
        /// @param buffer[in] audio data (mono)
        /// @param leftOrRight false: left (default), true: right
        /// @retval osStatus_t
        osStatus_t write(const BufferMono &buffer, bool leftOrRight = false) {
            if (event) event.clear();

            int evt = EVENT_CLEAR;
            auto res = event.pop(evt, eventTimeout);
            if (res != osOK) return res;

            auto *stereo = evt == EVENT_HALF ? i2s.txBuffer.half() : i2s.txBuffer.begin();
            if (leftOrRight)
                for (size_t i = 0; i < Buffer::halfSize(); i++) stereo[i].right = buffer[i];
            else
                for (size_t i = 0; i < Buffer::halfSize(); i++) stereo[i].left = buffer[i];
            return res;
        }

        /// write blocking
        /// @param buffer[in] audio data (stereo)
        /// @retval osStatus_t
        osStatus_t write(const BufferStereo &buffer) {
            if (event) event.clear();

            int evt = EVENT_CLEAR;
            auto res = event.pop(evt, eventTimeout);
            if (res != osOK) return res;

            auto *stereo = evt == EVENT_HALF ? i2s.txBuffer.half() : i2s.txBuffer.begin();
            for (size_t i = 0; i < Buffer::halfSize(); i++) stereo[i] = buffer[i];
            return res;
        }

        void speakerEnable() const { speaker.on(); }
        void speakerDisable() const { speaker.off(); }
        [[nodiscard]] bool isSpeakerEnabled() const { return speaker.read(); }
    };

}

#endif //PROJECT_AUDIO_H

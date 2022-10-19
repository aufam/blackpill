#ifndef PROJECT_AUDIO_H
#define PROJECT_AUDIO_H

#include "periph/i2s.h"

namespace Project {

    struct Audio{
        using I2S    = Periph::I2S;
        using Buffer = I2S::Buffer;
        using Stereo = I2S::Stereo;
        using Mono   = int16_t;
        using BufferStereo = DSP::Buffer<Stereo, Buffer::halfLen()>;
        using BufferMono = DSP::Buffer<Mono, Buffer::halfLen()>;
        using Event = OS::QueueStatic<int, 1>;

        enum { EVENT_CLEAR, EVENT_HALF, EVENT_FULL };

        I2S &i2s;
        Event event;
        GPIO_TypeDef *speakerEnPort; ///< configured in cubeMX
        const uint16_t speakerEnPin; ///< configured in cubeMX
        constexpr explicit Audio(I2S &i2s, GPIO_TypeDef *speakerEnPort, uint16_t speakerEnPin)
                : i2s(i2s)
                , event()
                , speakerEnPort(speakerEnPort)
                , speakerEnPin(speakerEnPin)
        {}

        void init();
        void read(BufferMono &buffer);
        void read(BufferStereo &buffer);
        void write(BufferMono &buffer);
        void write(BufferStereo &buffer);

        void speakerEnable() const;
        void speakerDisable() const;
        bool isSpeakerEnabled() const;
    };

}

#endif //PROJECT_AUDIO_H

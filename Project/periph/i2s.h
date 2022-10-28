#ifndef PROJECT_PERIPH_I2S_H
#define PROJECT_PERIPH_I2S_H

#include "../../Core/Inc/i2s.h"
#include "dsp/buffer.h"

namespace Project::Periph {

    /// I2S peripheral class. requirements: Full duplex master, SPIx global interrupt, tx & rx DMA circular 16 bit
    struct I2S {
        /// callback function class
        struct Callback {
            typedef void (*Function)(void*);
            Function fn;
            void *arg;
        };
        typedef int16_t Mono;
        struct Stereo { Mono left, right; };
        using Buffer = DSP::BufferDouble<Stereo, 160 * 2>; ///< buffer type definition, stereo, dual buffer
        static const size_t nChannels = 2; ///< I2S standard is 2 channels (stereo)

        I2S_HandleTypeDef &hi2s; ///< I2S handler configured in cubeMX
        const size_t sampRate; ///< configured in cubeMX
        Buffer txBuffer{};
        Buffer rxBuffer{};
        Callback halfCB{};
        Callback fullCB{};
        constexpr I2S(I2S_HandleTypeDef &hi2s, size_t sampRate) : hi2s(hi2s), sampRate(sampRate) {}

        /// start transmit receive DMA and set callback
        /// @param halfCBFn half complete callback function pointer
        /// @param halfCBArg half complete callback function argument
        /// @param fullCBFn complete callback function pointer
        /// @param fullCBArg complete callback function argument
        void init(
                Callback::Function halfCBFn = nullptr, void *halfCBArg = nullptr,
                Callback::Function fullCBFn = nullptr, void *fullCBArg = nullptr);
        /// stop DMA
        void deinit();
        /// set callback
        /// @param halfCBFn half complete callback function pointer
        /// @param halfCBArg half complete callback function argument
        /// @param fullCBFn complete callback function pointer
        /// @param fullCBArg complete callback function argument
        void setCallback(
                Callback::Function halfCBFn = nullptr, void *halfCBArg = nullptr,
                Callback::Function fullCBFn = nullptr, void *fullCBArg = nullptr);
    };

    /// callback functions (half or full) will be called in 160 / 8000 = 20 ms
    inline I2S i2s2(hi2s2, 8000);

} // namespace Project


#endif // PROJECT_PERIPH_I2S_H
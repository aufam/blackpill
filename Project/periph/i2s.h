#ifndef PROJECT_PERIPH_I2S_H
#define PROJECT_PERIPH_I2S_H

#include "../../Core/Inc/i2s.h"
#include "dsp/buffer.h"

namespace Project::Periph {

    /// I2S peripheral class.
    /// requirements: Full duplex master, SPIx global interrupt, tx & rx DMA circular 16 bit, audio rate 8k
    struct I2S {
        /// callback function class
        struct Callback {
            typedef void (*Function)(void*);
            Function fn;
            void *arg;
        };
        typedef int16_t Mono;
        struct Stereo { Mono left, right; };

        static const size_t nChannels = 2; ///< I2S standard is 2 channels (stereo)
        static const size_t nSamples = 160; ///< number of samples (stereo) in each callback (half or full)
        static const size_t audioRate = 8000; ///< configured in cubeMX

        /// buffer type definition, stereo, dual buffer.
        /// callback functions (half or full) will be called in 160 / 8000 = 20 ms
        using Buffer = DSP::BufferDouble<Stereo, nSamples * nChannels>;

        I2S_HandleTypeDef &hi2s; ///< I2S handler configured in cubeMX
        Buffer txBuffer{};
        Buffer rxBuffer{};
        Callback halfCB{};
        Callback fullCB{};
        constexpr explicit I2S(I2S_HandleTypeDef &hi2s) : hi2s(hi2s) {}

        /// start transmit receive DMA and set callback
        /// @param halfCBFn half complete callback function pointer
        /// @param halfCBArg half complete callback function argument
        /// @param fullCBFn complete callback function pointer
        /// @param fullCBArg complete callback function argument
        void init(Callback::Function halfCBFn = nullptr, void *halfCBArg = nullptr,
                  Callback::Function fullCBFn = nullptr, void *fullCBArg = nullptr)
        {
            setCallback(halfCBFn, halfCBArg, fullCBFn, fullCBArg);
            HAL_I2SEx_TransmitReceive_DMA(&hi2s,
                                          (uint16_t *)txBuffer.data(),
                                          (uint16_t *)rxBuffer.data(),
                                          txBuffer.len() * nChannels);
        }

        /// stop DMA
        void deinit() {
            HAL_I2S_DMAStop(&hi2s);
        }

        /// set callback
        /// @param halfCBFn half complete callback function pointer
        /// @param halfCBArg half complete callback function argument
        /// @param fullCBFn complete callback function pointer
        /// @param fullCBArg complete callback function argument
        void setCallback(Callback::Function halfCBFn = nullptr, void *halfCBArg = nullptr,
                         Callback::Function fullCBFn = nullptr, void *fullCBArg = nullptr)
        {
            halfCB.fn = halfCBFn; halfCB.arg = halfCBArg;
            fullCB.fn = fullCBFn; fullCB.arg = fullCBArg;
        }
    };

    /// callback functions (half or full) will be called in 160 / 8000 = 20 ms
    inline I2S i2s2 { hi2s2 };

} // namespace Project


#endif // PROJECT_PERIPH_I2S_H
#ifndef PERIPH_CONFIG_H
#define PERIPH_CONFIG_H

// TIM encoder
#if !defined(PERIPH_ENCODER_USE_IT) && !defined(PERIPH_ENCODER_USE_DMA)
#define PERIPH_ENCODER_USE_IT
#endif

// TIM input capture
#if !defined(PERIPH_INPUT_CAPTURE_USE_IT) && !defined(PERIPH_INPUT_CAPTURE_USE_DMA)
#define PERIPH_INPUT_CAPTURE_USE_IT
#endif

// TIM PWM 
#if !defined(PERIPH_PWM_USE_IT) && !defined(PERIPH_PWM_USE_DMA)
#define PERIPH_PWM_USE_IT
#endif

// I2C
#if !defined(PERIPH_I2C_MEM_WRITE_USE_IT) && !defined(PERIPH_I2C_MEM_WRITE_USE_DMA)
#define PERIPH_I2C_MEM_WRITE_USE_DMA
#endif

// I2S
#if !defined(PERIPH_I2S_AUDIO_RATE)
#define PERIPH_I2S_AUDIO_RATE 8000
#endif

#if !defined(PERIPH_I2S_N_SAMPLES)
#define PERIPH_I2S_N_SAMPLES 160
#endif

#if !defined(PERIPH_I2S_CHANNEL_MONO) && !defined(PERIPH_I2S_CHANNEL_STEREO)
#define PERIPH_I2S_CHANNEL_STEREO
#endif

#define PERIPH_I2S_SAMPLING_TIME ((double) PERIPH_I2S_N_SAMPLES / (double) PERIPH_I2S_AUDIO_RATE)

// UART
#if !defined(PERIPH_UART_RECEIVE_USE_IT) && !defined(PERIPH_UART_RECEIVE_USE_DMA)
#define PERIPH_UART_RECEIVE_USE_IT
#endif

#if !defined(PERIPH_UART_TRANSMIT_USE_IT) && !defined(PERIPH_UART_TRANSMIT_USE_DMA)
#define PERIPH_UART_TRANSMIT_USE_IT
#endif

namespace Project::periph::detail {

    template <typename T, size_t N>
    class UniqueInstances {
    public:
        T* instances[N] = {};

        bool push(T* it) {
            for (auto& instance : instances) if (!instance) {
                instance = it;
                return true;
            }
            return false;
        }

        bool pop(T* it) {
            for (auto& instance : instances) if (instance == it) {
                instance = nullptr;
                return true;
            }
            return false;
        }
    };
}


#endif // PERIPH_CONFIG_H
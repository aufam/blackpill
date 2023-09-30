#ifndef PROJECT_ENCODER_H
#define PROJECT_ENCODER_H

#include "Core/Inc/tim.h"
#include "etl/function.h"

namespace Project::periph {

    /// rotary encoder using TIM
    /// @note requirements: TIMx encoder mode, TIMx global interrupt
    struct Encoder {
        using Callback = etl::Function<void(), void*>; ///< callback function class

        TIM_HandleTypeDef &htim;        ///< TIM handler configured by cubeMX
        int16_t value = 0;              ///< current value
        Callback incrementCB;
        Callback decrementCB;

        /// default constructor
        constexpr explicit Encoder(TIM_HandleTypeDef &htim) : htim(htim) {}

        Encoder(const Encoder&) = delete; ///< disable copy constructor
        Encoder& operator=(const Encoder&) = delete;  ///< disable copy assignment

        /// start encoder interrupt
        void init() { HAL_TIM_Encoder_Start_IT(&htim, TIM_CHANNEL_ALL); }

        /// disable encoder interrupt
        void deinit() { HAL_TIM_Encoder_Stop_IT(&htim, TIM_CHANNEL_ALL); }

        void inputCaptureCallback() {
            uint16_t counter = htim.Instance->CNT;
            int cnt = counter / 4;
            if (cnt > value) incrementCB();
            if (cnt < value) decrementCB();
            value = static_cast<int16_t> (cnt);
        }
    };

    inline Encoder encoder4(htim4);

} // namespace Project


#endif // PROJECT_ENCODER_H
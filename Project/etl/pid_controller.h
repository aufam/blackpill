#ifndef ETL_PID_CONTROLLER_H
#define ETL_PID_CONTROLLER_H

#include "etl/math.h"

namespace Project::etl {
    
    template <typename KP, typename KI, typename KD, typename KDF>
    class PID {
        KP kp;
        KI ki;
        KD kd;
        KDF kdf;

        float integral = 0, derivative = 0, errorPrev = 0;

    public:
        constexpr PID(KP kp, KI ki, KD kd, KDF kdf) : kp(kp), ki(ki), kd(kd), kdf(kdf) {}

        float calculate(float err, float dt) {
            float kp_, ki_, kd_, kdf_;

            if constexpr (etl::is_floating_point_v<decay_t<KP>>) kp_ = kp; // expect floating point
            else kp_ = kp(); // expect nullary function
            
            if constexpr (etl::is_floating_point_v<decay_t<KI>>) ki_ = ki; // expect floating point
            else ki_ = ki(); // expect nullary function
            
            if constexpr (etl::is_floating_point_v<decay_t<KD>>) kd_ = kd; // expect floating point
            else kd_ = kd(); // expect nullary function
            
            if constexpr (etl::is_floating_point_v<decay_t<KDF>>) kdf_ = kdf; // expect floating point
            else if constexpr (is_same_v<decay_t<KDF>, None>) kdf_ = 1.f;
            else kdf_ = kdf(); // expect nullary function

            integral += err * dt;
            derivative = etl::low_pass_fast(derivative, (err - errorPrev) / dt, kdf_);
            errorPrev = err;
            return kp_ * err + ki_ * integral + kd_ * derivative;
        }
    };

    template <typename KP, typename KI, typename KD, typename KDF> constexpr auto
    pid(KP&& kp, KI&& ki, KD&& kd, KDF&& kdf) { 
        return PID(etl::forward<KP>(kp), etl::forward<KI>(ki), etl::forward<KD>(kd), etl::forward<KDF>(kdf));
    }

    template <typename KP, typename KI, typename KD> constexpr auto
    pid(KP&& kp, KI&& ki, KD&& kd) { 
        return PID(etl::forward<KP>(kp), etl::forward<KI>(ki), etl::forward<KD>(kd), etl::none);
    }
}

#endif
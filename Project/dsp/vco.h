#ifndef PROJECT_DSP_VCO_H
#define PROJECT_DSP_VCO_H

#include "types.h"

namespace Project::DSP {

    struct VCO {
        angle32_t phase;    // current angle phase (in radian), normalize to uint32_t
        int32_t phase_rate; // current angle speed (radian / sample), normalize to int32_t
        uint32_t samp_rate; // samp_rate for the object in Hz

        // deviation and samp_rate in Hz
        constexpr explicit VCO(int32_t deviation = 0, uint32_t samp_rate = 0xFFFFFFFFUL) :
                phase(0_pi),
                phase_rate((int32_t) (deviation * (0xFFFFFFFFUL / samp_rate))),
                samp_rate(samp_rate)
        {}

        amplitude8_t cos()  const { return this->phase.cos(); }
        amplitude8_t sin()  const { return this->phase.sin(); }
        complex8_t sincos() const { return {this->cos(), this->sin()}; }

        void setDeviation(int32_t deviation) {
            this->phase_rate = (int32_t) (deviation * (0xFFFFFFFFUL / this->samp_rate));
        }

        float getPhase() const { return float(this->phase); } // in radian
        float getPhaseRate() const { return angle32_t::fixToFloat(this->phase_rate); } // in radian / sample

        VCO &operator ++() {
            this->phase += this->phase_rate;
            return *this;
        }
        VCO operator ++(int) {
            this->phase += this->phase_rate;
            return *this;
        }
    };
}

#endif //PROJECT_DSP_VCO_H

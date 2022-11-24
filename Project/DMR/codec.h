#ifndef PROJECT_DMR_CODEC_H
#define PROJECT_DMR_CODEC_H

#include <cmath>
#include "imbe_vocoder/imbe_vocoder.h"
#include "ambe_engine/ambe.h"

namespace Project::DMR {

	class Codec {
	 public:
        enum { N_SAMPLES = 160, N_BYTES = 9 };
		
	 private:
		imbe_vocoder vocoder;
		mbe_parms mbe_now, mbe_prv, mbe_enh;
		float gain_adjust;
		
		// decoder mode
		int error, error2;
		char error_str[64], data[49];
		
	 public:
		Codec();
		void encode(const int16_t audio_in[N_SAMPLES], uint8_t bytes_out[N_BYTES]);
		void decode(const uint8_t bytes_in[N_BYTES], int16_t audio_out[N_SAMPLES]);
		
	 private:
		void encode_ambe(int b[N_BYTES]);
		void encode_49bit(uint8_t out[49], const int b[N_BYTES]);
		void encode_dmr(const unsigned char in[N_BYTES], unsigned char out[N_BYTES]);
	};

} // namespace DMR


#endif // PROJECT_DMR_CODEC_H
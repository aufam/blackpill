#ifndef PROJECT_CODEC_H
#define PROJECT_CODEC_H

//#define USE_MATH_DEFINES
#include <cmath>
#include "imbe_vocoder/imbe_vocoder.h"
#include "ambe_engine/ambe.h"
#ifndef M_PI
#define M_PI        3.14159265358979323846
#define M_SQRT2        1.41421356237309504880
#endif
namespace Project {

	class Codec {
	 public:
	 	struct Number {
			enum : uint32_t {SAMPLES = 160, BYTES = 9};
		};
		
	 private:
		imbe_vocoder vocoder;
		mbe_parms mbe_now, mbe_prv, mbe_enh;
		float gain_adjust;
		
		// decoder mode
		int error, error2;
		char error_str[64], data[49];
		
	 public:
		Codec();
		void encode(const int16_t audiosamp_in[Number::SAMPLES], uint8_t bytes_out[Number::BYTES]);
		void decode(const uint8_t bytes_in[Number::BYTES], int16_t audiosamp_out[Number::SAMPLES]);
		
	 private:
		void encode_ambe(int b[Number::BYTES]);
		void encode_49bit(uint8_t out[49], const int b[Number::BYTES]);
		void encode_dmr(const unsigned char in[Number::BYTES], unsigned char out[Number::BYTES]);
	};

} // namespace DMR


#endif // PROJECT_CODEC_H
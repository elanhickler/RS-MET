#ifndef RAPT_CONSTANTS_H_INCLUDED
#define RAPT_CONSTANTS_H_INCLUDED

// todo: change the #defines into constants and wrap them into the RAPT namespace - we don't want
// to pollute everything with #defines

// mathematical constants (maybe prepend RS_ ):

#define PI 3.1415926535897932384626433832795
#define PI_INV 0.31830988618379067153776752674503       // 1/PI
#define EULER 2.7182818284590452353602874713527
#define EULER_CONSTANT 0.5772156649015328606065120900824024310421
#define SQRT2 1.4142135623730950488016887242097
#define SQRT2_INV 0.70710678118654752440084436210485    // 1 / sqrt(2)
#define LN10 2.3025850929940456840179914546844
#define LN10_INV 0.43429448190325182765112891891661     // 1 / log(10)
#define LN2 0.69314718055994530941723212145818          // log(2)
#define LN2_INV 1.4426950408889634073599246810019       // 1 / log(2)
#define GOLDEN_RATIO 1.6180339887498948482045868343656381 // (1+sqrt(5))/2

#define PI_F ((float)PI)

// music/audio related constants:
#define SEMITONE_FACTOR 1.0594630943592952645618252949463  // 12th root of 2

// constants related to numeric format:
#undef min  // some silly include header on windows (minwindef.h) defines min/max as macros
#undef max
#define RS_INF(T) (std::numeric_limits<T>::infinity())
#define RS_MIN(T) (std::numeric_limits<T>::min())
#define RS_MAX(T) (std::numeric_limits<T>::max())
#define RS_EPS(T) (std::numeric_limits<T>::epsilon())
#define RS_NAN(T) (std::numeric_limits<T>::quiet_NaN())  // rename to RS_QUIET_NAN
#define RS_SIGNALING_NAN(T) (std::numeric_limits<T>::signaling_NaN())



// powers of two:
#define RS_POW2_0 1          // 2^0 = 1
#define RS_POW2_1 2          // 2^1 = 2
#define RS_POW2_2 4          // 2^2 = 4
#define RS_POW2_3 8          // 2^3 = 8
#define RS_POW2_4 16         // etc.
#define RS_POW2_5 32
#define RS_POW2_6 64
#define RS_POW2_7 128
#define RS_POW2_8 256
#define RS_POW2_9 512
#define RS_POW2_10 1024
#define RS_POW2_11 2048
#define RS_POW2_12 4096
#define RS_POW2_13 8192
#define RS_POW2_14 16384
#define RS_POW2_15 32768

//#define FIX_DENORM 1.e-12  // small constant to add to avoid denormal numbers
#define RS_TINY RS_MIN(float)


#endif

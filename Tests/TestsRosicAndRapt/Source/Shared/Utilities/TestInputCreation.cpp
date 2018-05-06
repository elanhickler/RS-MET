#include "TestInputCreation.h" 

// these will eventually go to the RAPT library:

template <class T>
inline void fillWithZeros(T *buffer, int length)
{
  for(int i = 0; i < length; i++)
    buffer[i] = T(0);
}

template <class T>
inline T rsSawWave(T x)
{
  T tmp = (T)fmod(x, 2*PI);
  if( tmp < PI )
    return T(tmp/PI);
  else
    return T((tmp/PI) - 2);
}

template <class T>
inline T rsSqrWave(T x)
{
  T tmp = (T)fmod(x, 2*PI);
  if( tmp < PI )
    return 1;
  else
    return -1;
}

template <class T>
inline T rsTriWave(T x)
{
  T tmp = (T)fmod(x, 2*PI);
  if( tmp < 0.5f*PI )
    return T(tmp/(0.5f*PI));
  else if( tmp < 1.5f*PI )
    return T(1 - ((tmp-0.5f*PI)/(0.5f*PI)));
  else
    return T(-1 + ((tmp-1.5f*PI)/(0.5f*PI)));
}

//-------------------------------------------------------------------------------------------------

void createTimeAxis(int numSamples, float *timeAxis, float sampleRate)
{
  for(int n = 0; n < numSamples; n++)
    timeAxis[n] = n / sampleRate;
}

void createTimeAxis(int numSamples, double *timeAxis, double sampleRate)
{
  for(int n = 0; n < numSamples; n++)
    timeAxis[n] = n / sampleRate;
}

template<class T>
void createWaveform(T *x, int N, int shape, T frequency, T sampleRate, T phase, bool antiAlias)
{
  T w = (T)(2*PI*frequency/sampleRate);
  fillWithZeros(x, N);
  switch( shape )
  {
  case 0:
  {
    if( !(frequency >= sampleRate/2 && antiAlias == true) )
    {
      for(int n=0; n<N; n++)
        x[n] = sin(w*n + phase);
    }
  }
  break;
  case 1:
  {
    if( antiAlias == false )
    {
      for(int n=0; n<N; n++)
        x[n] = (T) rsSawWave(T(w*n) + T(phase));
    }
    else
    {
      int k = 1;
      while( k*frequency < sampleRate/2 )
      {
        T a = T(-2.f / (k*PI));
        for(int n=0; n<N; n++)
          x[n] += T(a * sin(k*(w*n+PI) + phase));
        k++;
      }
    }
  }
  break;
  case 2:
  {
    if( antiAlias == false )
    {
      for(int n=0; n<N; n++)
        x[n] = rsSqrWave(T(w*n + phase));
    }
    else
    {
      int k = 1;
      while( k*frequency < sampleRate/2 )
      {
        T a = T(-4.f / (k*PI));
        for(int n=0; n<N; n++)
          x[n] += T(a * sin(k*(w*n+PI) + phase));
        k+=2;
      }
    }
  }
  break;
  case 3:
  {
    if( antiAlias == false )
    {
      for(int n=0; n<N; n++)
        x[n] = rsTriWave(w*n + phase);
    }
    else
    {
      int k = 1;
      T s = 1.0; // sign 
      while( k*frequency < sampleRate/2 )
      {
        T a = T(8.f / (k*k*PI*PI));
        for(int n=0; n<N; n++)
          x[n] += s * a * sin(k*w*n + phase);
        k +=  2;
        s *= -1.0;
      }
    }
  }
  break;
  }
}
template void createWaveform(float *x, int N, int shape, float frequency, float sampleRate, 
  float phase, bool antiAlias);
template void createWaveform(double *x, int N, int shape, double frequency, double sampleRate, 
  double phase, bool antiAlias);


void createPulseWave(double *x, int N, double frequency, double dutyCycle, 
  double sampleRate, double phase, bool antiAlias)
{
  double w = 2*PI*frequency/sampleRate;
  fillWithZeros(x, N);
  if( antiAlias == false )
  {
    for(int n=0; n<N; n++)
      x[n] = RAPT::rsPulseWave(w*n + phase, dutyCycle);
  }
  else
  {
    int k = 1;
    while( k*frequency < sampleRate/2 )
    {
      double a = 4.0 * sin(k*PI*dutyCycle) / (k*PI);
      for(int n=0; n<N; n++)
        x[n] += a * cos(k*w*n + phase);
      k++;
    }
  }
}

double sineSum(double p, double *A, double N)
{
  double y = 0.0;
  for(int h = 1; h <= N; h++)
    y += A[h-1] * sin(h*p);
  return y;
}

void createSineWave(double *x, int N, double f, double a, double fs)
{
  double w = 2*PI*f/fs;
  double p = 0;            // startphase - make this an optional parameter later
  for(int n = 0; n < N; n++)
    x[n] = a * sin(w*n+p);
}

void createSineWave(double *x, int N, double *f, double a, double fs)
{
  double s   = 2*PI/fs;  // frequency scaler
  double phi = 0.0;      // instantaneous phase
  for(int n = 0; n < N; n++)
  {
    x[n] = a * sin(phi);
    phi += s * f[n];
  }
}

template<class T>
std::vector<T> createNoise(int numSamples, int seed, T min, T max)
{
  std::vector<T> x(numSamples);
  RAPT::rsNoiseGenerator<T> ng;
  ng.setRange(min, max);
  ng.setSeed(seed);
  ng.reset();
  for(int n = 0; n < numSamples; n++)
    x[n] = ng.getSample();
  return x;
}
template std::vector<float> createNoise(int numSamples, int seed, float min, float max);
template std::vector<double> createNoise(int numSamples, int seed, double min, double max);
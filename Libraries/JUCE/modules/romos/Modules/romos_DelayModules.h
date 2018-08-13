#ifndef romos_DelayModules_h
#define romos_DelayModules_h

#include "../Framework/romos_ModuleAtomic.h"
#include "romos_ModuleDefinitionMacros.h"

namespace romos
{

  /** Delays the input signal by one sample. */
  class UnitDelayModule : public ModuleAtomic
  {
    CREATE_COMMON_DECLARATIONS_1(UnitDelayModule);
  public:
    virtual void resetVoiceState(int voiceIndex);
  protected:
    virtual void allocateMemory();
    virtual void freeMemory();
    double *buffer;    
  };

  // todo: MultiUnitDelay/UnitDelayChain: provides outputs for x[n-1], x[n-2], ... - useful for 
  // building filters

  // DelayLine: user can choose interpolation (round, linear, allpass, etc.) on gui
  // Inputs: Audio-In, Delay (in seconds)
  // Outputs: delayed audio

  // MultiTapDelay: x[n-N1], x[n-N2], ...
  // Inputs: Audio-In, Delay-times
  // Outputs: delayed audio signals

} 

#endif 

#ifndef jura_BreakpointModulatorAudioModule_h
#define jura_BreakpointModulatorAudioModule_h

/** This class wraps a RAPT::BreakpointModulator into a jura::AudioModule to facilitate its use as 
plugIn or sub-module inside a plugIn. */

class JUCE_API BreakpointModulatorAudioModule 
  : public AudioModule, public ModulationSource
{

  friend class BreakpointModulatorEditor;
  friend class BreakpointModulatorGlobalEditor;
  friend class BreakpointParameterEditor;
  friend class BreakpointModulatorEditorMulti;
  friend class BreakpointModulatorEditorCompact;

public:

  //---------------------------------------------------------------------------------------------
  // construction/destruction:

  /** Constructor. */
  BreakpointModulatorAudioModule(CriticalSection *newPlugInLock, 
    rosic::BreakpointModulator *newBreakpointModulatorToWrap);

  //---------------------------------------------------------------------------------------------
  // overrides:

  virtual void parameterChanged(Parameter* parameterThatHasChanged);

  virtual void setStateFromXml(const XmlElement& xmlState, const juce::String& stateName,
    bool markAsClean);

  virtual XmlElement* getStateAsXml(const juce::String& stateName, bool markAsClean);

  virtual void setStateToDefaults();

  virtual void getSampleFrameStereo(double* inOutL, double* inOutR)
  {
    *inOutR = *inOutL = wrappedBreakpointModulator->getSample();
  }
  // maybe, this function is obsolete now

  virtual void updateModulationValue() override
  {
    modValue = wrappedBreakpointModulator->getSample();
  }

  // new overrides (added after dragging the old code over - they are currently only dummies):
  virtual AudioModuleEditor *createEditor() override { return nullptr; }
  virtual void processBlock(double **inOutBuffer, int numChannels, int numSamples) override {}


protected:

  /** Fills the array of automatable parameters. */
  virtual void initializeAutomatableParameters();

  /** Pointer to the underlying RAPT object which is wrapped. */
  //RAPT::rsBreakpointModulator<double> *wrappedBreakpointModulator;
  rosic::BreakpointModulator *wrappedBreakpointModulator;


  juce_UseDebuggingNewOperator;
};


#endif 

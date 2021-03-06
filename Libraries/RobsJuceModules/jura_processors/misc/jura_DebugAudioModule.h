#ifndef jura_DebugModule_h
#define jura_DebugModule_h

/** A module for framework debugging puposes */

class JUCE_API DebugAudioModule : public AudioModuleWithMidiIn
{

public:

  DebugAudioModule(CriticalSection *lockToUse);
    
  virtual void createParameters();

  // overriden from AudioModule baseclass:
  AudioModuleEditor* createEditor(int type) override;
  virtual void processBlock(double **inOutBuffer, int numChannels, int numSamples) override;
  virtual void processStereoFrame(double *left, double *right) override;
  virtual void setSampleRate(double newSampleRate) override; 
  virtual void reset() override;
  virtual void setMidiController(int controllerNumber, float controllerValue) override;

  // callback target functions:
  void setLeftValue(double newValue);
  void setRightValue(double newValue);
  void setSmoothingTime(double newTime);
  void setTestParam(double newValue);

protected:

  /** Tries to cast passed Parameter int rsSmoothableParameter - if successful, sets the smoothing 
  time for it. */
  void setSmoothingTime(Parameter* p, double newTime);

  static const int numValues = 2;
  double values[numValues] = { 0, 0 };

  Parameter *leftParam, *rightParam;
  Parameter *smoothParam, *testParam;


  EqualizerAudioModule* eqModule = nullptr;
  // We use an equalizer to see if dealing with child-modules and works well. Also, the eq
  // has a dynamic number of parameters, so we can check that stuff with eq, too.


  friend class DebugModuleEditor;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugAudioModule)
};

//=================================================================================================

/** Editor for the debugging AudioModule. */

class JUCE_API DebugModuleEditor : public AudioModuleEditor
{

public:

  DebugModuleEditor(jura::DebugAudioModule *newDebugModuleToEdit);
  virtual void resized() override;

protected:

  void createWidgets();

  EqualizerModuleEditor *eqEditor = nullptr;

  DebugAudioModule *debugModule;

  rsVectorPad *xyPad;
  rsNodeEditor *nodeEditor = nullptr;

  rsModulatableSlider *leftSlider, *rightSlider, *smoothSlider, *testSlider;
  //rsAutomatableComboBox *modeComboBox;
  //rsAutomatableButton *invertButton;  
   // use ModulatabelSlider, etc later

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DebugModuleEditor)
};


#endif 
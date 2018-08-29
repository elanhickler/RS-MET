#ifndef jura_LibertyModules_h
#define jura_LibertyModules_h


//=================================================================================================
// subclasses for concrete module types:

class ParameterModuleEditor : public ModulePropertiesEditor 
{
public:
  ParameterModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit); 
  virtual ~ParameterModuleEditor()
  {
    int dummy = 0; // for debug
  }
  virtual void rButtonClicked(RButton *buttonThatWasClicked);
  virtual void resized();
  virtual void updateWidgetsFromModuleState();
  juce_UseDebuggingNewOperator;
protected:

  juce::Rectangle<int> topSectionRect, setupRect, controlSetupRect;

  RTextField *parameterSetupLabel;
  RTextField *helpTextLabel;
  LibertyTextEntryField        *helpTextField;
  RTextField                   *minValueLabel, *maxValueLabel;
  LibertyTextEntryField        *minValueField, *maxValueField;
  LibertySlider                *valueSlider;
  LibertyLabeledTextEntryField *valueField, *defaultField, *unitField;
  LibertyNamedComboBox         *scalingComboBox; 
  RClickButton                 *enterValueButton, *setToDefaultButton;

  // current, default, scaling, unit, quantization
  // ControllerNumber, ControlRangeMin, ControlRangeMax, Smoothing

  //RTextField            *minValueLabel,  *defaultValueLabel,  *maxValueLabel;
  //LibertyTextEntryField *minValueField,  *defaultValueField,  *maxValueField;
  //RClickButton          *setToMinButton, *setToDefaultButton, *setToMaxButton;

  // mappingBox
  // metaSilder, metaMinField, metaMaxField
  // smoothingSlider

  // todo: derive parameter modules from jura_ModulatableParameter -> let the user set up 
  // modulations
};

// rename to LibertyParameterEditor, LibertyContainerEditor, etc.

//-------------------------------------------------------------------------------------------------

class ContainerModuleEditor : public ModulePropertiesEditor 
{
public:
  ContainerModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit); 
  juce_UseDebuggingNewOperator;

protected:

};

class TopLevelModuleEditor : public ModulePropertiesEditor 
{
public:
  TopLevelModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit); 
  juce_UseDebuggingNewOperator;

protected:

};

class VoiceKillerModuleEditor : public ModulePropertiesEditor 
{
public:
  VoiceKillerModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit);   
  virtual void resized();
  juce_UseDebuggingNewOperator;
protected:
  //RLabeledTextEntryField *thresholdField, *timeOutField;
  LibertySlider *thresholdSlider, *timeOutSlider;
};

class WhiteNoiseModuleEditor : public ModulePropertiesEditor 
{
public:
  WhiteNoiseModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit);   
  virtual void resized();
  juce_UseDebuggingNewOperator;
protected:
  LibertySlider *seedSlider;
};

class BiquadDesignerModuleEditor : public ModulePropertiesEditor 
{
public:
  BiquadDesignerModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit);   
  virtual void resized();
  juce_UseDebuggingNewOperator;
protected:
  LibertyNamedComboBox *modeComboBox;
};

class LibertyLadderFilterModuleEditor : public ModulePropertiesEditor 
{
public:
  LibertyLadderFilterModuleEditor(LibertyAudioModule *newLiberty, romos::Module* newModuleToEdit);   
  virtual void resized();
  juce_UseDebuggingNewOperator;
protected:
  LibertyNamedComboBox *filterModeComboBox, *saturationModeComboBox;
};







#endif
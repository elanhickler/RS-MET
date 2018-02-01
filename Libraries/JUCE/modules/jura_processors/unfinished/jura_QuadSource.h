
class JUCE_API QuadSourceAudioModule : public jura::AudioModule // maybe rename to QuadPolySource
{

public:

  QuadSourceAudioModule(CriticalSection *lockToUse, rosic::rsQuadSource* coreToUse);

  AudioModuleEditor* createEditor() override;


protected:

  rosic::rsQuadSource* sourceCore;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuadSourceAudioModule)
};

//=================================================================================================

class JUCE_API QuadSourceEditor : public jura::AudioModuleEditor
{

public:

  QuadSourceEditor(CriticalSection* lockToUse, QuadSourceAudioModule* sourceToEdit);

protected:


  QuadSourceAudioModule* sourceModule;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QuadSourceEditor)
};
#ifdef ROSIC_H_INCLUDED
/* When you add this cpp file to your project, you mustn't include it in a file where you've
already included any other headers - just put it inside a file on its own, possibly with your config
flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
header files that the compiler may be using. */
#error "Incorrect use of JUCE cpp file"
#endif

#include "rosic.h"

/** The cpp files are included in the order in which they depend on each other. 
ToDo: reorder them in the library accordingly, such that files in one library folder depend only on 
other files in folders that are considered "above" in the hierarchy. 
In the future, rosic should be made dependent on the RAPT library and whereever it makes sense, the
rosic-class should be turned into an template instatiation of  RAPT class template.
jura_processors should depend on the rosic module and grab its DSP code from there
rename modules:
rosic: rs_dsp (this should never depend on any juce class/module)
jura_framework: rs_framework
jura_processors: rs_audio_processors
...namespace name should be rs (but do all of this only after dragging in all old plugin code and 
integrating it into the Chainer.

*/

// basics (but we needed to intersperse some stuff from other folders)
#include "basics/GlobalFunctions.cpp"
#include "basics/rosic_ChannelMatrix2x2.cpp"
#include "basics/rosic_Constants.cpp"                        // empty
#include "basics/rosic_FunctionTemplates.cpp"                // empty
#include "basics/rosic_HelperFunctions.cpp"
#include "basics/rosic_Interpolator.cpp"
#include "basics/rosic_NumberManipulations.cpp"              // empty
#include "infrastructure/rosic_MutexLock.cpp"                // used by SampleBuffer
#include "basics/rosic_SampleBuffer.cpp"
#include "basics/rosic_SamplePlaybackParameters.cpp"
#include "math/rosic_ElementaryFunctionsReal.cpp"            // used by SpecialFunctionsReal?
#include "math/rosic_RealFunctionEvaluationAlgorithms.cpp"   // used by SpecialFunctionsReal
#include "math/rosic_SpecialFunctionsReal.cpp"               // used by ComplexFunctions?
#include "math/rosic_Complex.cpp"                            // used by ComplexFunctionsAlgorithms
#include "math/rosic_ComplexFunctionsAlgorithms.cpp" 
#include "math/rosic_ComplexFunctions.cpp"                   // used by ExpressionEvaluator
#include "scripting/rosic_ExpressionEvaluator.cpp"           // used by TabulatedFunction
#include "basics/rosic_TabulatedFunction.cpp"                // needs ExpressionEvaluator
#include "basics/rosic_WarpedAllpassInterpolator.cpp"
#include "basics/rosic_WindowDesigner.cpp"

// datastructures
#include "datastructures/rosic_Array.cpp"                    // empty
#include "datastructures/rosic_ExtensionsForSTL.cpp"         // empty
#include "datastructures/rosic_KeyValueMap.cpp"              // empty
#include "datastructures/rosic_String.cpp"

// math (some of the cpp files in this folder are already included in the basics section):
#include "math/rosic_IntegerFunctions.cpp"
#include "math/rosic_LinearAlgebra.cpp"
#include "math/rosic_Vector.cpp"
#include "math/rosic_Matrix.cpp"
#include "math/rosic_MatrixVectorFunctions.cpp"
#include "math/rosic_Interpolation.cpp"
#include "math/rosic_PolynomialAlgorithms.cpp"
#include "math/rosic_PrimeNumbers.cpp"
#include "math/rosic_Transformations.cpp"

// transforms:
#include "transforms/rosic_FourierTransformerRadix2.cpp"
#include "transforms/rosic_FourierTransformerBluestein.cpp"
#include "transforms/rosic_WaveletTransforms.cpp"
#include "transforms/rosic_SpectralManipulator.cpp"  // maybe move to other folder

// filters
#include "filters/rosic_AllpassChain.cpp"
#include "filters/rosic_BiquadBase.cpp"
#include "filters/rosic_FilterAnalyzer.cpp"
#include "filters/rosic_BiquadCascade.cpp"
#include "filters/rosic_BiquadDesigner.cpp"
#include "filters/rosic_BiquadMonoDF1.cpp"
#include "filters/rosic_BiquadStereoDF1.cpp"
#include "filters/rosic_BiquadStereoDF2.cpp"
#include "filters/rosic_CombFilter.cpp"
#include "filters/rosic_DampingFilter.cpp"
#include "filters/rosic_CombResonator.cpp"
#include "filters/rosic_ConvolverBruteForce.cpp"
#include "filters/rosic_ConvolverFFT.cpp"
#include "filters/rosic_ConvolverPartitioned.cpp"
#include "filters/rosic_CookbookFilter.cpp"
#include "filters/rosic_PrototypeDesigner.cpp"
#include "filters/rosic_PoleZeroMapper.cpp"
#include "filters/rosic_FilterCoefficientConverter.cpp"
#include "filters/rosic_InfiniteImpulseResponseDesigner.cpp"
#include "filters/rosic_EngineersFilter.cpp"
#include "filters/rosic_LinkwitzRileyCrossOver.cpp"
#include "filters/rosic_CrossOver4Way.cpp"
#include "filters/rosic_DirectFormFilter.cpp"
#include "filters/rosic_TwoPoleFilter.cpp"
#include "filters/rosic_DualTwoPoleFilter.cpp"
#include "filters/rosic_EllipticQuarterBandFilter.cpp"
#include "filters/rosic_EllipticSubBandFilter.cpp"
#include "filters/rosic_EllipticSubBandFilterDirectForm.cpp"
#include "filters/rosic_Equalizer.cpp"
#include "filters/rosic_EqualizerStereo.cpp"
#include "filters/rosic_FiniteImpulseResponseDesigner.cpp"
#include "filters/rosic_FiniteImpulseResponseFilter.cpp"
#include "filters/rosic_FourPoleFilter.cpp"
#include "filters/rosic_OnePoleFilter.cpp"
#include "filters/rosic_OnePoleFilterStereo.cpp"
#include "filters/rosic_LadderFilter.cpp"
#include "filters/rosic_LeakyIntegrator.cpp"
#include "filters/rosic_LowpassHighpass.cpp"
#include "filters/rosic_LowpassHighpassStereo.cpp"
#include "filters/rosic_LpfHpfApf.cpp"
#include "filters/rosic_NyquistBlocker.cpp"
#include "filters/rosic_QuadratureNetwork.cpp"
#include "filters/rosic_SlopeFilter.cpp"
#include "filters/rosic_TeeBeeFilter.cpp"
#include "filters/rosic_ToneControl.cpp"
#include "filters/rosic_TwoPoleBandpass.cpp"
#include "filters/rosic_VowelFilterStereo.cpp"
#include "filters/rosic_WarpedBiquadMonoDF1.cpp"            // empty
#include "filters/rosic_WhiteToPinkFilter.cpp"

// rendering:
//#include "rendering/rosic_AdditveWaveformRenderer.cpp"    // obsolete?
#include "rendering/rosic_AlgorithmicWaveformRenderer.cpp"
#include "rendering/rosic_MultiSegmentWaveformRenderer.cpp"
#include "rendering/rosic_NonRealtimeProcesses.cpp"         // should be renamed
#include "rendering/rosic_StandardWaveformRenderer.cpp"
#include "rendering/rosic_WaveformBuffer.cpp"
#include "rendering/rosic_WaveformRenderer.cpp"

// generators:
#include "generators/rosic_MipMappedWaveTable.cpp"
#include "generators/rosic_MipMappedWaveTableOld.cpp"       // can this be removed?
#include "generators/rosic_MipMappedWaveTableStereo.cpp"
#include "generators/rosic_BlendOscillator.cpp"
#include "generators/rosic_LorentzSystem.cpp"
#include "generators/rosic_ModalSynthesizer.cpp"            // replace with code from RSLib
#include "generators/rosic_NoiseGenerator.cpp"
#include "generators/rosic_NoiseGeneratorOld.cpp"           // can this be removed?
#include "generators/rosic_Oscillator.cpp"
#include "generators/rosic_OscillatorBank.cpp"
#include "generators/rosic_OscillatorStereo.cpp"
#include "generators/rosic_FourOscSection.cpp"
#include "generators/rosic_SampleOscillator.cpp"
#include "generators/rosic_SamplePlayer.cpp"
#include "generators/rosic_SineOscillator.cpp"
#include "generators/rosic_SineOscillatorStereo.cpp"
#include "generators/rosic_SuperOscillator.cpp"
#include "generators/rosic_TestGenerator.cpp"
#include "generators/rosic_WaveTable.cpp"

// modulators:
//#include "modulators/MagicCarpetModulator.cpp" // needs MagicCarpetDefinitions.h - where is this?
#include "modulators/rosic_AmpEnvRc.cpp"
#include "modulators/rosic_AnalogEnvelope.cpp"
#include "modulators/rosic_AnalogEnvelopeScaled.cpp"
#include "modulators/rosic_AttackDecayEnvelope.cpp"
#include "modulators/rosic_BreakpointModulator.cpp"
#include "modulators/rosic_DecayEnvelope.cpp"
#include "modulators/rosic_EnvelopeGenerator.cpp"
#include "modulators/rosic_EnvelopeGenerator2.cpp"  // do we need this?
#include "modulators/rosic_ExponentialRamp.cpp"
#include "others/rosic_SlewRateLimiter.cpp"         // move to basics
#include "others/rosic_SlewRateLimiterLinear.cpp"
//#include "others/rosic_SlewRateLimiterOld.cpp"    // not used anymore
#include "modulators/rosic_LowFrequencyOscillator.cpp" // needs SlewRateLimiter, WaveTable
#include "modulators/rosic_PitchEnvRc.cpp"
#include "modulators/rosic_SampleModulator.cpp"
#include "modulators/rosic_Modulator.cpp"              // needs SampleModulator

// others:
#include "others/rosic_DemoVersionNoiseEmitter.cpp"    // may not be needed
#include "others/rosic_RandomNumberGenerator01.cpp"
#include "others/rosic_RandomNumberGenerator02.cpp"
#include "others/rosic_KeyGenerator.cpp"               // remove...or keep only the "Validator" part
#include "others/rosic_OverlapAddProcessor.cpp"
#include "others/rosic_PiecewiseFunction.cpp"
//#include "others/rosic_Plotter.cpp"                  // obsolete? needs writeDataToFile
#include "others/rosic_ProcessorCycleCounter.cpp"
#include "others/rosic_RoutingMatrix.cpp"
#include "others/rosic_SpectralProcessor.cpp"
#include "others/rosic_SpectralEnvelopeProcessor.cpp"
#include "others/rosic_SpectralFilter.cpp"
#include "others/rosic_TuningTable.cpp"
#include "others/rosic_VectorMixer.cpp"  




// a bit out of order - these are kind-of higher-level generators, maybe they should be in the
// instruments section...or something:
#include "infrastructure/rosic_Module.cpp"            // needed by Quadrigen
//#include "others/rosic_RoutingMatrix.cpp"           // needed by Quadrigen
#include "generators/rosic_Quadrigen.cpp"             // needs Module, RoutingMatrix
#include "generators/rosic_VectorSamplePlayer.cpp"    // needs LowFrequencyOscillator (in modulators)
                                                      // and VectorMixer




// analysis:
#include "analysis/rosic_CyclicAutoCorrelator.cpp" 
//#include "analysis/rosic_EnvelopeFollower.cpp"    // needs SlewRateLimiter
#include "analysis/rosic_LinearPredictor.cpp"
#include "analysis/rosic_FormantRemover.cpp"
#include "analysis/rosic_FormantPreserver.cpp"

//#include "analysis/rosic_InstantaneousEnvelopeDetector.cpp"
//#include "analysis/rosic_LevelDetector.cpp"



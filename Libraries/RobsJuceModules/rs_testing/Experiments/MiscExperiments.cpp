// todo: SinusoidalSynthesizer to RAPT to make it available here

using namespace RAPT;


// convenience function:
std::vector<double> synthesizeSinusoidal(
  const RAPT::rsSinusoidalModel<double>& model, double sampleRate, double fadeTime)
{
  typedef RAPT::rsSinusoidalSynthesizer<double> SS;
  typedef SS::PhaseInterpolationMethod PIM;
  SS synth;
  synth.setSampleRate(sampleRate);
  //synth.setCubicAmplitudeInterpolation(true);
  synth.setPhaseInterpolation(PIM::tweakedFreqIntegral);
  std::vector<double> x = synth.synthesize(model);
  if(fadeTime > 0.0)
    applyFadeInAndOut( &x[0], (int) x.size(), int (fadeTime*sampleRate));
  return x;
}

void setupHarmonicAnalyzerFor(RAPT::rsHarmonicAnalyzer<double>& analyzer, 
  const std::string& sampleName, double fs, double f0)
{
  // First, set up some standard settings:

  typedef rsWindowFunction::WindowType WT;
  analyzer.setSampleRate(fs);
  analyzer.setSincInterpolationLength(64);
  analyzer.setNumCyclesPerBlock(4);
  //analyzer.setWindowType(WT::hamming);
  analyzer.setWindowType(WT::blackman);
  analyzer.setSpectralOversampling(8);  // zero padding
  analyzer.setAllowInharmonics(true);
  analyzer.setSpectralPeakSearchWidth(0.5);       // default: 1 - blackman needs a value less than 1
  analyzer.setMinPeakToMainlobeWidthRatio(0.75);  // default: 0.75

  //analyzer.setFreqsByPhaseDerivative(true);
  //analyzer.setFreqPhaseConsistency(true);
  // todo: maybe provide different freq-refinement methods (not necessarily mutually exclusive)

  // set up settings of the embedded cycle-mark finder:
  rsCycleMarkFinder<double>& cmf = analyzer.getCycleFinder();
                                              // defaults:
  cmf.setRelativeBandpassWidth(0.5);          // 1.0
  cmf.setBandpassSteepness(5);                // 3
  cmf.setSubSampleApproximationPrecision(2);  // 1, 0: linear, 1: cubic, 2: quintic, ...
  cmf.setFundamentalRange(50., 1000.);        // 20, 5000 
  cmf.setFundamental(f0);                     // 0 -> auto-detect
  //cmf.setAlgorithm(cmf.F0_ZERO_CROSSINGS);
  cmf.setAlgorithm(cmf.CYCLE_CORRELATION);


  // Second: override some settings for specific samples:

  if(sampleName == "flute-C-octave2")
  {

  }
  else if(sampleName == "piano_E2")
  {
    analyzer.setNumCyclesPerBlock(8);
    analyzer.setSpectralOversampling(4);
    analyzer.setSpectralPeakSearchWidth(1.0);
    analyzer.setMinPeakToMainlobeWidthRatio(0.5);
  }
  else if(sampleName == "Rhodes_F3")
  {
    //cmf.setAlgorithm(cmf.F0_ZERO_CROSSINGS);
    // because CYCLE_CORRELATION hangs - but why?
  }
  // etc...

}

// maybe it will soon make sense to wrap this into a class
void testHarmonicResynthesis(const std::string& name, std::vector<double>& input, 
  double fs, double f0, bool writeWaveFiles, bool plotResults)
{
  // analyze, resynthesize and create resiudal signal:


  double* x = &input[0];   // pointer to first sample (for convenience)
  int Nx = (int) input.size();


  // Analysis:

  RAPT::rsHarmonicAnalyzer<double> analyzer;
  setupHarmonicAnalyzerFor(analyzer, name, fs, f0);
  RAPT::rsSinusoidalModel<double> mdl = analyzer.analyze(x, Nx);


  // Manipulations:

  mdl.removePartial(0);                      // remove DC
  mdl.removePartialsWithMeanFreqAbove(fs/2); // anti-alias

  //mdl.keepOnly({0, 9});  // for test with TwoSines_Freq1=200_Freq2=2025
  //mdl.removePartial(0);    // test: resynthesize without fundamental

  //mdl.keepOnly({0, 99});  // fom TwoSines_Freq1=100_Freq2=10020

  //mdl.removePartialsAbove(9); 
  // for testing artifacts (spurious high partials) in flute-c1 - are they analysis or synthesis 
  // artifacts? if they occur evene though we remove the partials above 9, they occur in synthesis
  // ->they seem to be analysis artifacts

  //double splitFreq = 1000;
  //RAPT::rsSinusoidalModel<double> lp = rsSinusoidalProcessor<double>::extractLowpassPart(mdl, splitFreq);
  //RAPT::rsSinusoidalModel<double> hp = rsSinusoidalProcessor<double>::extractHighpassPart(mdl, splitFreq);


  // Resynthesis:

  typedef RAPT::rsSinusoidalSynthesizer<double> SS;
  typedef SS::PhaseInterpolationMethod PIM;
  SS synth;
  synth.setSampleRate(fs);
  //synth.setCubicAmplitudeInterpolation(true);
  synth.setPhaseInterpolation(PIM::tweakedFreqIntegral);
  //synth.setPhaseInterpolation(PIM::cubicHermite);
  //synth.setPhaseInterpolation(PIM::quinticHermite);
  //synth.setPhaseInterpolation(PIM::linear);  // does not yet exist
  std::vector<double> output = synth.synthesize(mdl);

  std::vector<double> error = output-input;
  double* y = &output[0]; int Ny = (int) output.size(); // again, for convenience
  double* e = &error[0];  int Ne = (int) error.size();  // dito




  // Plotting and file output:

  // write original, resynthesized and error signals to files, if desired:
  if(writeWaveFiles == true) {
    rosic::writeToMonoWaveFile((name + "Original.wav").c_str(),      x, Nx, (int)fs);
    rosic::writeToMonoWaveFile((name + "Resynthesized.wav").c_str(), y, Ny, (int)fs);
    rosic::writeToMonoWaveFile((name + "Residual.wav").c_str(),      e, Ne, (int)fs);
  }


  // move the required functions to rs_testing module
  // plot model data, if desired....
  bool plotModel = false;      // make user parameter
  if(plotModel == true)
    plotSineModel(mdl, fs);


  // plot original, resynthesized and error signals, if desired:
  if(plotResults == true) {
    GNUPlotter plt;
    plt.addDataArrays(Nx, x);
    plt.addDataArrays(Ny, y);
    plt.addDataArrays(Ne, e);

    std::vector<double> marks = analyzer.getOriginalTimeStamps();
    std::vector<double> zeros(marks.size());    // y values for plotting (all zero)
    RAPT::rsArray::fillWithZeros(&zeros[0], (int) marks.size());
    plt.addDataArrays((int) marks.size(), &marks[0], &zeros[0]);

    plt.setGraphStyles("lines", "lines", "lines", "points");
    plt.setPixelSize(1000, 300);
    plt.plot();
  }
}

void testMakeHarmonic(const std::string& name, std::vector<double>& input,
  double fs, double f0Out, double inharmonicity,  double f0In)
{
  //double* x = &input[0];   // pointer to first sample (for convenience)
  //int Nx = (int) input.size();

  // analyze:

  RAPT::rsHarmonicAnalyzer<double> analyzer;
  setupHarmonicAnalyzerFor(analyzer, name, fs, f0In);
  RAPT::rsSinusoidalModel<double> mdl = analyzer.analyze(&input[0], (int) input.size());

  // plotSineModel(mdl, fs); // move to rapt

  // process model data:

  mdl.removePartial(0); 
  //mdl.keepOnly({ 0 });  // for test
  rsSinusoidalProcessor<double>::makeStrictlyHarmonic(mdl, f0Out, inharmonicity, 0.5);
  mdl.removePartialsWithMeanFreqAbove(fs/2); // anti-alias - maybe use min- insetad of mean-freq to be safe...


  //plotSineModel(mdl, fs);

  // (re)synthesize:
  typedef RAPT::rsSinusoidalSynthesizer<double> SS;
  typedef SS::PhaseInterpolationMethod PIM;
  SS synth;
  synth.setSampleRate(fs);
  synth.setCubicAmplitudeInterpolation(true);
  //synth.setPhaseInterpolation(PIM::tweakedFreqIntegral);
  synth.setPhaseInterpolation(PIM::cubicHermite);
  std::vector<double> output = synth.synthesize(mdl);

  // due to new phase-relationships, the maximum output amplitude may be different from the 
  // original sound - renormalize:
  rsArray::normalize(&output[0], (int) output.size(), 1.0);


  std::string name2 = name + "_F=" + std::to_string((int)f0Out) + "Hz";
  if(inharmonicity != 0)
    name2 += "_B=" + std::to_string(inharmonicity);
  name2 += ".wav";
  // the formatting of floating point numbers std::to_string sucks -> write a better rsToString 
  // function
  rosic::writeToMonoWaveFile(name2.c_str(), &output[0], (int) output.size(), (int)fs);
}

void getPaddedSignals(double* xIn, int Nx, 
  const RAPT::rsSinusoidalModel<double>& model,
  const RAPT::rsSinusoidalSynthesizer<double>& synth,
  std::vector<double>& x, std::vector<double>& y)
{
  typedef std::vector<double> Vec;

  // synthesize y from the model:
  y = synth.synthesize(model);
  int nf = model.getStartSampleIndex(synth.getSampleRate()); // # fade-in samples

  size_t n;
  if(nf < 0) {
    // obtain a version of x with an appropriate number of zeros prepended
    nf = -nf;
    x.resize(Nx+nf);
    for(n = 0; n < nf; n++)        x[n] = 0;
    for(n = nf; n < x.size(); n++) x[n] = xIn[n-nf];
  }
  else {
    RAPT::rsPadLeft(y, nf, 0.0);       // prepend zeros to y, if nf > 0
    x = RAPT::toVector(xIn, Nx);
  }

  // extend the shorter of both signals with zeros:
  size_t nx = x.size();
  size_t ny = y.size();
  if(nx > ny)
    RAPT::rsResizeWithInit(y, nx, 0.0);
  else if(ny > nx)
    RAPT::rsResizeWithInit(x, ny, 0.0);
}


void testModalResynthesis(const std::string& name, std::vector<double>& x,
  double fs, double f0)
{
  int N = (int) x.size();

  RAPT::rsHarmonicAnalyzer<double> sineAnalyzer;
  setupHarmonicAnalyzerFor(sineAnalyzer, name, fs, f0);
  RAPT::rsSinusoidalModel<double> sineModel = sineAnalyzer.analyze(&x[0], N);
  sineModel.removePartial(0);  // we don't want DC
  //plotSineModel(sineModel, fs);
  //std::vector<double> ys = synthesizeSinusoidal(sineModel, fs);
  //rosic::writeToMonoWaveFile("ModalSineOutput.wav", &ys[0], (int) ys.size(), (int)fs);
  // ...maybe also remove all partials whose (mean) frequency is above the nyquist freq

  rsModalAnalyzer<double> modeAnalyzer;
  std::vector<rsModalFilterParameters<double>> modeModel
    = modeAnalyzer.getModalModel(sineModel);
  plotModalAmplitudes(modeModel);
  std::vector<double> y = synthesizeModal(modeModel, fs, N);

  rosic::writeToMonoWaveFile("ModalInput.wav",  &x[0],  N, (int)fs);
  rosic::writeToMonoWaveFile("ModalOutput.wav", &y[0],  N, (int)fs);
}

void testDeBeating(const std::string& name, std::vector<double>& x, double fs, double f0)
{
  // for reference, write input into a wavefile:
  rosic::writeToMonoWaveFile(name + "DeBeatInput.wav",  &x[0], (int)x.size(), (int)fs);

  // create and set up analyzer and obtain sinusoidal model and write a resynthesized unmodified
  // signal into a wavefile:
  int N = (int) x.size(); // x is the input signal

  RAPT::rsHarmonicAnalyzer<double> analyzer;
  setupHarmonicAnalyzerFor(analyzer, name, fs, f0);
  RAPT::rsSinusoidalModel<double> mdl = analyzer.analyze(&x[0], N);
  std::vector<double> y = synthesizeSinusoidal(mdl, fs);
  rosic::writeToMonoWaveFile(name + "DeBeatOutputUnmodified.wav", &y[0], (int)y.size(), (int)fs);

  // mdl now contains the sinusoidal model for the sound. Now, we set up a rsPartialBeatingRemover 
  // and apply the de-beating to model data, then synthesize the de-beated signal and write into
  // wavefile:
  rsPartialBeatingRemover<double> deBeater;
  deBeater.setPhaseSmoothingParameters(5.0, 1, 4); // cutoff = 10 leaves a vibrato
  deBeater.processModel(mdl);
  y = synthesizeSinusoidal(mdl, fs);
  rosic::writeToMonoWaveFile(name + "DeBeatOutput.wav", &y[0], (int)y.size(), (int)fs);
}

void testEnvelopeMatching(std::vector<double>& x1, std::vector<double>& x2)
{
  // todo: 
  // (1) extract envelopes of both signals
  // (2) find best match offset

  //rsEnvelopeExtractor<double> ee;

  rsEnvelopeFollower2<double> ef;
  ef.setSampleRate(44100);  // make this a function parameter
  ef.setInputFrequency(85); // this too

  // exctract envelopes:
  std::vector<double> e1(x1.size()), e2(x2.size());
  int n;
  for(n = 0; n < (int )x1.size(); n++) e1[n] = ef.getSample(x1[n]);
  ef.reset();
  for(n = 0; n < (int )x2.size(); n++) e2[n] = ef.getSample(x2[n]);

  rsPlotVectors(x2, e2);




  int dummy = 0;
}





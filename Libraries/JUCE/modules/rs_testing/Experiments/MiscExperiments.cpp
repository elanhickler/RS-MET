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
  synth.setCubicAmplitudeInterpolation(true);
  synth.setPhaseInterpolation(PIM::tweakedFreqIntegral);
  std::vector<double> x = synth.synthesize(model);
  if(fadeTime > 0.0)
    applyFadeInAndOut( &x[0], (int) x.size(), int (fadeTime*sampleRate));
  return x;
}

// maybe it will soon make sense to wrap this into a class
void testHarmonicResynthesis(const std::string& name, std::vector<double>& input, 
  double fs, double f0, bool writeWaveFiles, bool plotResults)
{
  // analyze, resynthesize and create error signal:


  // Analysis:

  double* x = &input[0];   // pointer to first sample (for convenience)
  int Nx = (int) input.size();

  // create and set up harmonic analyzer object:
  RAPT::rsHarmonicAnalyzer<double> analyzer;
  analyzer.setSampleRate(fs);
  analyzer.setSincInterpolationLength(64);
  //analyzer.setSpectralOversampling(8);       // implementation not yet complete
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
  cmf.setAlgorithm(cmf.F0_ZERO_CROSSINGS);
  //cmf.setAlgorithm(rsCycleMarkFinder<double>::CYCLE_CORRELATION);

  // let the analyzer analyze the sound - obtains a sinusoidal model:
  RAPT::rsSinusoidalModel<double> mdl = analyzer.analyze(x, Nx);
  mdl.removePartial(0);        // remove DC component


  // Manipulations:

  //mdl.keepOnly({0, 9});  // for test with TwoSines_Freq1=200_Freq2=2025
  //mdl.removePartial(0);    // test: resynthesize without fundamental

  //mdl.keepOnly({0, 99});  // fom TwoSines_Freq1=100_Freq2=10020


  // Resynthesis:

  typedef RAPT::rsSinusoidalSynthesizer<double> SS;
  typedef SS::PhaseInterpolationMethod PIM;
  SS synth;
  synth.setSampleRate(fs);
  synth.setCubicAmplitudeInterpolation(true);
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


  // plot model data, if desired....
  bool plotModel = true;
  //if(plotModel == true)
    //plotSineModel(mdl, fs);
  // 

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
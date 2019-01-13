#pragma once


template<class T>
std::vector<T> synthesizeSinusoidal(const RAPT::rsSinusoidalModel<T>& model, T sampleRate);


//template<class T>
//RAPT::rsSinusoidalModel<T> analyzeSinusoidal(T* sampleData, int numSamples, T sampleRate);




/** Prototype of STFT based sinusoidal analyzer */

template<class T>
class SinusoidalAnalyzer
{

public:

  inline void setBlockSize(int newBlockSize)      { sp.setBlockSize(newBlockSize); }
  inline void setHopSize(int newHopSize)          { sp.setHopSize(newHopSize); }
  inline void setZeroPaddingFactor(int newFactor) { sp.setZeroPaddingFactor(newFactor); }

  /** Should be one of the type in RAPt::rsWindowFunction::windowTypes */
  inline void setWindowType(int newType)          { sp.setAnalysisWindowType(newType); }
  //setRootKey/setFundamentalFrequency, 

  RAPT::rsSinusoidalModel<T> analyze(T* sampleData, int numSamples, T sampleRate) const;

protected:

  /** For a given frequency (belonging to a psectral peak of the current frame), search through the
  tracks array to find the one that is closest in frequency, if it is within a given range around 
  the peak-frequency and for which the trackContinued flag is false (i.e. the track has not been 
  used up by another current peak frequency already - each track can be continued only with one
  partner). Called from continuePartialTracks */
  size_t findBestMatch(T frequency, std::vector<RAPT::rsSinusoidalPartial<double>>& tracks,
    T maxFreqDeviation, const std::vector<bool>& trackContinued) const;

  /** This function implements the peak continuation step - for all current spectral peaks in 
  newPeakData, find a corresponding continuation partner among the activeTracks - 3 situations have 
  to be handled:
  -when a partner is found, continue the track, i.e. append the newPeakData to the corresponding
   track in activeTracks
  -when no partner is found, create a new track ("birth"), i.e. start a new track in activeTracks
  -all active tracks that have not been used in this continuation are killed (i.e. moved to 
   finishedTracks */
  void continuePartialTracks(
    std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeakData,
    std::vector<RAPT::rsSinusoidalPartial<T>>& activeTracks,
    std::vector<RAPT::rsSinusoidalPartial<T>>& finishedTracks,
    T maxFreqDeviation, T frameTimeDelta, int direction) const;
  // rename to continuePartialTracks1 and let continuePartialTracks be a dispatcher that selects
  // between continuePartialTracks1/continuePartialTracks2
  // or rename to findContinuations


  /** Internal function called from continuePartialTracks... */
  void applyContinuations(
    std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeakData,
    std::vector<RAPT::rsSinusoidalPartial<T>>& activeTracks,
    std::vector<RAPT::rsSinusoidalPartial<T>>& finishedTracks,
    std::vector<size_t>& births, std::vector<size_t>& deaths,
    std::vector<std::pair<size_t, size_t>>& continuations);







  /** Alternative version of the peak-tracking algoritm. This one loops over all the tracks to find 
  the best match in newPeakData instead of looping over all peaks to find a best match in the 
  activeTracks, i.e. the roles are reversed. This is, how it's described in the literature
  Not yet finished
  */
  //void continuePartialTracks2(
  //  std::vector<RAPT::rsInstantaneousSineParams<T>>& newPeakData,
  //  std::vector<RAPT::rsSinusoidalPartial<T>>& activeTracks,
  //  std::vector<RAPT::rsSinusoidalPartial<T>>& finishedTracks,
  //  T maxFreqDeviation, T frameTimeDelta, int direction) const;



  RAPT::rsSpectrogram<T> sp;   // spectrogram processor

};
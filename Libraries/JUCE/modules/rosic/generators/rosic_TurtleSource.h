#ifndef rosic_TurtleSource_h
#define rosic_TurtleSource_h

namespace rosic
{

/** A class for counting up to some upper limit and then reset to zero. Meant to be used in a real 
time turtle graphics driver to reset the turtle's state periodically. */

class ResetCounter
{

public:

  ResetCounter();

  /** Sets the reset interval for the counter. The interval does not have to be an integer. If it 
  has a fractional part, this class will internally manage to appropriately alternate between the 
  floor and ceiling of the desired interval so as to give an average reset interval equal to the 
  desired value. */
  void setInterval(double newInterval);

  /** Counts one unit up for the given counter and possibly resets it to zero, when the limit is 
  reached. The return value informs, whether or nor a rest occurred. */
  bool tick();

  //void setParametersZero();

  void setParametersToOffMode();

  /** Resets the internal state of the counter. */
  void reset();

  double getInterval() { return interval; }

protected:

  void updateJitteringLimit();

  static const int maxInterval = INT_MAX-1; // does it work at that value or do we need a margin?

  // counter parameter variables:
  double interval;   // time interval after which to periodically reset, redundant but kept for optimization
  int    intPart;    // integer part of interval
  double fracPart;   // fractional part of interval

  // counter state variables:
  double errAccu;        // accumulated fractional error
  int    jitteringLimit; // alternates between floor and ceil of interval
  int    counter;        // the actual counter

};
// maybe it can be implemented much simpler by just using double for the counter and limit - why
// do i make it more complicated than necessary? ...maybe implement an alternative counter and do
// unit- and performance tests


//=================================================================================================

/** TurtleSource is a sound generator based on the turtle graphics concept. It interprets the 
sequence of x,y points generated by the turtle as outputs for the left and right stereo channel. */

class TurtleSource
{

public:

  /** Enumeration of different the ways that can be used to interpolate between the sequence of 
  points that the turtle generates. */
  enum interpolationModes
  {
    LEFT_NEIGHBOUR,
    RIGHT_NEIGHBOUR,
    NEAREST_NEIGHBOUR,
    LINEAR,
    CUBIC,               // cubic hermite - matches slopes at datapoints
    QUARTIC              // like cubic and normalizes integral to be the same as in linear
  };

  TurtleSource();

  //-----------------------------------------------------------------------------------------------
  // \name Setup

  /** Sets the string of turtle graphics commands to be used to produce ("draw") the output 
  signal. */
  void setTurtleCommands(const std::string& commands);

  /** Sets the sample-rate. */
  void setSampleRate(double newSampleRate);

  /** Sets the frequency (in Hz) of the signal to be generated. */
  void setFrequency(double newFrequency);

  /** Sets a scale factor for the frequency to be generated (mainly for testing stuff - maybe 
  remove in final version) */
  void setFrequencyScaler(double newScaler);

  /** Sets the amplitude (as raw factor). */
  void setAmplitude(double newAmplitude) { amplitude = newAmplitude; }

  /** Sets a rotation angle (in degrees) to be applied to the produced xy coordinate pair. */
  void setRotation(double newRotation);

  /** Sets the starting position within the curve as normalized value between 0 and 1. The value, 
  you give here, will be quantized to the start of the nearest line segment and a new setting will 
  take effect on the next reset. */
  void setStartPosition(double newPosition);
  // not yet implemented...maybe it's not a good idea to round to the next integer line position
  // because when combining outputs of 2 L-systems with different numbers of iterations, they may
  // fall out of sync...maybe, we even have to do something more to make sure that line segments
  // in the lower order system are synced to corresponding sets of line segments in the higher 
  // order system (via keeping track of which commands spawned which child commands)


  // these functions are only to provide appropriate callback targets - maybe handle this in 
  // jura::Snowflake...
  void setResetRatio1( double newRatio)  { setResetRatio( 0, newRatio);  }
  void setResetOffset1(double newOffset) { setResetOffset(0, newOffset); }
  void setResetRatio2( double newRatio)  { setResetRatio( 1, newRatio);  }
  void setResetOffset2(double newOffset) { setResetOffset(1, newOffset); }

  // ....and keep only these here:

  /** Sets the reset interval as a ratio with respect to the length of the cycle (i.e. the number 
  of lines). If there are 100 lines and this parameter is set to 1, the turtle drawer will be 
  reset into its initial state after drawing 100 lines. If it's set to 0, it will never reset, i.e.
  it set the turtle into free-running mode. In general, it will reset after numLines/ratio. If this 
  results in a non-integer number, we will use jittering to attain the desired interval on the 
  average. For example, if numLines/ratio = 100.25 when there are 100 lines, it will reset after 
  100 lines 3 times, then once after 101 lines and so on. */
  void setResetRatio( int index, double newRatio);

  /** Sets a frequency dependent offset to the ratio that is set up by setResetRatio. The frequency
  scaling ha the effect that the apparent modulation frequency resulting for the offset will stay
  constant across the keyboard. */
  void setResetOffset(int index, double newOffset);

  /** Sets the method that is used to interpolate between the sequence of points that the turtle 
  generates. */
  void setInterpolationMode(int newMode) { interpolation = newMode; }

  /** Selects whether or not a precomputed (wave) table should be used or samples should be 
  computed from the turtle commands on the fly. The behavior is a bit different in both cases.
  Using a table, the 'f' turtle command does not behave properly and turn angle modulation is
  prohibitively expensive. On the other hand, the table-based implementation allows fo stereo
  detuning and should be generally more efficient (verify this). */
  void setUseTable(bool shouldUseTable) { useTable = shouldUseTable; }

  /** Experimental feature.. */
  void setAntiAlias(bool shouldAntiAlias) { antiAlias = shouldAntiAlias; }

  //void setStereoDetune(double newDetune);
  //void setStereoFrequencyOffset(double newOffset);
  // these are relevant only in table-mode - maybe use the regular wavetable oscillator

  /** Sets the turning angle for the turtle-graphics interpreter. */
  void setTurnAngle(double newAngle);

  void setSkew(double newAngle); // nope - that doesn't work as expected

  //-----------------------------------------------------------------------------------------------
  // \name Inquiry

  /** Returns the number of lines that the turtle would produce according to the current command 
  string. */
  virtual int getNumTurtleLines();

  /** Returns the number of lines produced since the last reset of lineCount. (mostly for figuring
  out resetting behavior) */
  //int getLineCount() const { return lineCount; }

  //-----------------------------------------------------------------------------------------------
  // \name Processing

  /** Calculates one output-sample frame at a time. */
  INLINE void getSampleFrameStereo(double *outL, double *outR)
  {
    // some checks (optimize - have a single readyToPlay flag so we only need one check here):
    if(numLines < 1)                return;
    if(!tableUpToDate && useTable)  updateWaveTable();
    //if(!incUpToDate)                updateIncrement();

    // integer and fractional part of position:
    int iPos = floorInt(pos);
    double fPos = pos - iPos;
    if(iPos != lineIndex)
      goToLineSegment(iPos);

    // read out buffered line segment (x[], y[] members) with interpolation:
    interpolate(outL, outR, fPos);
    *outL = normalizer * amplitude * (*outL - meanX);
    *outR = normalizer * amplitude * (*outR - meanY);
    rotator.apply(outL, outR);

    if(!incUpToDate)      // test: update increment here instead of at the beginning of the 
      updateIncrement();  // function - right before using it

    updatePosition();
  }

  /** Resets the state of the object, such that we start at 0,0 and head towards 1,0 (in 
  unnormalized coordinates). */
  void reset();


protected:

  //-----------------------------------------------------------------------------------------------
  // \name Misc

  /** Updates our readout position variable by incrementing it and taking care of the required 
  wraparound(s). */
  INLINE void updatePosition()
  {
    // increment and wraparound:
    pos += inc;
    while(pos >= numLines) pos -= numLines;
    while(pos <  0)        pos += numLines;
  }

  /** Reads out our line-segment buffer with interpolation and assigns the output for left and 
  right channel accordingly. */
  INLINE void interpolate(double *left, double *right, double frac)
  {
    switch(interpolation)
    {
    case LEFT_NEIGHBOUR: {
      *left  = x[0];
      *right = y[0]; } break;
    case RIGHT_NEIGHBOUR: {
      *left  = x[1];
      *right = y[1]; } break;
    case NEAREST_NEIGHBOUR: {
      if(frac >= 0.5) { *left = x[0]; *right = y[0]; }
      else            { *left = x[1]; *right = y[1]; } } break;
    case LINEAR: {
      *left  = (1-frac)*x[0] + frac*x[1];
      *right = (1-frac)*y[0] + frac*y[1]; } break;
    default: {
      *left  = 0;
      *right = 0;
      rsAssertFalse; } // unknown interpolation setting 
    }
    // or maybe call the 1st three floor, ceil, round, ..at least on the GUI
  }

  /** Resets only the turtle into its initial state, leaving the other state variables as is. Used
  internally in reset (which resets everything)) and for resetting after a number of lines has been
  rendered. */
  void resetTurtle();

  /** Resets our counters that are used for triggering resets of the turtle drawing algo. */
  void resetCounters();

  /** Makes the buffers x[0],x[1],y[0],y[1] etc. reflect the line endpoints of the given index for
  the target line.  */
  void goToLineSegment(int targetLineIndex);

  /** Goes from the current line segment (defined by member lineIndex) to the next, possibly 
  including a wraparound. */
  void goToNextLineSegment();

  /** Updates the x,y arrays that hold the endpoint of the line that is currently being drawn, i.e. 
  the points between which we currently interpolate. */
  void updateLineBuffer();

  /** Renders the wavetable and updates related variables. */
  void updateWaveTable();

  /** Updates our array of indices of the line-drawing commands inside the turtle command 
  string. */
  void updateLineCommandIndices();

  /** Updates all the resetters. */
  void updateResetters();

  /** Updates the resetter with the given index according to the resetter user parameters. */
  void updateResetter(int index);

  /** Updates the wavetable increment according to desired frequency, sample rate and number of 
  lines. */
  void updateIncrement();
   
  /** Updates our meanX, meanY, normalizer members according to the current turtleCommands and 
  angle settings. */
  void updateMeanAndNormalizer();

  /** Only for debugging - checks, if the lineIndex and commandIndex are in sync. */
  bool checkIndexConsistency();

  //-----------------------------------------------------------------------------------------------
  // \name Data

  // state for on-the fly rendering:
  double pos = 0;                 // position in wavetable / string
  double inc = 0;                 // wavetable increment
  double startPos = 0;            // start position, normalized to 0..1
  double meanX = 0, meanY = 0;    // mean values of x,y coordinates in one cycle
  double normalizer = 1;          // scales outputs such that -1 <= x,y <= +1 for all points
  double x[2], y[2];              // x[0]: point we come from, x[1]: point we go to, maybe apply a DC blocking filter to these x,y states

  int numLines       = 0;         // number of 'F's in turtleCommands
  int lineIndex      = 0;         // index of current line
  //int lineInc        = 1;         // increment to go from current to next line (+1 or -1)
  int startLineIndex = 0;

  std::string turtleCommands;          // string of drawing commands for the turtle
  int commandIndex = 0;                // current index in the list of turtle-commands
  int startCommandIndex = 0;           // index of the first command to be executed after reset
  std::vector<int> lineCommandIndices; // indices for the line commands

  // parameters:
  double amplitude      = 1;
  double frequency      = 100;
  double freqScaler     = 1;
  double sampleRate     = 44100;
  double turnAngle      = 0;
  double skew           = 0;
  int    cyclicReset    = 1;
  int    interpolation  = LINEAR;
  bool   antiAlias      = false;
  bool   useTable       = false;
  bool   reverse        = false;

  // resetters:
  static const int numResetters = 2;
  double resetRatios[numResetters];
  double resetOffsets[numResetters];
  ResetCounter resetters[numResetters];
  // todo: have also reversers that switch periodically between forward and backward drawing

  // rendering objects and related variables:
  TurtleGraphics turtle;
  RAPT::rsRotationXY<double> rotator; // for rotating final x,y coordinates


  // for optional table-based synthesis (maybe at some point we can drop that?)
  std::vector<double> tableX, tableY;  // rendered (wave)tables for x (left) and y (right)

  rsEngineersFilterStereo turtleLowpass; 
    // lowpass applied to turtle output for anti-aliasing ...check, if the filter coeffs have the
    // correct limit, i.e. go into bypass mode when cutoff == fs/2


  // flags to indicate whether or not various rendering state variables are up to date:

  std::atomic_bool tableUpToDate = false; // maybe rename to tableReady ...get rid
  std::atomic_bool incUpToDate = false;
  // this is a new way of dealing with updating internal variables - it avoids redundant 
  // recalculations when sereval parameters change at once and allows the re-calculation to be
  // done in a thread-safe way in the audio thread - the gui thread just atomically sets these
  // flags...maybe if we do it everywhere like this, we can even get rid of locking in 
  // jura::Parameter...that would be great!!

};

}

#endif
Snowflake::Snowflake()
{
  // init to order 4 Koch snowflake:
  turnAngle = 60;
  turtle.setAngle(turnAngle);
  axiom = "F--F--F--";
  clearRules();
  addRule('F', "F+F--F+F");
  numIterations = 4;

  updateTurtleCommands();
  updateWaveTable();
}

void Snowflake::setSampleRate(double newSampleRate)
{
  sampleRate = newSampleRate;
  incUpToDate = false;
}

void Snowflake::setFrequency(double newFrequency)
{
  frequency = newFrequency;
  incUpToDate = false;
}

void Snowflake::setRotation(double newRotation)
{
  rotator.setAngle((PI/180) * newRotation);
}

void Snowflake::setResetAfterCycles(int numCycles) 
{ 
  cyclicReset = numCycles; 


  //cycleCount = 0; 
  // resetting the cycle count is required here because we may be running since ages without reset 
  // and it could be at -2341234 or something - and we don't want to wait that long for the next 
  // reset ...nope - not anymore - it's clamped to 0 now when resetting is off
}

void Snowflake::setNumIterations(int newNumIterations) 
{ 
  if(newNumIterations == numIterations)
    return;
  numIterations = newNumIterations; 
  commandsReady = false;
  tableUpToDate = false; 
}

void Snowflake::setAngle(double newAngle) 
{
  turnAngle = newAngle;
  turtle.setAngle(turnAngle);
  updateMeanAndNormalizer();
  tableUpToDate = false; 
}

void Snowflake::clearRules() 
{ 
  lindSys.clearRules(); 
  commandsReady = false;
  tableUpToDate = false; 
}

void Snowflake::Snowflake::addRule(char input, const std::string& output) 
{ 
  lindSys.addRule(input, output);
  commandsReady = false;
  tableUpToDate = false; 
}

void Snowflake::setAxiom(const std::string& newAxiom) 
{ 
  axiom = newAxiom; 
  commandsReady = false;
  tableUpToDate = false; 
}

void Snowflake::updateAllInternals()
{
  if(!commandsReady) updateTurtleCommands();
  if(!incUpToDate)   updateIncrement();
  if(!tableUpToDate) updateWaveTable();
}

void Snowflake::updateWaveTable()
{
  if(!commandsReady)
    updateTurtleCommands();  // updates also mean values, normalizer, increment and resets

  TurtleGraphics tmpTurtle;
  tmpTurtle.setAngle(turnAngle);
  tmpTurtle.translate(turtleCommands, tableX, tableY);

  tableUpToDate = true;
}

void Snowflake::updateTurtleCommands()
{
  lindenmayerResult = lindSys.apply(axiom, numIterations);
  turtleCommands = turtle.extractCommands(lindenmayerResult);
  numLines = turtle.getNumberOfLines(turtleCommands); 
  updateMeanAndNormalizer();
  commandsReady = true;
  updateIncrement();
  reset();
}

void Snowflake::reset()
{
  pos = 0;
  cycleCount = 0;
  resetTurtle();
  updateXY();      // rename to drawNextLineToBuffer
  lineIndex = 0;


  //lineIndex = -1;     // try to rewrite the code, so we can set it to 0 here - i think it's only there
                      // because goToLineSegment initially increments it but...
  //goToLineSegment(0); 
  // ..but why is this needed? ...a...i think, because the turtle must draw the first
  // line (update x,y buffers) ...maybe we can just call updateXY and get rid of
  // the call above?


}

void Snowflake::resetTurtle()
{
  commandIndex = 0;
  turtle.init(0, 0, 1, 0);
}

void Snowflake::goToLineSegment(int targetLineIndex)
{
  if(numLines == 0) { x[0] = y[0] = x[1] = y[1] = 0; return; }
  if(useTable)
  {
    //lineIndex = targetLineIndex;
    //x[0] = tableX[lineIndex];
    //y[0] = tableY[lineIndex];
    //x[1] = tableX[lineIndex+1];
    //y[1] = tableY[lineIndex+1];
    // not yet used - maybe we don't need it, if we want to doe special things in wavetable mode
    // such as stereo detuning - if that's useless, uncomment code above and refactor the
    // getSampleFrame... fucntions
  }
  else
  {
    while(lineIndex != targetLineIndex)
      goToNextLineSegment(); // increments lineIndex with wrap around
  }

  // later: update interpolator coeffs here according to x,y buffers
}

void Snowflake::goToNextLineSegment()
{
  updateXY();
  lineIndex++;
  if(lineIndex == numLines) {
    lineIndex = 0;
    cycleCount++;
    if(cyclicReset == 0)
      cycleCount = 0;
    else if(cycleCount >= cyclicReset) {
      cycleCount = 0;
      resetTurtle();
      updateXY();
    }
  }
}


// make reset of the turtle optional, or reset after a certain number of cycles, 0: never
// reset. the turtle is free running in this case. when the angle is slightly off the perfect
// value, the picture rotates - but we need to make sure to define axioms in a way that they
// head into the same direction as in the beginning after completing the cycle, for example
// "F--F--F--" instead of "F--F--F" for the initial triangle for the koch snowflake
// maybe have a soft-reset parameter that resets only partially (i.e. interpolates between
// current and initial state )


void Snowflake::updateXY()  // // rename to drawNextLineToBuffer
{
  bool xyUpdated = false;
  while(xyUpdated == false) {
    bool draw = turtle.interpretCharacter(turtleCommands[commandIndex]);
    commandIndex++;
    if(commandIndex == turtleCommands.size())
      commandIndex = 0;
    if(draw) {
      x[0] = turtle.getStartX();
      y[0] = turtle.getStartY();
      x[1] = turtle.getEndX();
      y[1] = turtle.getEndY();
      xyUpdated = true;
    }
  }
}

void Snowflake::updateIncrement()
{
  inc = numLines * frequency / sampleRate; // avoid division
  incUpToDate = true;
}

void Snowflake::updateMeanAndNormalizer()
{
  // run through all turtle commands once, thereby keep track of the mean and min/max values of the
  // generated curve, then set our meanX, meanY, normalizer members accordingly

  double minX = 0, maxX = 0, minY = 0, maxY = 0, sumX = 0, sumY = 0;
  TurtleGraphics tmpTurtle;
  tmpTurtle.setAngle(turnAngle);
  tmpTurtle.init();
  int N = (int) turtleCommands.size();
  for(int i = 0; i < N; i++) {
    bool lineDrawn = tmpTurtle.interpretCharacter(turtleCommands[i]);
    if( lineDrawn == true )  {
      double x = tmpTurtle.getX();
      double y = tmpTurtle.getY();
      minX  = rmin(minX, x);
      maxX  = rmax(maxX, x);
      minY  = rmin(minY, y);
      maxY  = rmax(maxY, y);
      sumX += x;
      sumY += y;
    }
  }
  double tmp = 1.0 / numLines; // maybe have a member for that (optimization)
  meanX = sumX * tmp;
  meanY = sumY * tmp;
  minX -= meanX; maxX -= meanX;
  minY -= meanY; maxY -= meanY;
  maxX  = rmax(fabs(minX), fabs(maxX));
  maxY  = rmax(fabs(minY), fabs(maxY));
  normalizer = 1.0 / rmax(maxX, maxY);

  // in free running mode, this so computed mean is sometimes (often) wrong because one cycle is 
  // only part (for example half) of a larger shape - in this case, we would need the mean over two 
  // cycles ...maybe we can somehow deduce the symmetry in order to compute a better mean? maybe we
  // should offer different normalization modes...maybe have a highpass and leveller running on it
  // in realtime (but not oversampled)
}


/*
Ideas: 

-make TurningAngle modulatable
 -optionally compensate for changes in turning-angle by a rotation of the whole image, i.e. rotate
  image by -TurningAngle ..or maybe -compensationAmount*TurningAngle
 -requires to not pre-render a wavetable but only render the L-system output string and interpret 
  it on the fly (the string should be stripped from meaningless characters, retaining only F,+,- 
  before)
 -may require to compute shift and scale (to remove DC and normalize) when a new angle is set up
  -maybe these values should be precomputed for various angles, for example 0,5,10,15,.. and 
   interpolation be used (to avoid to expensively compute the actually desired value whenever the
   angle changes)
-allow a turtle syntax like +30 or -45 to mean: turn 30� left or 45� right (instead of whatever 
 value the turtle drawer is set to)
 -allows an initiator to use different angles than the generator(s) - for example, start with a 
  pentagon and replace edges with something triangular (as in the koch snowflake)
 -to make that more efficiently parsable in realtime, replace the + by P and the - by M in the 
  realtime-parsed string - avoids to check, if next char is a number whenever a + or - is 
  encountered, P and M indicate that is is, + and - indicate that it isn't, maybe allow rational
  numbers to be entered, like 20/3 for 6.66666� but also allow to use decimal dot (for numerator
  and denominator), assume /1 when a / is encountered without any number thereafter
-make rotation angle quantizable to a set of numbers that the user can enter, for example
 120,90,72,60,45,36,30,24,22.5,20,15,12,10
-have different loop modes: forward, backward, alternating (avoids jumps for non-closed curves)
 -maybe in backward passes, (optionally) interpret angles as their negative (necessarry to actually
  "go back")
-have a "reset" parameter that determines the number of cycles (through the command list) after 
 which the turtle state is reset into its initial state, interpret "0" as "never" - relevant for
 on-the-fly rendering with "weird" turn angles
-maybe have an additional string of turtle commands that can be applied after each cycle has passed
 (empty by default)...or maybe even different command strings to be applied after different numbers 
 of cycles ..this defines the "turn/wrap around behavior"
-instead of having a single L-system, have a set of them, maybe named A,B,C,.., then instead of 
 setting a number of iterations, use a string AAABBCCA to mean: apply A 3 times, then B 2 times, 
 then  C 2 times then A once, allow also syntax like A3B2C2A1, where the final 1 is optional
 ...maybe allow also things like (AABA)^2 (BBA^3)...or ((AABA)2(BBA)3)2
 -on wikipedia, there's a sierpinski triangle version that requires G to interpreted like F - this
  could be realized by defining:
  axiom: F-G-G, rules: (A: F=F-G+F+G-F; G=GG) (B: G=F) and then doing AAAB (or A3B) for 3rd order
  ...maybe the parser doesn't have to care about the parentheses - it may separate the system
  definition by the colon, but they make it more readable...but maybe newlines could also be used
  A: F=F-G+F+G-F; G=GG
  B: G=F
 -maybe this best realized by making a class LindenmayerSystemSet
-allow the left hand sides of rules to be strings instead of characters. this allows a context 
 sensitive replacement, for example a rule +F+=+F-F-F+ means: replace F by F-F-F, if the F is 
 between + and + (the surrounding "context" plusses appear in the rhs, too - so the rule won't 
 have them removed them in the output string)
-maybe rename the "Axiom" back to "Seed"
-interpolation modes: left, right, nearest, linear, cubic
-allow a traversal speed (or better: duration/length) to be associated with each point (i.e. the 
 line segment that goes toward that point)...like a normal 'F' would assume 1, an 'f' would 
 assume '0', but we could also have F2 to let it take 2 time units to traverse the segment or 
 1/3 to take one thrid of the time unit, the increment computation would then not use "numLines" 
 as multiplier but sum-of-traversal-duration
-optionally "upsample" the turtle commands like F+F-FF -> x3 -> FFF+FFF-FFFFFF, this givens a finer
 resulution of lines. by itself, it's useless but now we apply additional bending inside ths line
 (like turning by a few degrees after each F)
-randomize turn angles, user can set seed and distribution (uniform, bell, bimodal, etc.)
-randomize step lengths
-maybe let the turle have scaleX, scaleY members that are applied in each step - these can be 
 modulated and/or randomized
-have a fine turning angle delta (maybe 0 to 10%) with keytracking
-DC blocker and leveller may replace or complement normalization (it doesn't always wrk right, 
 especially in free running mode)
-maybe allow for a non-integer number of iterations, 3.7 would crossfade between the result from
 3 and 4 iterations with weights 0.3 and 0.7 - maybe we should precompute and store the strings
 for all orders up to something (don't recompute strings on the fly when the user modulates 
 numIterations)
-factor out a class TurtleGraphicsGenerator - that can then also be used for turtle-strings 
 generated by other means (like finite automata using christoffel words or whatever)

-call the whole synthesis method Fractal Geometric Synthesis (FG-synthesis), the extended 
 Lindenmayer/Turtle grammar Fractal Definition Language (FDL) or maybe fractal geometric synthesis
 language (FGSL)...or maybe Fractal Pattern Synthesis
-write a tutorial:
 1: Turtle Graphics
 2: Lindenmayer Systems
 3: Fractal Definition Language
  3.1: Extensions to Turtle Syntax
  3.2: Extensions to Lindenmayer Syntax
 4: Fractal Geometry Synthesis
  4.1: Turning Angle Modulation ...maybe allow Step Size Modulation, too?
  4.2: Loop Modes
  4.3: Normalization Modes
  4.5: Subtractive Post Processing
   4.5.1 Regular Musical Filters
   4.5.2 Linear Phase Filters
  4.6: Rational Numbers and Harmonics
   4.5.2 Number of Segments (N) vs Segment Length (L)

Sound Design Guide:
-if you want the free-running behavior to match the reset-mode, you need to make sure that the
 turtle heads into the same direction as initially after it completed a cycle around the seed, i.e,
 for a square seed (at 90�) don't use F+F+F+F but F+F+F+F+, the additionla plus at the end makes 
 the turtle look to the right again after completing the square
-an turn angle sligtly off from the ideal value lets the picture slowly rotate in free-running mode
-rules with branches create nice overtone structures (branches introduce discontinuities in the 
 waveshape)
-to analyze a patch:
 -stop rotation (if any) by setting turning angle to precise value and/or use resetting
 -turn the numIterations to zero to see the seed
 -turn it to 1 to see the rule, maybe use a simple 'F' seed to see the pure rule


see here for inspiration for new curves
https://www.youtube.com/watch?v=RU0wScIj36o&t=53s

*/
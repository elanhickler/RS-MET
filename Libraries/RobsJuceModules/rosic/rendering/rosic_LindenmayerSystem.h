#ifndef rosic_LindenmayerSystem_h
#define rosic_LindenmayerSystem_h

namespace rosic
{

/** Implements a Lindenmayer system (aka L-system). This is a system that iteratively replaces
certain characters in a string by replacement strings according to a set of replacement rules.
When the resulting strings are translated to graphics by interpreting some of the characters as
drawing commands (such as 'F': go forward, '+': turn left, '-': turn right), curves with
self-similar features can be generated.

References:
https://de.wikipedia.org/wiki/Lindenmayer-System
https://en.wikipedia.org/wiki/L-system
http://algorithmicbotany.org/papers/abop/abop-ch1.pdf
http://www.abrazol.com/books/patterngen/
http://www.sidefx.com/docs/houdini/nodes/sop/lsystem.html
http://www.sccg.sk/~smolenova/elearning/ks_fmfiuk06.pdf
*/

class LindenmayerSystem
{

public:

  /** Adds a replacement rule to our set of rules.  */
  void addRule(char input, const std::string& output);

  /** Clears the set of replacement rules. */
  void clearRules();

  /** Applies the set of replacement rules to the given character. If a matching rule-input
  character is found, it returns the corresponding output string, otherwise the character itself
  will be returned as string of length 1. */
  std::string apply(char c);

  /** Applies the set of replacement rules to the given string once and returns the result. */
  std::string apply(const std::string& input);

  /** Iteratively applies the set of replacement rules the given number of times and returns the
  result. In L-system jargon, the "input" string is also called "axiom" and the numberOfTimes is
  called order. */
  std::string apply(const std::string& input, int numberOfTimes);


protected:

  std::vector<char> ruleInputs;
  std::vector<std::string> ruleOutputs;

};

//=================================================================================================

/** Encapsulates a LindenmayerSytem (as baseclass) and a TurtleGraphics object (as member) to make
rendering of L-system generatad point-sequences more convenient. */

class LindenmayerRenderer : public LindenmayerSystem
{

public:

  /** Sets the turning angle for the turtle graphics renderer. */
  void setAngle(double degrees) { turtleGraphics.setAngle(degrees); }

  /** Turns normalization on/off. The normalization also includes DC removal. */
  void setNormalization(bool shouldNormalize) { normalize = shouldNormalize; }




  /** Produces the array of verices for a Koch snowflake of given order.
  see: https://en.wikipedia.org/wiki/Koch_snowflake */
  void getKochSnowflake(int order, std::vector<double>& x, std::vector<double>& y);

  /** Produces the array of verices for a Moore curve of given order.
  see: https://en.wikipedia.org/wiki/Moore_curve */
  void getMooreCurve(int order, std::vector<double>& x, std::vector<double>& y);

  // closed curves taken from: http://mathforum.org/advanced/robertd/lsys2d.html
  void get32SegmentCurve(int order, std::vector<double>& x, std::vector<double>& y);
  void getQuadraticKochIsland(int order, std::vector<double>& x, std::vector<double>& y);
  void getSquareCurve(int order, std::vector<double>& x, std::vector<double>& y);
  void getSierpinskiTriangle(int order, std::vector<double>& x, std::vector<double>& y);

  // closed curves taken from http://www.kevs3d.co.uk/dev/lsystems/
  void getSierpinskiTriangle2(int order, std::vector<double>& x, std::vector<double>& y);
    // this doesn't work - why?
  void getPleasantError(int order, std::vector<double>& x, std::vector<double>& y);

  // see also here - there seems to be an L-system in inkscape - maybe i can figure out why the
  // 2nd sierpinske triangle doesn't work by looking at what inkscape does
  //https://thebrickinthesky.wordpress.com/2013/03/17/l-systems-and-penrose-p3-in-inkscape/


  // maybe have a "numPoints" parameter that is used for resampling the resulting curve to a given
  // number of points (by linear interpolation)

  /** Given our set of rules, this function iteratively applies the text-replacement up to the
  given order and then renders the resulting string into a sequence of x,y points using turtle
  graphics with given angle. */
  void render(const std::string& seed, int order, double angle,
    std::vector<double>& x, std::vector<double>& y);

  /** Renders the given Lindenmayer string into a sequence of points using the given turning
  angle. */
  void translate(const std::string& str, double angle,
    std::vector<double>& x, std::vector<double>& y);
  // maybe get rid of the render/translate functions that take an angle parameter



  void render(const std::string& seed, int order, std::vector<double>& x, std::vector<double>& y);

  void translate(const std::string& str, std::vector<double>& x, std::vector<double>& y);



  /** Normalizes the xy coordinates such that both x and y are free of DC (centered around 0) and
  the maximum absolute value is 1 (this second step is done by finding the maximum absolute value
  of both vectors and scaling both by the same value - in order to preserve the aspect ratio). */
  void normalizeXY(std::vector<double>& x, std::vector<double>& y);

protected:

  TurtleGraphics turtleGraphics;

  bool normalize = true;

};

}

#endif
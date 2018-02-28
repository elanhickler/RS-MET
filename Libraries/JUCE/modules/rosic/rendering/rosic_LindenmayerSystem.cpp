//#include "LindenmayerSystem.h"

void LindenmayerSystem::addRule(char input, const std::string& output)
{
  ruleInputs.push_back(input);
  ruleOutputs.push_back(output);
}

void LindenmayerSystem::clearRules()
{
  ruleInputs.clear();
  ruleOutputs.clear();
}

std::string LindenmayerSystem::apply(char c)
{
  for(size_t i = 0; i < ruleInputs.size(); i++)
    if( ruleInputs[i] == c )
      return ruleOutputs[i];
  return std::string(1, c);   // 'c' repeated one time
}

std::string LindenmayerSystem::apply(const std::string& s)
{
  std::string r; // result
  for(int i = 0; i < s.length(); i++)
    r += apply(s[i]);
  return r;
}

std::string LindenmayerSystem::apply(const std::string& s, int num)
{
  std::string r = s;
  for(int i = 1; i <= num; i++)
    r = apply(r);
  return r; 
}

//=================================================================================================

void TurtleGraphics::setAngle(double degrees)
{
  double radians = RAPT::rsDegreeToRadiant(degrees);
  //long double factor =  (PI / 180.0);
  //double radians = (double) (factor * (long double) degrees);
  rotLeft.setAngle(  radians);
  rotRight.setAngle(-radians); // get rid, implement applyInverse
  // entries of the rotation matrices that should be 0 are at 6.123e-17 - probably not an issue in 
  // practice but not nice - maybe clamp to zero anything below 1.e-16
}

void TurtleGraphics::init(double _x, double _y, double _dx, double _dy, bool keepOldXY)
{
  if(keepOldXY) { xo = x; yo = y; } // either keep current position in old position (xo,yo) (to
  else          { xo = 0; yo = 0; } // draw a connection) or reset them to 0 as well

  x  = _x;
  y  = _y;
  dx = _dx;
  dy = _dy;

  stateStack.clear();
}

void TurtleGraphics::goForward()
{
  xo = x;
  yo = y;
  x += dx;
  y += dy;
}

void TurtleGraphics::turnLeft()
{
  rotLeft.apply(&dx, &dy);
}

void TurtleGraphics::turnRight()
{
  rotRight.apply(&dx, &dy); // use rotation.applyInverse
}

void TurtleGraphics::pushState()
{
  stateStack.push_back(TurtleState(x, y, dx, dy, xo, yo));
}

void TurtleGraphics::popState()
{
  rsAssert(stateStack.size() > 0);
  if(stateStack.size() == 0)
    return;
  TurtleState s = RAPT::rsGetAndRemoveLast(stateStack);
  x  = s.x;   y = s.y;
  dx = s.dx; dy = s.dy;
  xo = s.xo; yo = s.yo;
}

void TurtleGraphics::translate(const std::string& str, 
  std::vector<double>& vx, std::vector<double>& vy)
{
  // clear and add initial vertex:
  vx.clear();
  vy.clear();
  vx.push_back(getX()); 
  vy.push_back(getY()); 

  // loop through the string and add vertices as needed:
  for(int i = 0; i < str.size(); i++) {
    if(interpretCharacter(str[i])) {
      vx.push_back(getX()); 
      vy.push_back(getY()); 
    }
  }
}

bool TurtleGraphics::interpretCharacter(char c)
{
  if(c == 'F') { goForward(); return true;  }
  if(c == '+') { turnLeft();  return false; }
  if(c == '-') { turnRight(); return false; }
  if(c == 'f') { goForward(); return false; }
  if(c == 'G') { goForward(); return true;  }
  if(c == '[') { pushState(); return false; }
  if(c == ']') { popState();  return false; }
  return false;

  // maybe use commands like +30F-45F to turn 30� left, go forward, turn 45� right, go forward
  // ...so whenever a number comes after a + or -, don't use the default angle that is set up but 
  // the angle given/ by that number - maybe don't call standard turnLeft but a turnLeftBy(angle)
  // function, same for right - but the same effect can be also achieved by using 15 degrees
  // and doing ++F---F, so maybe it doens't really formally extend the possibilities - but some
  // things could be expressed more conveniently...one could use a pentagonal initiator with 
  // triangular generators - expressing this with +++++..., would require to set the angle to 2�
  // and using an excessive number of +ses
}

std::string TurtleGraphics::extractCommands(const std::string& s)
{
  std::string tmp;
  for(int i = 0; i < s.size(); i++)
    if(isCommand(s[i])) tmp += s[i];
  return tmp;
}

bool TurtleGraphics::isCommand(char c)
{
  return c == '+' || c == '-' || c == 'F' || c == 'f' || c == 'G' || c == '[' || c == ']';
}

int TurtleGraphics::getNumberOfLines(const std::string& s)
{
  int n = 0;
  for(int i = 0; i < s.size(); i++)
    if(s[i] == 'F' || s[i] == 'G') n++;
  return n;
}

//=================================================================================================

void LindenmayerRenderer::getKochSnowflake(int N, std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "F+F--F+F");
  render("F--F--F", N, 60, x, y);
}

void LindenmayerRenderer::getMooreCurve(int N, std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('L', "-RF+LFL+FR-");
  addRule('R', "+LF-RFR-FL+");
  render("LFL+F+LFL", N, 90, x, y);
}

void LindenmayerRenderer::get32SegmentCurve(int N, std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "-F+F-F-F+F+FF-F+F+FF+F-F-FF+FF-FF+F+F-FF-F-F+FF-F-F+F+F-F+");
  render("F+F+F+F", N, 90, x, y);
}

void LindenmayerRenderer::getQuadraticKochIsland(int N, 
  std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "F-F+F+FFF-F-F+F");
  render("F+F+F+F", N, 90, x, y);
}

void LindenmayerRenderer::getSquareCurve(int N, std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('X', "XF-F+F-XF+F+XF-F+F-X");
  render("F+XF+F+XF", N, 90, x, y);
}

void LindenmayerRenderer::getSierpinskiTriangle(int N, 
  std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "FF");
  addRule('X', "--FXF++FXF++FXF--");
  render("FXF--FF--FF", N, 60, x, y);
}

void LindenmayerRenderer::getSierpinskiTriangle2(int N, 
  std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "F-G+F+G-F");
  addRule('G', "GG");
  render("F-G-G", N, 120, x, y);
}

void LindenmayerRenderer::getPleasantError(int N, std::vector<double>& x, std::vector<double>& y)
{
  clearRules();
  addRule('F', "F-F++F+F-F-F");         // original
  //addRule('F', "F-F++F+F-F-F+F-F");     // variation 1
  //addRule('F', "F-F++F+F-F-F-F+F");
  //addRule('F', "F-F++F+F-F-F+F+F-F-F");
  //addRule('F', "F-F++F+F-F-F-F-F+F+F");
  render("F-F-F-F-F", N, 72, x, y);
  // hmm...it seems, i need to code a module where the user can enter L-system rules, axioms, etc.
  // i cannot hardcode all interesting curves - maybe it should be possible to detune left/right
}

void LindenmayerRenderer::render(const std::string& seed, int order, double angle, 
  std::vector<double>& x, std::vector<double>& y)
{
  turtleGraphics.setAngle(angle);
  render(seed, order, x, y);
}

void LindenmayerRenderer::translate(const std::string& str, double angle,
  std::vector<double>& x, std::vector<double>& y)
{
  turtleGraphics.setAngle(angle);
  translate(str, x, y);
}

void LindenmayerRenderer::render(const std::string& seed, int order, 
  std::vector<double>& x, std::vector<double>& y)
{
  std::string str = apply(seed, order);
  translate(str, x, y);
}

void LindenmayerRenderer::translate(const std::string& str, 
  std::vector<double>& x, std::vector<double>& y)
{
  turtleGraphics.init(0, 0, 1, 0);
  turtleGraphics.translate(str, x, y);
  if(normalize) 
    normalizeXY(x, y);
}

void LindenmayerRenderer::normalizeXY(std::vector<double>& x, std::vector<double>& y)
{
  int N = (int)x.size();

  // remove mean:
  bool loop = true;  // make member
  if(loop == true) { // we need to ignore last element in mean computation
    double m;        // because it just repeats the first and is irrelevant
    m = mean(&x[0], N-1); add(&x[0], -m, &x[0], N);
    m = mean(&y[0], N-1); add(&y[0], -m, &y[0], N); }
  else {
    removeMean(&x[0], N);
    removeMean(&y[0], N); }

  // adjust maximum:
  double maxX = maxAbs(&x[0], N);
  double maxY = maxAbs(&y[0], N);
  double scl = 1.0 / max(maxX, maxY);
  scale(&x[0], &x[0], N, scl);
  scale(&y[0], &y[0], N, scl);
}

// other closed curves that can be generated:
// http://mathforum.org/advanced/robertd/lsys2d.html (many curves with L-system rules)
// http://www.kevs3d.co.uk/dev/lsystems/ (applet with examples)

// https://www.cut-the-knot.org/do_you_know/hilbert.shtml

// not sure, if doable by L-system:
// https://en.wikipedia.org/wiki/Sierpi%C5%84ski_curve
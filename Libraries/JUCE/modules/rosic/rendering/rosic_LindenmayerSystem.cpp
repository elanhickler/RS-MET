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
  rotRight.setAngle(-radians);
  // entries of the rotation matrices that should be 0 are at 6.123e-17 - probably not an issue in 
  // practice but not nice - maybe clamp to zero anything below 1.e-16
}

void TurtleGraphics::init(double _x, double _y, double _dx, double _dy)
{
  x  = _x;
  y  = _y;
  dx = _dx;
  dy = _dy;
}

void TurtleGraphics::goForward()
{
  x += dx;
  y += dy;
}

void TurtleGraphics::turnLeft()
{
  rotLeft.apply(&dx, &dy);
}

void TurtleGraphics::turnRight()
{
  rotRight.apply(&dx, &dy);
}

void TurtleGraphics::translate(const std::string& str, 
  std::vector<double>& vx, std::vector<double>& vy)
{
  // clear and add initial vertex:
  vx.clear();
  vy.clear();
  vx.push_back(x); 
  vy.push_back(y); 

  // loop through the string and add vertices as needed:
  for(int i = 0; i < str.size(); i++) {
    if(str[i] == '+')   
      turnLeft();
    if(str[i] == '-')   
      turnRight();
    if(str[i] == 'f')    // this is nonstandard, standard would be to go forward but avoid
      goForward();       // drawing a line (that cannot expressed here)
    if(str[i] == 'F') { 
      goForward(); 
      vx.push_back(x); 
      vy.push_back(y); 
    }
  }
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
  addRule('F', "F-G+F+G-F");      // F=F-G+F+G-F
  addRule('G', "GG");             // G=GG
  render("F-G-G", N, 120, x, y);  // seed = F-G-G
  // does not work. it was taken from http://www.kevs3d.co.uk/dev/lsystems/ i thought, maybe the
  // rendere there interprets G as drawing command, but replacing it with X there changed nothing
  // (it still works there)
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
  std::string str = apply(seed, order);
  translate(str, angle, x, y);
}

void LindenmayerRenderer::translate(const std::string& str, double angle,
  std::vector<double>& x, std::vector<double>& y)
{
  turtleGraphics.init(0, 0, 1, 0);
  turtleGraphics.setAngle(angle);
  turtleGraphics.translate(str, x, y);
  if(normalize) 
    normalizeXY(x, y);
}

void LindenmayerRenderer::normalizeXY(std::vector<double>& x, std::vector<double>& y)
{
  int N = (int)x.size();

  // remove mean:
  bool loop = true;  // make member
  if(loop == true) { // we need to ignore 0th element in mean computation
    double m;        // because it's repeated at the end
    m = mean(&x[1], N-1); add(&x[0], -m, &x[0], N);
    m = mean(&y[1], N-1); add(&y[0], -m, &y[0], N); }
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
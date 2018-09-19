template<class TCor, class TWgt>
rsRealTimeSpline<TCor, TWgt>::rsRealTimeSpline()
{
  reset();
}

template<class TCor, class TWgt>
void rsRealTimeSpline<TCor, TWgt>::setDotBuffers(TCor* bufX, TCor* bufY, TWgt* bufWeights, 
  int bufLengths)
{
  dotsX = bufX;
  dotsY = bufY;
  dotsW = bufWeights;
  dotBufferLength = bufLengths;
}

template<class TCor, class TWgt>
int rsRealTimeSpline<TCor, TWgt>::getDotsForInputPoint(TCor inX, TCor inY)
{
  rsArray::pushFrontPopBack4(inX, x);
  rsArray::pushFrontPopBack4(inY, y);

  if(lineDensity == 0.f) {
    dotsX[0] = inX;
    dotsY[0] = inY;
    dotsW[0] = brightness;
    return;
  }

  switch(drawMode)
  {
  case LINEAR:        return dotsLinear();
  case CUBIC_HERMITE: return dotsCubicHermite();
  default:            return 0;
  }
}

template<class TCor, class TWgt>
void rsRealTimeSpline<TCor, TWgt>::reset(TCor x_, TCor y_)
{
  rsArray::fillWithValue(x, 4, x_);
  rsArray::fillWithValue(y, 4, y_);
  wOld = TWgt(0);
}

template<class TCor, class TWgt>
int rsRealTimeSpline<TCor, TWgt>::numDotsForSegment(TCor segmentLength)
{
  TCor minDotDistance = 1; // maybe make user-adjustable
  int numDots = rsMax(1, (int)floor(density*pixelDistance/minDotDistance));
  if(maxNumDots > -1)
    numDots = rsMin(numDots, maxNumDots);
  numDots = rsMin(numDots, dotBufferLength);
}

template<class TCor, class TWgt>
void rsRealTimeSpline<TCor, TWgt>::getStartAndEndWeights(int numDots, TWgt* wStart, TWgt* wEnd)
{
  bool scaleByNumDots = true; // make user adjustable
  TWgt scaler = TWgt(1);
  if(scaleByNumDots)
    scaler /= (TWgt)numDots;

  if(useGradient) {
    TWgt wt = brightness * scaler;   // target weight that would be used if we don't do gradients
    *wEnd = (2.f*ct) - wOld;         // desired endpoint color
    *wEnd = rsMax(*wEnd, ct);        // wEnd could come out negative, use wt as lower bound
    *wStart = wOld;
  }
  else {
    *wStart = brightness * scaler;
    *wEnd   = brightness * scaler;
  }
  wOld = *wEnd;
}

template<class TCor, class TWgt>
int rsRealTimeSpline<TCor, TWgt>::dotsLinear()
{
  // distance computation:
  TCor dx = x[1]-x[2];
  TCor dy = y[1]-y[2];
  TCor pixelDistance = sqrt(dx*dx + dy*dy);
  int numDots = numDotsForSegment(pixelDistance);

  // start/end weight computation:
  TWgt w1, w2, dw;
  getStartAndEndWeights(numDots, &w1, &w2);
  dw = w2-w1;

  // dot computation:
  TCor scaler = (TCor)(1.0 / numDots);
  TCor k;
  for(int i = 0; i < numDots; i++) {
    k = scaler * (i+1);     // == (i+1) / numDots
    X[i] = x[1] + k*dx;
    Y[i] = y[1] + k*dy;
    W[i] = w1 + TWgt(k)*dw;
  }
}

template<class TCor, class TWgt>
int rsRealTimeSpline<TCor, TWgt>::dotsCubicHermite()
{
  // compute (2nd order) derivative estimates at inner points x[1], x[2]:
  TCor dx1, dx2, dy1, dy2;
  dx1 = TCor(0.5)*(x[0]-x[2]);    // estimated dx/dt at x[1]
  dy1 = TCor(0.5)*(y[0]-y[2]);    // estimated dy/dt at y[1]
  dx2 = TCor(0.5)*(x[1]-x[3]);    // estimated dx/dt at x[2]
  dy2 = TCor(0.5)*(y[1]-y[3]);    // estimated dy/dt at y[2]

  // compute polynomial coeffs:
  TCor a[4], b[4];   // coeffs for x(t), y(t)
  cubicSplineArcCoeffs2D(x1, x1s, y1, y1s, x2, x2s, y2, y2s, a, b);

  // distance computation:
  bool desityCompensation = false;  // make user adjustable
  TCor splineLength;
  if(desityCompensation) {
    // obtain arc-length s as (sampled) function of parameter t:
    static const int N = 17; // make this user-adjustable (setDensityCompensationPrecision)
    r.resize(N);  // move into setDensityCompensationPrecision
    s.resize(N);

    rsArray::fillWithRangeLinear(&r[0], N, TCor(0), TCor(1));
    cubicSplineArcLength2D(a, b, &r[0], &s[0], N);
    splineLength = s[N-1]; // last value in s is total length: s(t=1)
  }
  else {
    // crude estimate:
    TCor dx = x[1]-x[2];
    TCor dy = y[1]-y[2];
    splineLength = sqrt(dx*dx + dy*dy);
  }


  int numDots = numDotsForSegment(splineLength);



  // dot computation:


  //painter.drawDottedSpline(
  //  x[2], dx2, y[2], dy2,      // older inner point comes first
  //  x[1], dx1, y[1], dy1,      // newer inner point comes second
  //  color1, color2, numDots);

}




// -functionality for this class is currently scattered over rsImagePainter and rsPhaseScopeBuffer

// Ideas:
// -make density normalization optional (it's expensive)
// -try to draw the spline with short line-segments
//  -maybe in this case, the higher density in regions of high curvature is actually is actually 
//   beneficial, so no density compensation is required?
// -try quartic interpolation spline 
//  -maybe less overshoot?
// -try to fix the 3rd deruvative at the joints to 0
//  -needs a quinitc spline
// -try a simpler (to compute) density compensation:
//  -evaluate instantaneous speed sqrt( (dx/dt)^2 + (dy/dt)^2 ) at each spline point and skip dots or
//   insert extra dots depending on the value
//  -maybe use |dx/dt| + |dy/dt| instead of the sqrt
//  -maybe instead of skip/insert dots, change the brightness of the dot to be drawn

// try a quadratic spline:
// x(t) = a0 + a1*t + a2*t^2, x'(t) = a1 + 2*a2*t
// y(t) = b0 + b1*t + b2*t^2, y'(t) = b1 + 2*b2*t
// and require only dy/dx = x'/y' to be equal at the joints. This is less restrictive than 
// requiring dx/dt and dy/dt to be simultaneously equal. Does that also ensure parametric 
// continuity or only geometric continuity (i think, the former)? The constraint equations are:
// x(t=0) = x0 = a0
// y(t=0) = y0 = b0
// x(t=1) = x1 = a0 + a1 + a2
// y(t=1) = y1 = b0 + b1 + b2
// s(t=0) = s0 = dy/dx|t=0 -> s0 = (b1+2*b2*0)/(a1+2*a2*0) = b1/a1 -> s0*a1 = b1
// s(t=1) = s1 = dy/dx|t=1 -> s1 = (b1+2*b2*1)/(a1+2*a2*1) -> s1*(a1+2*a2) = b1+2*b2
// where dy/dx = (dy/dt)/(dx/dt). So, we have 6 equations for 6 unknowns, so this should work - but 
// what if dx/dt = 0? We could use r0 = 1/s0 = dx/dy in this case, but what if both dx/dt and dy/dt
// are both zero? ...maybe treat such special cases separately and set dy/dx = 1
// Maybe this quadratic interpolation could also be improved by normalizing the integral under
// x(t) and y(t) to be equal to the integral of a linear interpolant. This would give two 
// additional constraint equations calling for cubics. But these would be different cubics than 
// the Hermite cubics that we have now....perhaps better? less wiggle/overshoot? -> try it!
// Also, we could use as additonal constraints that the 2nd derivatives d2y/dx2|t=0, d2y/dx2|t=1 
// should be equal at the joints - 2 additional constraints -> two additional parameters (a3,b3)
// -> 8 coupled equations
// solving the system with sage:
/*
# variables and function definitions:
var("a0 a1 a2 b0 b1 b2 t x0 x1 y0 y1 s0 s1")
x(t)  = a0 + a1*t + a2*t^2
y(t)  = b0 + b1*t + b2*t^2
dx(t) = diff(x(t), t)         # dx/dt
dy(t) = diff(y(t), t)         # dy/dt

# constraint equations and solution:
e1 = x(0) == x0
e2 = y(0) == y0
e3 = x(1) == x1
e4 = y(1) == y1
e5 = dy(0)/dx(0) == s0
e6 = dy(1)/dx(1) == s1
solve([e1,e2,e3,e4,e5,e6], a0,a1,a2,b0,b1,b2)

gives:
a0 == x0 
a1 == 2*(s1*x0 - s1*x1 - y0 + y1)/(s0 - s1)
a2 == -((s0 + s1)*x0 - (s0 + s1)*x1 - 2*y0 + 2*y1)/(s0 - s1)
b0 == y0
b1 == 2*(s0*s1*x0 - s0*s1*x1 - s0*y0 + s0*y1)/(s0 - s1)
b2 == -(2*s0*s1*x0 - 2*s0*s1*x1 - (s0 + s1)*y0 + (s0 + s1)*y1)/(s0 - s1)

the case s0 == s1 needs to be treated separately to avoid div-by-zero - maybe use a line in this 
case, precompute: 1/(s0-s1), (s0+s1), 

// simplified (needs verification):
dx = x1-x0;
dy = y1-y0;
ss = s0+s1;     // slope sum
k  = 1/(s0-s1)
s1dx = s1*dx;
a0 = x0 
a1 = 2*(dy - s1dx)*k
a2 = (ss*dx - 2*dy)*k
b0 = y0
b1 = 2*(-s0*(s1dx + dy))*k
b2 = (2*s0*s1dx - ss*dy)*k


*/
#ifndef RS_INTERPOLATION_H
#define RS_INTERPOLATION_H

namespace RSLib
{

  // \todo prepend prefix "rs" to function names

  /** Computes coefficients for a cubic polynomial that goes through the 4 points (-1,y[-1]),
  (0,y[0]), (1,y[1]), (2,y[2]) that will have matched 1st derivatives when used on successive
  positions in an y-array. [TEST THIS! it has not yet been tested] */
  RSLib_API void rsCubicSplineCoeffsFourPoints(double *a, double *y);


  /** Fits a cubic polynomial of the form:
  \f[ f(x) = a3*x^3 + a2*x^2 + a1*x + a0  \f]
  to two points (x1,y1), (x2,y2) and matches values of the derivative (given by yd1, yd2) at these
  points.
  \todo change order of a coeffs - but make sure that all client code using this is updated
  accordingly. or maybe pass an array a[4] - this will force client code to be updated  */
  RSLib_API void fitCubicWithDerivative(double x1, double x2, double y1, double y2, double yd1,
    double yd2, double *a3, double *a2, double *a1, double *a0);

  /** Similar to fitCubicWithDerivative, but the x-coodinates of the two points are fixed at x0=0,
  x1=1 such that we fit the points (0,y0), (1,y1) and match values of the derivative (given by yd0,
  yd1) there. This simplifies the computation a lot compared to the general case. */
  RSLib_API void fitCubicWithDerivativeFixedX(double y0, double y1, double yd0, double yd1,
    double *a3, double *a2, double *a1, double *a0);

  /** Similar to fitCubicWithDerivativeFixedX, but fits a quintic (5th order) polynomial in order
  to additionaly match desired values for the 2nd derivatives (given by ydd0, ydd1) at the sample
  points. */
  RSLib_API void fitQuinticWithDerivativesFixedX(double y0, double y1, double yd0, double yd1,
    double ydd0, double ydd1, double *a5, double *a4,  double *a3, double *a2, double *a1,
    double *a0);

  /** Computes coefficients for a polynomial that passes through the points (x0 = 0, y0[0]),
  (x1 = 1, y1[0]) and in addition to passing through these points, it also matches a number "M" of
  derivatives to prescribed values. These values should be passed in y0[1], y0[2], etc. for the
  values at x0 = 0 and in y1[1], y1[2], etc. for the values at x1 = 1. The argument "a" is used to
  return the computed 2*M+2 polynomial coefficients for the polynomial of order 2*M+1.
  y0: (M+1)-element array containing y(0), y'(0), y''(0), y'''(0), etc.
  y1: (M+1)-element array containing y(1), y'(1), y''(1), y'''(1), etc.
  a:  (2*M+2)-element array for returning a0, a1, a2, a3, a4, a5, a6, a7, etc.  */
  RSLib_API void getHermiteCoeffsM(double *y0, double *y1, double *a, int M);

  /** Optimized version of getHermiteCoeffsM for the case M == 1. */
  RSLib_API void getHermiteCoeffs1(double *y0, double *y1, double *a);

  /** Optimized version of getHermiteCoeffsM for the case M == 2. */
  RSLib_API void getHermiteCoeffs2(double *y0, double *y1, double *a);

  /** Optimized version of getHermiteCoeffsM for the case M == 3. */
  RSLib_API void getHermiteCoeffs3(double *y0, double *y1, double *a);

  /** Computes a delayed sample with a fractional delay of "d" (0 <= d <= 1) behind y[0]. To
  compute the output value, the function uses y[0], y[-1], y[-2], ..., y[-(M+1)] to obtain finite
  difference approximations for a number "M" of derivatives and uses these as desired derivatives
  for a Hermite interpolation. The resulting interpolating function (seen in the continuous time
  domain) will pass through all the sample-points and has M continuous derivatives. The continuous
  time impulse response of the interpolator is asymmetric due to the way in which the derivatives
  are approximated (using only past values). The "shape" parameter controls, which value will
  actually be used for  the desired derivative by multiplying the raw finite difference
  approximation for the n-th derivative by shape^n.  */
  RSLib_API double getDelayedSampleAsymmetricHermiteM(double d, double *y, int M,
    double shape = 1.0);

  /** Optimized version of getDelayedSampleAsymmetricHermiteM for the case M == 1. */
  RSLib_API double getDelayedSampleAsymmetricHermite1(double d, double *y, double shape = 1.0);

  /** Computes a delayed sample with a fractional delay of "d" (0 <= d <= 1) behind y[0] by
  linearly interpolating between y[0] and y[-1]. */
  RSLib_API double getDelayedSampleLinear(double d, double *y);

  // \todo make symmetric versions for Hermite interpolators (using symmetric finite differences),
  // make similar functions for Lagrange interpolation and sinc-interpolation


  /** Fits a cubic polynomial of the form:
  \f[ f(x) = a*x^3 + b*x^2 + c*x + d  \f]
  to four points (x0,y0), (x1,y1),(x2,y2), (x3,y3)  */
  RSLib_API void fitCubicThroughFourPoints(double x0, double y0, double x1, double y1, double x2,
    double y2, double x3, double y3, double *a, double *b, double *c, double *d);


  /** Given two x-axis values x1, x2 and the corresponding y-axis values y1, y2, this function
  returns the y-value corresponding to x by linearly interpolating between x1, x2. If x is outside
  the range of x1, x2, the function will extrapolate. */
  template<class T>
  T rsInterpolateLinear(T x1, T x2, T y1, T y2, T x);

  /** Given two length N arrays x, y with x-axis values and corresponding y-axis values, this
  function fills the array yi with values corresponding to the xi by linear interpolation
  (or extrapolation, if necessary) of the input data. The xi and yi arrays are of length Ni. */
  template<class T>
  void rsInterpolateLinear(T *x, T *y, int N, T *xi, T *yi, int Ni);

  /** Given four x-axis values x1, x2, x3, x4 and the corresponding y-axis values y1, y2, y3, y4,
  this function returns the y-value corresponding to x by cubic hermite interpolation. It is
  assumed that the value of x is between x2 and x3 and the interpolation will be such that
  successive interpolants on the same data set will have matching derivatives at the data points
  (in addition to pass through the points). It can be used for interpolating data sets for which
  the x-axis values are not equidistant. */
  template<class T>
  T rsInterpolateCubicHermite(T x1, T x2, T x3, T x4, T y1, T y2, T y3, T y4, T x);



  /** Given two length N arrays x, y with x-axis values and corresponding y-axis values, and a
  2D array of size MxN containing M derivative values at the sample points y', y'', this function
  fills the array yi with values corresponding to the xi by spline interpolation (or extrapolation,
  if necessary) of the input data. The xi and yi arrays are of length Ni. The number M gives the
  number of derivatives that should match at the data points where two subsequent splines join and
  the actual values of the m-th derivative at a data-point with index n is given by yd[m-1][n].

  \todo have also an optional parameter **ydi where interpolated values of the derivative will be
  stored - we can just use evaluatePolynomialAndDerivativesAt instead of evaluatePolynomialAt and
  do some copying
  */
  template<class T>
  void rsInterpolateSpline(T *x, T *y, T **yd, int N, int M, T *xi, T *yi, int Ni);


  /** Given two length N arrays x, y with x-axis values and corresponding y-axis values, this
  function fills the array yi with values corresponding to the xi by spline interpolation
  (or extrapolation, if necessary) of the input data. The xi and yi arrays are of length Ni. The
  smoothness parameter gives the number of derivatives that should match at the data points where
  two subsequent splines join. The higher this value, the smoother the resulting spline function
  will be in terms of continuous derivatives, but it will also tend to oscillate more between the
  datapoints for higher smoothness values. With a value of 1, which is the default, a cubic spline
  will be used and the 1st derivative will match at the data points. Generally, a polynomial of
  order 2*smoothness+1 will be used. */
  template<class T>
  void rsInterpolateSpline(T *x, T *y, int N, T *xi, T *yi, int Ni, int smoothness = 1);


  //===============================================================================================
  // implementation of template-functions:


  template<class T>
  T rsInterpolateLinear(T x1, T x2, T y1, T y2, T x)
  {
    if( x1 == x2 )
      return 0.5 * (y1+y2); // at a discontinuity, we return the average
    T a = (y2-y1) / (x2-x1);
    T b = y1 - a*x1;
    return a*x + b;
      // factor out computation of a and b
  }

  template<class T>
  void rsInterpolateLinear(T *x, T *y, int N, T *xi, T *yi, int Ni)
  {
    int n = 0;  // index into input data
    int i = 0;  // index into interpolated data
    T a, b;     // parameters of the line y = a*x + b

    while( n < N-1 )
    {
      a = (y[n+1]-y[n]) / (x[n+1]-x[n]);
      b = y[n] - a*x[n];
      while( xi[i] < x[n+1] && i < Ni )
      {
        yi[i] = a*xi[i] + b;
        i++;
      }
      n++;
    }

    while( i < Ni )
    {
      yi[i] = a*xi[i] + b;
      i++;
    }
  }

  template<class T>
  T rsInterpolateCubicHermite(T x1, T x2, T x3, T x4, T y1, T y2, T y3, T y4, T x)
  {
    T s, d1, d3, s2, s3, a[4];
    s  = T(1)/(x3-x2);
    d1 = s*(x2-x1);
    d3 = s*(x4-x2)-T(1);
    s2 = ((y2-y1)/d1 + (y3-y2)*d1) / (d1+T(1));
    s3 = ((y3-y2)*d3 + (y4-y3)/d3) / (d3+T(1));
    fitCubicWithDerivativeFixedX(y2, y3, s2, s3, &a[3], &a[2], &a[1], &a[0]);
    return evaluatePolynomialAt(s*(x-x2), a, 3);
    // maybe factor out a function that returns the polynomial coefficients (and s) because the
    // same set of coefficients my get used to interpolate at multiple values of x between the
    // same x2 and x3 and it is wasteful to recompute the coefficients each time
  }

  template<class T>
  void rsInterpolateSpline(T *x, T *y, T **yd, int N, int M, T *xi, T *yi, int Ni)
  {
    int n = 0;            // index into input data
    int i = 0;            // index into interpolated data
    int m;                // index of the derivative
    T scale, shift;       // scaler and shifter for the input value for the polynomial
    T *a  = new T[2*M+2]; // polynomial coefficients
    T *y0 = new T[M+1];   // y0 values and derivatives passed to getHermiteCoeffsM
    T *y1 = new T[M+1];   // y1 values and derivatives passed to getHermiteCoeffsM

    while( n < N-1 )
    {
      // compute coeffs for spline and shift- and scale values:
      shift = x[n];
      scale = x[n+1]-x[n];
      y0[0] = y[n];
      y1[0] = y[n+1];
      for(m = 1; m <= M; m++)
      {
        y0[m] = yd[m-1][n]   * scale;
        y1[m] = yd[m-1][n+1] * scale;
      }
      getHermiteCoeffsM(y0, y1, a, M);

      // extra-/interpolate:
      scale = 1.0 / scale;
      while( xi[i] < x[n+1] && i < Ni )
      {
        yi[i] = evaluatePolynomialAt(scale*(xi[i]-shift), a, 2*M+1);
        i++;
      }

      n++;
    }

    // extrapolate tail:
    while( i < Ni )
    {
      yi[i] = evaluatePolynomialAt(scale*(xi[i]-shift), a, 2*M+1);
      i++;
    }

    // cleanup:
    delete[] a;
    delete[] y0;
    delete[] y1;
  }

  template<class T>
  void rsInterpolateSpline(T *x, T *y, int N, T *xi, T *yi, int Ni, int M)
  {
    // compute numeric derivatives of y, to be used for the spline at data points:
    T **yd = nullptr;
    if( M > 0 )
    {
      rsAllocateMatrix(yd, M, N);
      rsNumericDerivative(x, y, yd[0], N);
      for(int m = 1; m < M; m++)
        rsNumericDerivative(x, yd[m-1], yd[m], N);
    }

    // interpolate with these numeric derivatives and cleanup:
    rsInterpolateSpline(x, y, yd, N, M, xi, yi, Ni);
    if( M > 0 )
      rsDeAllocateMatrix(yd, M, N);
  }


}

#endif

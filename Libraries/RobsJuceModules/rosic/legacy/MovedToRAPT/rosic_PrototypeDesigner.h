#ifndef rosic_PrototypeDesigner_h
#define rosic_PrototypeDesigner_h

namespace rosic
{

/** A class to represent a rational function in terms of its zeros, poles and a global scale 
factor. */
/*
class ZerosPolesGain
{

public:

  std::vector<Complex> z; // the zeros
  std::vector<Complex> p; // the poles
  double k;               // the scale factor

};
*/

/** This class determines the locations of poles and zeros in the s-plane for a continuous time, 
unit cutoff lowpass or low-shelving prototype filter. It supports Butterworth, Chebychev, inverse 
Chebychev, elliptic, Bessel and Papoulis designs. The low-shelving design is a generalization of a 
unit cutoff lowpass filter and and its magnitude response can be seen as the lowpass-response 
raised to a pedestal. The height of this pedestal is called the reference gain (which is zero in 
the lowpass-case). The gain the 'passband' (represented by the member variable G) is either a 
boost (when G > 1) or an attenuation/cut (when G < 1) - for standard lowpass designs, this gain 
would be unity.

References:
 -(1) Sophocles J. Orfanidis: Lecture Notes on Elliptic Filter Design
 -(2) Sophocles J. Orfanidis: High-Order Elliptical Equalizer Design
 -(3) Larry D.Paarmann: Design and Analysis of Analog Filters

\todo
 -check the gain calculation
 -solve the degree quation for other values than the filter order
 -check getFilterResponseAt() - seems to be buggy - obsolete?
 -get rid of needsSpecialHighShelvTransform() - rewrite the prototype design code in such a way 
  that all approximations can be treated uniformly later on
 -lot's of stinky code duplication -> eliminate it */

class rsPrototypeDesigner
{

public:

  /** This is an enumeration of the available approximation methods. */
  enum approximationMethods
  {
    BUTTERWORTH = 0,   ///< maximally flat at DC
    CHEBYCHEV,         ///< equiripple in passband, monotonic in stopband
    INVERSE_CHEBYCHEV, ///< equiripple in stopband, monotonic in passband
    ELLIPTIC,          ///< equiripple in passband and stopband, maximally steep transition
    BESSEL,            ///< approximates linear phase
    PAPOULIS           ///< maximizes steepness at cutoff (selectivity) under constraint of monotonicity
    // GAUSS           ///< smallest timelength*bandwidth product, good time response (no overshoot?)
    // HALPERN         ///< minimizes ratio of bandwidths at specified magnitudes (shaping factor) under constraint of monotonicity
                       ///< ...less steep at cutoff but steeper in stopband than Papoulis
  };
  // re-order: COINCINDENT_POLE, GAUSS, BESSEL, BUTTERWORTH, PAPOULIS <-?-> HALPERN, CHEBY1 <-?-> 
  // CHEBY2, ELLIPTIC ->sorted by desirability of time response vs. frequency response (roughly)

  // ideas: try other polynomials, for example non-reversed Bessel, Laguerre, etc. - if roots occur 
  // in the right half-plane, reflect them, maybe try Power-Series expansions of various 
  // interesting functions as it is done with the Gaussian filter

  /** This enumerates the two possible prototype filter characterisitics. */
  enum prototypeModes
  {
    LOWPASS_PROTOTYPE = 1,
    LOWSHELV_PROTOTYPE
  };

  //-----------------------------------------------------------------------------------------------
  /** \name Construction/Destruction */

  /** Constructor. */
  rsPrototypeDesigner();

  /** Destructor. */
  ~rsPrototypeDesigner();

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Sets ups the order of the filter - that is the number of first order sections. Each section 
  increases the slope by 6 dB/oct (for lowpass-designs). If the order is odd, a real pole-/zero 
  pair will be present, otherwise all poles and zeros will be complex
  conjugate. */
  void setOrder(int newOrder);

  /** Chooses one of the approximation methods as enumerated above. */
  void setApproximationMethod(int newApproximationMethod);

  /** Chooses a lowpass or low-shelving prototype as enumerated above. */
  void setPrototypeMode(int newPrototypeMode);

  /** Sets the ripple in the passband for lowpass designs in decibels. */
  void setPassbandRipple(double newPassbandRipple);

  /** Sets the rejection in the stopband for lowpass designs in decibels. */
  void setStopbandRejection(double newStopbandRejection);

  /** Sets the gain for shelving filters in the passband (in dB). This will be positive for a 
  low-boost and negative for low-cut responses. */
  void setGain(double newGain);

  /** Sets up the reference gain for shelving filters (in dB). This will be usually unity. */
  void setReferenceGain(double newReferenceGain);

  /** Selects the fraction of the maximum gain, which the magnitude response assumes at the 
  passband-frequency. This parameter is relevant only for Chebychev and elliptic shelving filters.
  It should be chosen close to unity (for example 0.95) in order to prevent excessive ripple in the
  passband. */
  void setPassbandGainRatio(double newPassbandGainRatio);

  /** Selects the fraction of the maximum gain, which the magnitude response assumes at the 
  stopband-frequency. This parameter is only relevant for inverse Chebychev and elliptic shelving 
  filters. It should be chosen close to zero (for example 0.05) in order to prevent excessive 
  ripple in the stopband. */
  void setStopbandGainRatio(double newBandwidthGainRatio);

  /** Assigns the poles and zeros such that the resulting filter will just pass the signal 
  through. */
  void makeBypass();

  //-----------------------------------------------------------------------------------------------
  /** \name Static Functions */

  /** Given desired the order "N" of the prototype filter, this function returns the number of 
  required 2nd order sections in "L" and 1st order sections in "r" (either 0 or 1). */
  static void getNumBiquadsAndFirstOrderStages(int N, int &L, int &r);

  /** Solves the elliptic degree equation (k from N, k1). */
  static double ellipdeg(int N, double k_1);

  /** Solves the elliptic degree equation (k_1 from N, k1). */
  static double ellipdeg1(int N, double k);

  /** Solves the elliptic degree equation (k_1 from N, k) using nomes. */
  static double ellipdeg2(double N, double k);

  /** Calculates the order required for a Butterworth filter to fullfill given design 
  specifications. The actual order should be chosen to be the next integer. */
  static double getRequiredButterworthOrder(double passbandFrequency, double passbandRipple, 
    double stopbandFrequency, double stopbandRipple);

  /** Calculates the order required for a Chebychev filter (inverse or not) to fullfill given 
  design specifications. The actual order should be chosen to be the next integer. */
  static double getRequiredChebychevOrder(double passbandFrequency, double passbandRipple, 
    double stopbandFrequency, double stopbandRipple);

  /** Calculates the order required for an elliptic filter to fullfill given design 
  specifications. The actual order should be chosen to be the next integer. */
  static double getRequiredEllipticOrder(double passbandFrequency, double passbandRipple, 
    double stopbandFrequency, double stopbandRipple);

  /** Given the arrays of polynomial coefficients "b" and "a" of a transfer function 
  H(s) = N(s)/D(s), this function returns the polynomial coefficients of the corresponding 
  magnitude-squared function H(s)*H(-s) = (N(s)*N(-s)) / (D(s)*D(-s)). "N" is the order of the 
  filter, so "b", "a" should be of length N+1 and "b2", "a2" of length 2*N+1. */
  static void magSquaredNumAndDen(double *b, double *a, double *b2, double *a2, int N);

  /** Given the two arrays "b2" and "a2" of polynomial coefficients (both of length 2*N+1) for 
  numerator and denominator of an s-domain Nth order lowpass prototype magnitude-squared function 
  and gain a constant k, this function computes the numerator coefficients of the magnitude-squared 
  function of the corresponding low-shelving filter with reference-gain G0 and low-frequency gain G 
  and writes them into bS. */
  static void shelvingMagSqrNumFromLowpassMagSqr(double *b2, double *a2, double k, int N, 
    double G0, double G, double *bShelf);

  /** Given the two arrays "b" and "a" of polynomial coefficients (both of length N+1) for 
  numerator and denominator of an s-domain lowpass prototype transfer function and gain a 
  constant k, this function computes the numerator coefficients of the magnitude-squared function 
  of the corresponding low-shelving filter with reference-gain G0 and low-frequency gain G and 
  writes them into bS. Because the magnitude-squared function has twice the order of the transfer 
  function itself, bS will be of length 2*N+1. The left halfplane roots of bS will be the zeros of 
  the shelving filter. The denominator coefficients (and hence, the poles of the filter) are 
  unaffected by the lowpass-to-lowshelf transform, so you may re-use your "a" array in the 
  low-shelving filter. */
  static void shelvingMagSqrNumeratorFromLowpassTransfer(double *b, double *a, double k, int N, 
    double G0, double G, double *bS);

  /** Scales zeros, poles and gain factor, such that the magnitude response at unit frequency 
  equals "g". */
  static void scaleToMatchGainAtUnity(Complex* z, Complex* p, double* k, Complex* zNew, 
    Complex* pNew, double* kNew, int N, double g);

  /** Returns zeros, poles and gain in "zNew", "pNew", "kNew" of a filter that is inverse to the 
  filter with zeros, poles and gain of "z", "p", "k". \todo maybe move to PoleZeroMapper. zNew, 
  pNew may point to the same arrays as z, p. */
  static void getInverseFilter(Complex *z, Complex *p, double *k, Complex *zNew, Complex *pNew, 
    double *kNew, int N);

  /** Given an array of "N"+1 cofficients for a polynomial of order "N", this function returns the 
  left halfplane roots in "r" and returns the number of such roots in the return-value. The rest of
  the array "r" is left as is - in most cases, you should assume that it contains garbage.
  \todo maybe move to PolynomialAlgorithms */
  static int getLeftHalfPlaneRoots(double *a, Complex *r, int N);

  /** Computes zeros, poles and gain factor for an analog lowpass Bessel prototype filter of order 
  "N" and stores them in "z", "p" and "k", respectively.
  // \todo: include parameter for the normalization mode (asymptotic, delay-normalized, 
  cutoff-normalized) */
  static void getBesselLowpassZerosPolesAndGain(Complex *z, Complex *p, double *k, int N);

  /** Computes zeros, poles and gain factor for an analog low-shelving Bessel prototype filter.
  @see getBesselLowpassZerosPolesAndGain */
  static void getBesselLowShelfZerosPolesAndGain(Complex *z, Complex *p, double *k, int N, 
    double G, double G0);

  // Papoulis design:

  /** Constructs the denominator polynomial of the magnitude-squared function for Papoulis filters 
  where "N" is the filter order and "a2" is of length 2*N+1. */
  static void papoulisMagnitudeSquaredDenominator(double *a2, int N);
  static void getPapoulisLowpassZerosPolesAndGain(Complex *z, Complex *p, double *k, int N);
  static void getPapoulisLowShelfZerosPolesAndGain(Complex *z, Complex *p, double *k, int N, 
    double G, double G0);
  // maybe make this the only public method for Papoulis Design

  /** Computes zeros, poles and gain factor for an analog elliptic prototype filter of order "N" 
  with passband gain variation (ripple) "Gp" and maximum stopband amplitude "Gs", and stores them 
  in "z", "p" and "k", respectively. */
  static void getEllipticLowpassZerosPolesAndGain(Complex *z, Complex *p, double *k, int N, 
    double Gp, double Gs);

  //static void getLowpassZerosPolesAndGain(Complex *z, Complex *p, double *k, int N, 
  //  int approximationMethod);

  // use a structure FilterZerosPolesGain instead of the 2 arrays and k

  //static void getBesselLowshelfZerosPolesAndGain(Complex *z, Complex *p, double *k, int N, 
  //  double G, double G0); 

  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  /** Re-calculates the poles and zeros (if necesarry) an writes them into the respective arrays. 
  For complex conjugate pairs, it will only write one representant of the pair into the array. If 
  N denotes the order as passed to setOrder(), let L be the number of second order sections and r 
  be either one or zero indicating presence or absence of a first order section. Then, the array 
  will be filled with one representant for each pole pair into poles[0...L-1] and the real pole 
  will be written into poles[L+r-1], if present. The same goes for the zeros. */
  void getPolesAndZeros(Complex* poles, Complex* zeros);
    // \todo: rename into getNonRedundantPolesAndZeros - provide also a function getPolesAndZeros 
    // that writes all poles and zeros "as is" into the arrays.

  /** Calculates and returns the complex frequency response at some value of the Laplace transform
  variable s - seems to be buggy. */
  Complex getFilterResponseAt(Complex s);

  /** Returns the normalized (divided by the DC gain) magnitude at the given radian frequency w. */
  double getMagnitudeAt(double w);

  /** Finds the radian frequency w inside the interval wLow...wHigh at which the specified 
  magnitude (as raw amplitude value, not in dB) occurs. For the function to work properly, the 
  magnitude response inside the given interval should be monotonic and the specified magnitude 
  should occur somewhere inside the given interval. */
  double findFrequencyWithMagnitude(double magnitude, double wLow, double wHigh);

  /** Returns the number of finite poles. */
  int getNumFinitePoles();

  /** Returns the number of finite zeros. */
  int getNumFiniteZeros();

  /** Returns the number of non-redundant finite zeros (each pair of complex conjugate zeros counts
  only once). */
  int getNumNonRedundantFiniteZeros();

  /** Returns the number of non-redundant finite poles (each pair of complex conjugate poles counts 
  only once). */
  int getNumNonRedundantFinitePoles();

  /** Returns the passbandRipple in dB. */
  double getPassbandRipple() const { return Ap; }

  /** Returns the ratio between the peak gain (in dB) and the ripples inside the boosted/cutted 
  band for shelving modes. */
  double getPassbandGainRatio() const { return Rp; }

  /** Returns the approximation method to be used @see enum approximationMethods. */
  int getApproximationMethod() const { return approximationMethod; }

  /** Returns the mode of the prototype @see enum modes. */
  int getPrototypeMode() const { return prototypeMode; }

  /** Returns the filter order. */
  int getOrder() const { return N; }

  /** Returns true if the currently selected approximation method supports a ripple parameter. */
  bool hasCurrentMethodRippleParameter();
  //{ return (approximationMethod == ELLIPTIC) || (approximationMethod == CHEBYCHEV); }

  /** Returns true if the currently selected approximation method supports a rejection 
  parameter. */
  bool hasCurrentMethodRejectionParameter();
  //{ return (approximationMethod == ELLIPTIC) || (approximationMethod == INVERSE_CHEBYCHEV); }

  /** Returns true when the currently selected approximation method requires a special 
  lowshhelf-to-highshelf s-plane frequency transformation. This is a rather ugly kludge - we should 
  rewrite the prototype design code such that all approximation methods can be treated 
  uniformly. */
  bool needsSpecialHighShelvTransform();

  //===============================================================================================

protected:

  /** Calculates the poles and zeros according to the selected approximation method and the global 
  gain factor. */
  void updatePolesAndZeros();

  /** Calculates the poles and zeros for a Butterworth lowpass filter. */
  void makeButterworthLowpass();

  /** Calculates the poles and zeros for a Butterworth low-shelving filter. */
  void makeButterworthLowShelv();

  /** Calculates the poles and zeros for a Chebychev lowpass filter. */
  void makeChebychevLowpass();

  /** Calculates the poles and zeros for a Chebychev low-shelving filter. */
  void makeChebychevLowShelv();

  /** Calculates the poles and zeros for an inverse Chebychev lowpass filter. */
  void makeInverseChebychevLowpass();

  /** Calculates the poles and zeros for an inverse Chebychev low-shelving filter. */
  void makeInverseChebychevLowShelv();

  /** Calculates the poles and zeros for an elliptic lowpass filter. */
  void makeEllipticLowpass();

  /** Calculates the poles and zeros for an elliptic low-shelving filter. */
  void makeEllipticLowShelv();

  /** Assigns the postions of the poles of a Bessel prototype filter. */
  void makeBesselLowShelv(double G = 1.0, double G0 = 0.0);

  /** Assigns the postions of the poles of a Papoulis prototype filter. */
  void makePapoulisLowShelv(double G = 1.0, double G0 = 0.0);

  /** Given the arrays zTmp and pTmp of poles and zeros, this function picks the non-redundant ones
  and copies them into our z and p members, respectively. The arrays zTmp, pTmp are assumed to 
  contain N zeros and poles, where N is the order of the prototype filter. Complex poles/zeros 
  should occur in complex conjugate pairs - this function will select one representant for each 
  such pair.
  ATTENTION: If there is a real zero/pole present, they are assumed to be in zTmp[0] and pTmp[0] 
  respectively - the caller must ensure this.  */
  void pickNonRedundantPolesAndZeros(Complex *zTmp, Complex *pTmp);

  // user parameters:
  int N;                   // prototype filter order: N = 2*L + r 
  int approximationMethod; // selected approximation method 
  int prototypeMode;       // selected mode (lowpass or low-shelv)
  double Ap;               // passband ripple/attenuation for lowpass designs in dB
  double As;               // stopband rejection for lowpass designs in dB
  double A;                // boost/cut gain for low-shelv designs in dB
  double A0;               // reference gain for low-shelv designs in dB
  double Rp;               // ripple in the boosted/cutted band as fraction of dB-peak-gain for 
                           // shelvers (elliptic and chebychev)
  double Rs;               // ripple outside the boosted/cutted band as fraction of dB-peak-gain 
                           // for shelvers (elliptic and inverse chebychev)

  // internal variables:
  int L;                   // number of second order sections
  int r;                   // number of first order sections (either zero or one)
  int numFinitePoles;      // number of poles (excluding those at infinity)
  int numFiniteZeros;      // number of zeros (excluding those at infinity). 


  // arrays for nonredundant poles and zeros:
  static const int maxNumNonRedundantPoles = 13;
  Complex z[maxNumNonRedundantPoles];  // zeros
  Complex p[maxNumNonRedundantPoles];  // poles

  bool stateIsDirty;   // this flag indicates, whether the poles, zeros and gain need to be 
                       // re-calculated or are still valid from a previous calculation
};

}

#endif

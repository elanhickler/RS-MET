#include "TestUtilities.h"


bool runUnitTest(bool (*test)(), const std::string& name)
{
  //cout << "Testing: " + name + ": ";
  std::cout << name + ": ";
  bool passed = test();
  //rsAssert(passed); // break, if test fails
  if(passed)
    std::cout << "Passed\n";
  else
    std::cout << "!!!!----> F A I L E D <----!!!!\n";
  return passed;
}

/*
bool detectMemoryLeaks()
{
  #ifdef _MSC_VER
  return _CrtDumpMemoryLeaks() == 1;
  #else
  return false;
  #endif
}
*/

std::vector<double> rsLinearRangeVector(int N, double min, double max)
{
  std::vector<double> v(N);
  RAPT::rsArray::fillWithRangeLinear(&v[0], N, min, max);
  return v;
}

std::vector<double> rsExponentialRangeVector(int N, double min, double max)
{
  std::vector<double> v(N);
  RAPT::rsArray::fillWithRangeExponential(&v[0], N, min, max);
  return v;
}

std::vector<double> rsRandomVector(int N, double min, double max, int seed)
{
  std::vector<double> v(N);
  RAPT::rsArray::fillWithRandomValues(&v[0], N, min, max, seed);
  return v;
}

std::vector<double> rsApplyFunction(const std::vector<double>& v, double p, 
  double (*f) (double, double))
{
  std::vector<double> r(v.size());
  for(int i = 0; i < r.size(); i++)
    r[i] = f(v[i], p);
  return r;
}


#if defined(_MSC_VER)
// works only on MSVC and we need this function only in the performance tests, which we do only
// with the MSVC compiler anyway:
std::string toString(int n)
{
  //return std::to_string((_Longlong)n);
  return std::to_string((long long)n);
}
#endif

/*
double rsSquare(double x)
{
  return x*x;
}
*/

using namespace std;

#undef min

bool areNumbersEqual(double x, double y, double relativeTolerance)
{
  // nan == nan:
  double tmp = RS_NAN(double);
  if( memcmp(&x, &tmp, sizeof(double)) == 0 && memcmp(&y, &tmp, sizeof(double)) == 0 )
    return true;

  // inf == inf:
  tmp = RS_INF(double);
  if( x == tmp && y == tmp )
    return true;

  // -inf == -inf:
  tmp = -tmp;
  if( x == tmp && y == tmp )
    return true;

  // catch case where one or bothe of x, y are denormals - in this case, the 
  // relativeTolerance*rsMax(fabs(x), fabs(y)) yields a zero absolute tolerance whereas the 
  // difference on the left hand side is a very small (denormal) nonzero number
  //double denormThresh = RS_MIN(double);
  double denormThresh = std::numeric_limits<double>::min();
  if( fabs(x) <= denormThresh && fabs(y) <= denormThresh )
    return true;

  // x == y, if the absolute difference is below a relative tolerance:
  return fabs(x-y) <= relativeTolerance * max(fabs(x), fabs(y));
}

RAPT::rsWindowFunction::WindowType stringToWindowType(const std::string& wt)
{
  typedef RAPT::rsWindowFunction::WindowType WT;
  if(wt == "rc") return WT::rectangular;
  if(wt == "hn") return WT::hanningZZ;
  if(wt == "hm") return WT::hamming;
  if(wt == "bm") return WT::blackman;
  if(wt == "bh") return WT::blackmanHarris;
  RAPT::rsError("Unknown window type");
  return WT::rectangular;
}
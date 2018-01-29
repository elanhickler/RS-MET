#ifndef rosic_DualFilter_h
#define rosic_DualFilter_h

namespace rosic
{

class rsDualFilter
{

public:

  /** Determines whether the output of the 1st filter or the output of the 2nd filter (or both) 
  has a unit delay in the feedback path. */
  enum delayConfigs
  {
    DELAY_FIRST = 0,
    DELAY_SECOND,
    DELAY_BOTH
  };



  INLINE rsFloat64x2 getSample(rsFloat64x2 x)
  {
    if(delayConfig == DELAY_BOTH)
    {
      rsFloat64x2 y1Old = y1;
      y1 = filter1->getSample(a*x + d*y2);
      y2 = filter2->getSample(b*x + c*y1Old);
    }
    else if(delayConfig == DELAY_SECOND)
    {
      y1 = filter1->getSample(a*x + d*y2);
      y2 = filter2->getSample(b*x + c*y1);
    }
    else  // delayConfig == DELAY_FIRST
    {
      y2 = filter2->getSample(b*x + c*y1);
      y1 = filter1->getSample(a*x + d*y2);
    }
    return e*y1 + f*y2;
  }

  /*
  INLINE void getSampleFrameStereo(double* left, double* right)
  {
    rsFloat64x2 tmp(*left, *right);
    *left  = tmp[0];
    *right = tmp[1];
  }
  */

protected:

  int delayConfig = DELAY_BOTH;
  rsPolyModule *filter1 = nullptr, *filter2 = nullptr;
  rsFloat64x2 y1, y2;
  double a, b, c, d, e, f;

};


}

#endif
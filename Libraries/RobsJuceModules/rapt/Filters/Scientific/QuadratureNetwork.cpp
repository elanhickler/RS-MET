template<class TSig, class TPar>
rsQuadratureNetwork<TSig, TPar>::rsQuadratureNetwork() 
{
  order = 12;
  gain  = 1.0;
  designer.setApproximationMethod(rsPrototypeDesigner<TPar>::ELLIPTIC);  
  designer.setMode(rsPrototypeDesigner<TPar>::LOWPASS_PROTOTYPE);
  designer.setPrototypeOrder(order);
  designer.setSampleRate(TPar(44100));
  designer.setFrequency(TPar(11025));
  designer.setRipple(TPar(0.1));
  designer.setStopbandRejection(40.0);

  updateCoefficients();
  reset();
}

// setup:

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::setApproximationMethod(int newApproximationMethod)
{
  designer.setApproximationMethod(newApproximationMethod);
  updateCoefficients();
}

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::setOrder(int newOrder)
{
  if( newOrder <= maxOrder )
  {
    order = newOrder;
    designer.setPrototypeOrder(newOrder);
    updateCoefficients();
  }
}

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::setRipple(TPar newRipple)
{
  designer.setRipple(newRipple);
  updateCoefficients();
}

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::setStopbandRejection(TPar newStopbandRejection)
{
  designer.setStopbandRejection(newStopbandRejection);
  updateCoefficients();
}

// others:

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::reset()
{
  for(int i = 0; i < maxOrder; i++)
  {
    x[i] = 0.0;
    y[i] = 0.0;
  }
}

template<class TSig, class TPar>
void rsQuadratureNetwork<TSig, TPar>::updateCoefficients()
{
  // design the prototype halfband lowpass filter:
  designer.getPolesAndZeros(poles, zeros);

  // evaluate lowpass transfer function at z=1 and obtain the gain factor:
  TPar desiredGain = TPar(1);
  if( rsIsEven(order) )
    desiredGain = - rsDbToAmp(designer.getPassbandRipple());
  ComplexPar num = TPar(1);
  ComplexPar den = TPar(1);
  for(int i = 0; i < order; i++)
  {
    num *= (TPar(1)-zeros[i]);
    den *= (TPar(1)-poles[i]);
  }
  TPar actualGain = abs(num/den);
  gain = TPar(2)*desiredGain/actualGain; // factor 2 due to cos(x) = (exp(j*x)+exp(-j*x)) / 2

  // rotate pole/zero pattern by 90 degrees counterclockwise:
  ComplexPar j(0.0, 1.0); 
  for(int i = 0; i < order; i++)
  {
    poles[i] *= j;
    zeros[i] *= j;
  }
}

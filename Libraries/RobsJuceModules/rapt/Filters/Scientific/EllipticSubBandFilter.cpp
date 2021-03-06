template<class TSig, class TPar>
rsEllipticSubBandFilter<TSig, TPar>::rsEllipticSubBandFilter()
{
  this->initBiquadCoeffs(); // initializes the filter-coefficients
  this->reset();            // resets the filters memory buffers (to 0)
  this->numStages = 6;      // this is a 12th order filter with 6 biquad-sections

  // design the analog prototype filter with unit cutoff frequency:
  rsPrototypeDesigner<TPar> designer;
  designer.setApproximationMethod(rsPrototypeDesigner<TPar>::ELLIPTIC);
  designer.setOrder(2 * this->numStages);
  designer.setPassbandRipple(TPar(0.1));
  designer.setStopbandRejection(96.0);
  designer.getPolesAndZeros(prototypePoles, prototypeZeros);

  // now we have the poles and zeros of the prototype filter stored in our member variables, we
  // now make a digital halfband filter from them:
  setSubDivision(TPar(2));
}

template<class TSig, class TPar>
void rsEllipticSubBandFilter<TSig, TPar>::setSubDivision(TPar newSubDivision)
{
  if( newSubDivision > TPar(1) )
    subDivision = newSubDivision;
  else
  {
    // use a neutral filter when the subdivision is <= 1.0:
    this->initBiquadCoeffs();
    return;
    //DEBUG_BREAK;
  }

  // create temporary arrays for poles and zeros and copy the prototype poles and zeros into
  // these:
  Complex poles[6];
  Complex zeros[6];
  for(int i = 0; i < 6; i++)
  {
    poles[i] = prototypePoles[i];
    zeros[i] = prototypeZeros[i];
  }

  // calculate the desired cutoff frequency of the denormalized analog filter
  TPar wd = TPar(0.9*PI)/subDivision;
   // normalized digital radian frequency - this factor of 0.9 is kinda arbitrary, ideally we
   // would want to set a factor such that the stopband begins exactly at PI/subDivision (whereas
   // here we are dealing with the end of the passband)

  //wd = PI/subDivision;    // for FreqShifter test

  TPar sampleRate = TPar(44100);
  // actually, this should be irrelevant but we need it as dummy. \todo make some tests if this is
  // really irrelevant

  TPar wa = TPar(2)*sampleRate*tan(TPar(0.5)*wd);  // pre-warped analog radian frequency

  // obtain the denormalized s-domain poles and zeros by a lowpass->lowpass transform (this is done
  // in place on the temporary arrays):

  TPar gainDummy;
  rsPoleZeroMapper<TPar>::prototypeToAnalogLowpass(poles, 6, zeros, 6, &gainDummy, wa);

  // transform poles and zeros from s-domain to z-domain via bilinear transform:
  rsPoleZeroMapper<TPar>::bilinearAnalogToDigital(poles, 6, zeros, 6, sampleRate, &gainDummy);

  // convert the pole-zero representation to biquad cascade cofficients:
  rsFilterCoefficientConverter<TPar>::polesAndZerosToBiquadCascade(poles, 6, zeros, 6, this->b0,
    this->b1, this->b2, this->a1, this->a2, false);

  // normalize DC-gain to unity for each stage:
  rsFilterCoefficientConverter<TPar>::normalizeBiquadStages(this->b0, this->b1, this->b2, this->a1,
    this->a2, 0.0, 6);
}

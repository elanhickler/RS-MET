#ifndef RAPT_IMAGEPROCESSOR_H_INCLUDED
#define RAPT_IMAGEPROCESSOR_H_INCLUDED

/** A collection of (basic) image processing algorithms. */

template<class T>
class rsImageProcessor
{

public:



  static void normalize(rsImage<T>& img);


  static void normalizeFast(rsImage<T>& img);



  /** Joint normalization of two images */
  static void normalizeJointly(rsImage<T>& img1, rsImage<T>& img2);




  /** Scales the image up by the given factor by simply repeating pixel values */
  static rsImage<T> scaleUp(const rsImage<T>& img, int scl);

  // move code from GraphicsExperiments to here



};


#endif
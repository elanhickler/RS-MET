#ifndef RAPT_RAYBUBBLE_H_INCLUDED
#define RAPT_RAYBUBBLE_H_INCLUDED


/** A sound generator based on the idea of a particle/ray that bounces around inside an elliptic 
enclosure. 
maybe rename this to RayBubbleCore
*/

template<class T>
class rsRayBubble
{

public:

  /** \name Construction/Destruction */

  /** Constructor.  */
  rsRayBubble();


  /** \name Setup */

  /** Sets the initial position of the particle. */
  inline void setInitialPosition(T x, T y) { x0 = x; y0 = y; }

  /** Sets the angle at which our particle is launched from its initial position (in degrees). */
  inline void setLaunchAngle(T newAngle) { angle = T(PI*newAngle/180); }

  /** Sets the speed by which our particle moves around, i.e. the magnitude of the velocity, 
  which is a vector. */
  inline void setSpeed(T newSpeed) { speed = newSpeed; }
  // later replace this with setFrequency, setSampleRate...the speed is then proportional to
  // frequency/sampleRate - but maybe move this to some outside "driver" class

  /** Sets the parameters of the enclosing ellipse. */
  inline void setEllipseParameters(T newScale = 1, T newAspectRatio = 1, T newAngle = 0,
    T newCenterX = 0, T newCenterY = 0)
  {
    ellipse.setParameters(newScale, newAspectRatio, newAngle, newCenterX, newCenterY);
  }


  /** \name Processing */

  /** Computes one x,y-pair of output values at a time. */
  void getSampleFrame(T &x, T &y);

  /** Resets the internal state (position and velocity). */
  void reset();


protected:

  /** \name Internal Functions */

  /** Computes the intersection point between the current line segment along 
  xc + t*dx, yc + t*dy and the ellipse. */
  inline void getLineEllipseIntersectionPoint(T* xi, T* yi);

  /** Given a point xt,yt assumed to be on the ellipse, this function reflects the point
  x,y about the tangent at that point. */
  inline void reflectInTangentAt(T xt, T yt, T* x, T *y);

  /** Updates dx,dy by taking the the vector that points from the line/ellipse intersection point
  to the reflected point as new direction and adjusting its length according to the desired 
  speed. */
  inline void updateDirectionVector(T xi, T yi, T xn, T yn);


  /** \name Data */

  rsEllipse<T> ellipse; // enclosing ellipse

  // particle coordinates:
  T x0 = 0, y0 = 0;        // initial
  T xc = 0, yc = 0;        // current, maybe rename to x,y - take care in getSampleFrame to rename
                           // inputs

  // velocity components of particle (as increment per sample):
  T dx  = T(0.1), dy = T(0.2);  // current
  T speed = T(0.2);
  T angle = T(0.0);

};

#endif
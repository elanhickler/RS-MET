#ifndef RS_MODULARINTEGER_H
#define RS_MODULARINTEGER_H

namespace RSLib
{

  /**

  This is a class for representing integers modulo some modulus m. The arithmetic operators always 
  return values between 0...m-1 (inclusive) and it is assumed (and not checked), that the user 
  always initializes the value inside that range as well (for example, in constructors and 
  assignments).

  Note that, if you weant to use the division operator, you should instatiate the template-class 
  with a signed integer type (this is because computation of the modular inverse relies on the
  extended Euclid algorithm, which operates with signed values) - maybe we should get rid of the
  division operator anyway...

  class is not yet tested

  */

  template<class T>
  class rsModularInteger
  {

  public:

    /** \name Construction/Destruction */

    /** Constructor. You may initialize the number by passing some unsigned 64-bit integer. */
    rsModularInteger(rsUint64 initialValue, rsUint64 modulusToUse);

    /** Copy constructor. */
    rsModularInteger(const rsModularInteger& other);


    /** \name Operators */

    rsModularInteger& operator=(const rsBigInt& other);

    rsModularInteger operator-() const;

    bool operator==(const rsModularInteger& other) const;
    bool operator!=(const rsModularInteger& other) const;

    rsModularInteger operator+(const rsModularInteger& other);
    rsModularInteger operator-(const rsModularInteger& other);
    rsModularInteger operator*(const rsModularInteger& other);
    rsModularInteger operator/(const rsModularInteger& other);

    rsModularInteger& operator+=(const rsModularInteger& other);
    rsModularInteger& operator-=(const rsModularInteger& other);
    rsModularInteger& operator*=(const rsModularInteger& other);
    rsModularInteger& operator/=(const rsModularInteger& other);

    rsModularInteger& operator++();
    rsModularInteger& operator--();


    /** \name Data */

    T value, modulus;

  };

  /** Explicit template instantiation, to be used by the rsPow template-function. */
  template<class T>
  rsModularInteger<T> rsUnityValue(rsModularInteger<T> value);

}

#endif

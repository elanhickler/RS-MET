#ifndef RAPT_MATRIX_H
#define RAPT_MATRIX_H



template<class T>
class rsMatrix2x2
{

public:

  T a, b, c, d;
  // matrix coefficients |a b|
  //                     |c d|




  /** Adds two matrices: C = A + B. */
  rsMatrix2x2<T> operator+(const rsMatrix2x2<T>& B) const
  {
    rsMatrix2x2<T> C;
    C.a = a + B.a;
    C.b = b + B.b;
    C.c = c + B.c;
    C.d = d + B.d;
    return C;
  }

  /** Subtracts two matrices: C = A - B. */
  rsMatrix2x2<T> operator-(const rsMatrix2x2<T>& R) const
  {
    rsMatrix2x2<T> C;
    C.a = a - B.a;
    C.b = b - B.b;
    C.c = c - B.c;
    C.d = d - B.d;
    return C;
  }

  /** Multiplies two matrices: C = A * B. */
  rsMatrix2x2<T> operator*(const rsMatrix2x2<T>& B) const
  {
    rsMatrix2x2<T> C;
    C.a = A.a*B.a + A.b*B.c;
    C.b = A.a*B*b + A.b*B.d;
    C.c = A.c*B.a + A.d*B.c;
    C.d = A.c*B.b + A.d*B.d;
    return C;
  }

  //rsMatrix2x2<T> operator/(const rsMatrix2x2<T>& B) const { return *this * B.inverse(); }

  /** Multiplies matrix by a vector: w = A*v */
  rsVector2D<T> operator*(const rsVector2D<T>& v) const
  {
    rsVector2D<T> w;
    w.au = a * v.x  +  b * v.y;
    w.ad = c * v.x  +  d * v.y;
    return w;
  }
  // todo: left multiplication w = v^H * A

  /** Returns the commutator of the two matrices A and B: C = A*B - B*A. In general, matrix 
  multiplication is non-commutative, but for some special cases, it may be commutative nonetheless. 
  The commutator captures, how non-commutative two matrices behave when being multiplied. If the 
  two matrices commute (i.e. behave commutatively), their commutator is the zero matrix. */
  static rsMatrix2x2<T> commutator(const rsMatrix2x2<T>& A, const rsMatrix2x2<T>& B)
  {
    return A*B - B*A;
  }


  /** Returns the first eigenvalue of this matrix. */
  T eigenvalue1() const { return rsLinearAlgebra::eigenvalue2x2_1(a, b, c, d); }

  /** Returns the second eigenvalue of this matrix. */
  T eigenvalue2() const { return rsLinearAlgebra::eigenvalue2x2_2(a, b, c, d); }

  /** Returns the first eigenvector of this matrix. */
  rsVector2D<T> eigenvector1() const
  {
    rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_1(a, b, c, d, &v.x, &v.y, true); return v;
  }

  /** Returns the second eigenvector of this matrix. */
  rsQuantumSpin<T> eigenvector2() const
  {
    rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_2(a, b, c, d, &v.x, &v.y, true); return v;
  }

  // todo: determinant, inverse, etc.

};



//=================================================================================================

/** This is a class for treating C-arrays as matrices. It does not store/own the actual matrix 
data, it just acts as wrapper around an existing array for more conveniently accessing and 
manipulating matrix elements.

*/

template<class T>
class rsMatrixView
{

public:

  /** \name Construction/Destruction */




  /**  */
  rsMatrixView(int numRows = 0, int numColumns = 0, T* data = nullptr)
  {
    this->numRows = numRows;
    this->numCols = numColumns;
    d = data;
  }

  /** \name Setup */

  inline void setAllValues(T value)
  {
    rsArray::fillWithValue(d, int(numRows * numCols), value);
  }

  inline void reshape(int newNumRows, int newNumColumns)
  {
    rsAssert(newNumRows*newNumColumns == numRows*numCols);
    numRows = newNumRows;
    numCols = newNumColumns;
  }

  // void setToIdentityMatrix(T scaler = 1);


  /** \name Operators */

  /** Read and write access to matrix elements. */
  inline T& operator()(const int i, const int j)
  {
    return d[numCols*i + j];
    // more general colStride*i + rowStride*j
    // even more general: colOffset + colStride*i + (rowOffset + rowStride)*j
  }



protected:

  /** \name Data */

  //size_t N, M;    // number of rows and columns

  int numRows, numCols;
  T *d;           // data pointer

};

//=================================================================================================

/** This is a class for representing matrices and doing mathematical operations with them. */

template<class T>
class rsMatrixNew : public rsMatrixView<T>
{

public:

  /** \name Construction/Destruction */


  /** Standard constructor. You must pass the initial number of rows and columns */
  rsMatrixNew(int numRows = 0, int numColumns = 0);
  //rsMatrix(size_t numRows = 1, size_t numColumns = 1); // leads to memory leaks

 
  /** Copy constructor. */
  //rsMatrix(const rsMatrix& other);
                              
  /** Move constructor. */
  //rsMatrix(const rsMatrix&& other);


  /** Destructor. */
  ~rsMatrixNew() {}


  /** \name Setup */

  /** Sets the number of rows and columns, this matrix should have. ToDo: provide a way to retain 
  the data (optionally) - what does std::vector's resize do? Does it retain data...but if it does,
  it would be useless anyway in case the number of columns changed. */
  void setSize(int numRows, int numColumns);

    
  /** \name Manipulations */


  /** \name Inquiry */

    



  /** \name Decompositions */



  /** \name Data */

  std::vector<T> data;
  
}; 

  // some binary operators are defined outside the class such that the left hand operand does
  // not necesarrily need to be of class rsMatrix

  // matrix/vector functions:

  // todo: make some special-case classes for 2x2, 3x3 matrices which can use simpler algorithms
  // for some of the computations

#endif

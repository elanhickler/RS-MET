#ifndef RAPT_MATRIX_H
#define RAPT_MATRIX_H


/** A class for representing 2x2 matrices. They are treated as a special case because a lot of
things which are impractical in the general case can be done for the 2x2 case. For example, it's
possible to compute eigenvalues and eigenvectors via closed form formulas where in the general 
case numerical algorithms are needed. */

template<class T>
class rsMatrix2x2
{

public:

  T a, b, c, d;
  // matrix coefficients |a b|
  //                     |c d|


  /** Stadard constructor. You can pass the matrix elements. If you pass nothing, an identity
  matrix will be created. */
  //rsMatrix2x2(T a = T(1), T b = T(0), T c = T(0), T d = T(1)) { setValues(a, b, c, d); }
  // todo: maybe require arguments to be passed - or initialze teh matrix to the zero matrix
  // by default

  /** Constructor. Initializes elements with  given values. */
  rsMatrix2x2(T a, T b, T c, T d) { setValues(a, b, c, d); }

  /** Standard constructor. Leaves elements uninitialized.
  (...try to avoid using it - prefer RAII) */
  rsMatrix2x2() {}


  /** \name Setup */


  /** Sets up the elements of the matrix. */
  void setValues(T a, T b, T c, T d) { this->a = a; this->b = b; this->c = c; this->d = d; }


  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  // todo: prepend get

  /** Returns the determinant of this matrix. */
  T getDeterminant() const { return a*d - b*c; }

  /** Returns the trace (sum of diagonal elements) of this matrix.  */
  T getTrace() const { return a+d; }

  /** Returns the first eigenvalue of this matrix. */
  T getEigenvalue1() const { return rsLinearAlgebra::eigenvalue2x2_1(a, b, c, d); }

  /** Returns the second eigenvalue of this matrix. */
  T getEigenvalue2() const { return rsLinearAlgebra::eigenvalue2x2_2(a, b, c, d); }

  /** Returns the first eigenvector of this matrix. */
  rsVector2D<T> getEigenvector1() const
  { rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_1(a, b, c, d, &v.x, &v.y, true); return v; }

  /** Returns the second eigenvector of this matrix. */
  rsVector2D<T> getEigenvector2() const
  { rsVector2D<T> v; rsLinearAlgebra::eigenvector2x2_2(a, b, c, d, &v.x, &v.y, true); return v; }

  /** Returns the inverse of this matrix. */
  rsMatrix2x2<T> inverse() const
  { T D = getDeterminant(); T s = T(1) / D; return rsMatrix2x2<T>(s*d, -s*b, -s*c, s*a); }

  // maybe these functions should be named getDeterminant, etc. - more consistent with other
  // classes and states more explicitly what they do

  /** Tests, if another matrix B is close to this matrix within a given tolerance (all components
  of the difference must be <= tolerance). */
  bool isCloseTo(const rsMatrix2x2<T>& B, T tol)
  {
    if(rsAbs(a-B.a) <= tol && rsAbs(b-B.b) <= tol && rsAbs(c-B.c) <= tol && rsAbs(d-B.d) <= tol)
      return true;
    return false;
  }
  // doesn't work with complex matrices

  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Adds two matrices: C = A + B. */
  rsMatrix2x2<T> operator+(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a + B.a; C.b = b + B.b; C.c = c + B.c; C.d = d + B.d; return C; }
  // can't we just do:
  // return rsMatrix2x2<T>(a + B.a, b + B.b, c + B.c, d + B.d);
  // and likewise for the other operators

  /** Subtracts two matrices: C = A - B. */
  rsMatrix2x2<T> operator-(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a - B.a; C.b = b - B.b; C.c = c - B.c; C.d = d - B.d; return C; }

  /** Multiplies two matrices: C = A * B. */
  rsMatrix2x2<T> operator*(const rsMatrix2x2<T>& B) const
  { rsMatrix2x2<T> C; C.a = a*B.a + b*B.c; C.b = a*B.b + b*B.d;
    C.c = c*B.a + d*B.c; C.d = c*B.b + d*B.d; return C; }

  /** Multiplies the left matrix operand with the inverse of the right matrix operand. */
  rsMatrix2x2<T> operator/(const rsMatrix2x2<T>& B) const { return *this * B.inverse(); }

  /** Compares matrices for equality */
  bool operator==(const rsMatrix2x2<T>& B) const
  { return a == B.a && b == B.b && c == B.c && d == B.d; }

  /** Multiplies matrix by a vector: w = A*v */
  rsVector2D<T> operator*(const rsVector2D<T>& v) const
  {
    rsVector2D<T> w;
    w.x = a * v.x  +  b * v.y;
    w.y = c * v.x  +  d * v.y;
    return w;
  }
  // todo: left multiplication w = v^H * A

  // todo: operators that take a scalar as left or right argument


  //-----------------------------------------------------------------------------------------------
  /** \name Factory */

  static rsMatrix2x2<T> zero()     { return rsMatrix2x2<T>(T(0), T(0), T(0), T(0)); }

  static rsMatrix2x2<T> identity() { return rsMatrix2x2<T>(T(1), T(0), T(0), T(1)); }


  /** Returns the commutator of the two matrices A and B: C = A*B - B*A. In general, matrix
  multiplication is non-commutative, but for some special cases, it may be commutative nonetheless.
  The commutator captures, how non-commutative two matrices behave when being multiplied. If the
  two matrices commute (i.e. behave commutatively), their commutator is the zero matrix. */
  static rsMatrix2x2<T> commutator(const rsMatrix2x2<T>& A, const rsMatrix2x2<T>& B)
  {
    return A*B - B*A;
  }
  // see: https://en.wikipedia.org/wiki/Commutator#Ring_theory
  // maybe implement also the anticommutatior defined there as: {A,B} = A*B + B*A

};

/** Multiplies a scalar and a matrix. */
template<class T>
inline rsMatrix2x2<T> operator*(const T& s, const rsMatrix2x2<T>& A)
{
  return rsMatrix2x2<T>(s*A.a, s*A.b, s*A.c, s*A.d);
}


//=================================================================================================

/** This is a class for treating raw C-arrays as matrices. It does not store/own the actual matrix
data. It just acts as wrapper around an existing array for conveniently accessing and manipulating 
matrix elements via row/column indicies using the () operator with two integers. */

template<class T>
class rsMatrixView
{

public:

  //-----------------------------------------------------------------------------------------------
  /** \name Construction/Destruction */

  /** Default constructor. */
  rsMatrixView() {}

  /** Creates a matrix view with the given number of rows and columns for the given raw array of 
  values in "data". The view will *not* take ownership over the data. */
  rsMatrixView(int numRows, int numColumns, T* data)
  {
    rsAssert(numRows >= 1 && numColumns >= 1 && data != nullptr);
    this->numRows = numRows;
    this->numCols = numColumns;
    dataPointer = data;
  }

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Re-interprets the arrangement of the underlying array as having the new given numbers of rows
  and columns. Their product must remain the same, though. For example, you can reshape a 3x4 
  matrix into 4x3, 2x6, 6x2, 1x12, 12x1 but nothing else. ..todo: maybe lift that restriction? */
  void setShape(int newNumRows, int newNumColumns)
  {
    rsAssert(newNumRows*newNumColumns == numRows*numCols);
    numRows = newNumRows;
    numCols = newNumColumns;
  }
  // maybe rename to setShape for consistency with the rest of the library...otoh, reshape is 
  // consistent with NumPy

  /** Resets the number of rows and columns to zero and the dataPointer to nullptr. Should be called 
  whenever you need to invalidate our pointer member. */
  void reset()
  {
    numRows = 0;
    numCols = 0;
    dataPointer = nullptr;
  }

  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  /** Returns true, iff matrices A and B have the same number of rows and columns, such that 
  their sum A+B and difference A-B can be formed. */
  static bool areSameShape(const rsMatrixView<T>& A, const rsMatrixView<T>& B)
  { return A.numRows == B.numRows && A.numCols == B.numCols; }

  /** Returns true, iff the matrices A and B have dimensions, such that the matrix-matrix product 
  A*B can be formed. */
  static bool areMultiplicable(const rsMatrixView<T>& A, const rsMatrixView<T>& B)
  { return A.numCols == B.numRows; }

  /** Returns the number of rows. */
  int getNumRows()    const { return numRows; }

  /** Returns the number of columns. */
  int getNumColumns() const { return numCols; }

  /** Returns the total size, i.e. the number of elements in the matrix. */
  int getSize()       const { return numRows * numCols; }

  /** Returns true, iff this matrix is a row vector. */
  bool isRowVector() const { return numRows == 1; }

  /** Returns true, iff this matrix is a column vector. */
  bool isColumnVector() const { return numCols == 1; }

  /** Returns true, iff this matrix is either a row-vector or column-vector. */
  bool isVector() const { return isRowVector() || isColumnVector(); }

  /** Returns true, iff this matrix is a square matrix, i.e. has the same number of rows and 
  columns. */
  bool isSquare() const { return numRows == numCols; }

  /** Returns a const pointer to the data for read access as a flat array. */
  const T* getDataPointerConst() const { return dataPointer; }

  /** Returns a pointer to the data for read and write access as a flat array. */
  T* getDataPointer() { return dataPointer; }

  /** Returns a pointer to the stored data. When using this, be sure that you know exactly what
  you are doing.... */
  //T* getData() { return dataPointer; }

  /** Returns a pointer to a given row. */
  T* getRowPointer(int rowIndex)  { return &dataPointer[rowIndex*numCols]; }
  // if we later support column-major storage, we should assert that the matrix in row-major
  // storage

  /** Returns a const pointer to a given row. */
  const T* getRowPointerConst(int rowIndex) const { return &dataPointer[rowIndex*numCols]; }

  /** Return the number of data entries that overlap in matrix-views A and B. For many 
  computations, it is required, that A and B have non-overlapping data, so this function can be 
  used to check, if those operations can be done on two particular matrices. */
  /*
  static size_t getDataOverlap(const rsMatrixView<T>& A, const rsMatrixView<T>& B)
  {
    if(B.dataPointer < A.dataPointer)  // A's data must start before or at the same memory 
      return getDataOverlap(B, A);     // location as B's data - otherwise use swapped arguments

    size_t safeStartForB = A.dataPointer + A.getSize(); // 1 position after A's last slot
    if(B.dataPointer >= safeStartForB)
      return 0;  // no overlap
    else
      return rsMin(safeStartForB - B.dataPointer, B.getSize());
  }
  */
  // actually, we should move this to rsArray::getOverlap(T* x, size_t Nx, T*y, size_t Ny)
  // needs unit test

  //-----------------------------------------------------------------------------------------------
  /** \name Manipulation */

  /** Sets all matrix elements to zero. */
  void setToZero() { rsArray::fillWithZeros(dataPointer, getSize()); }

  /** Sets the matrix elements to the identity matrix, i.e. fills the main diagonal with ones and 
  the rest with zeros. If the matrix is not square, then the overhanging portion to the right or 
  bottom will be all zeros as well (-> verify this). */
  void setToIdentity() { setToZero(); setDiagonalValues(T(1)); }
  // needs test

  /** Sets all elements in the matrix to the given value. */
  void setAllValues(T value) { rsArray::fillWithValue(dataPointer, getSize(), value); }

  /** Initializes all elements with given value. */
  //void init(T value = T(0)) { RAPT::rsArray::fillWithValue(dataPointer, getSize(), value); }
  // maybe remove - is redundant with setAllValues

  /** Sets all elements on the main diagonal to the given value. If the matrix is not square, only
  the top- or left square submatrix will be affected. */
  void setDiagonalValues(T value)
  {
    for(int i = 0; i < rsMin(numRows, numCols); i++)
      dataPointer[i*numCols + i] = value;
  }
  // needs test

  /** Scales all elements in the matrix by a given factor. */
  void scale(T factor) { rsArray::scale(dataPointer, getSize(), factor); }

  /** Negates all values of the matrix, i.e. inverts their sign. */
  void negate() { rsArray::negate(dataPointer, dataPointer, getSize()); }

  // todo: conjugate


  /** Scales the row with given index by the given scale factor. */
  void scaleRow(int rowIndex, T scaler)
  {
    rsAssert(rowIndex >= 0 && rowIndex < numRows, "row index out of range");
    for(int j = 0; j < numCols; ++j)
      (*this)(rowIndex, j) *= scaler;
  }
  // needs test

  /** Scales the row with given index by the given scale factor. */
  void scaleColumn(int columnIndex, T scaler)
  {
    rsAssert(columnIndex >= 0 && columnIndex < numCols, "column index out of range");
    for(int i = 0; i < numCols; ++i)
      (*this)(i, columnIndex) *= scaler;
  }
  // needs test


  //-----------------------------------------------------------------------------------------------
  /** \name Arithmetic */

  /** Adds elements of A to corresponding elements in B and stores results in C. */
  static void add(const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(A, B) && areSameShape(A, *C), "arguments incompatible");
    rsArray::add(A.dataPointer, B.dataPointer, C->dataPointer, A.getSize());
  }

  /** Subtracts elements of B from corresponding elements A in and stores results in C. */
  static void subtract(const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(A, B) && areSameShape(A, *C), "arguments incompatible");
    rsArray::subtract(A.dataPointer, B.dataPointer, C->dataPointer, A.getSize());
  }

  /** Multiplies the two matrices element-wise. */
  static void elementwiseMultiply(
    const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(A, B) && areSameShape(A, *C), "arguments incompatible");
    rsArray::multiply(A.dataPointer, B.dataPointer, C->dataPointer, A.getSize());
  }

  /** Divides the two matrices element-wise. */
  static void elementwiseDivide(
    const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(areSameShape(A, B) && areSameShape(A, *C), "arguments incompatible");
    rsArray::divide(A.dataPointer, B.dataPointer, C->dataPointer, A.getSize());
  }

  /** Computes the matrix product C = A*B. */
  static void matrixMultiply(
    const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(A.numCols  == B.numRows);
    rsAssert(C->numCols == B.numCols);
    rsAssert(C->numRows == A.numRows);
    for(int i = 0; i < C->numRows; i++) {
      for(int j = 0; j < C->numCols; j++) {
        (*C)(i,j) = T(0);
        for(int k = 0; k < A.numCols; k++)
          (*C)(i,j) += A.at(i,k) * B.at(k,j); }}
  }
  // rename to matrixMultiply maybe implement matrixDivide: A/B := A * inverse(B) 


  /** Fills the matrix B with the transpose of matrix A. Assumes that A and B have compatible 
  shapes. Matrix A and B must point to non-overlapping arrays */
  static void transpose(const rsMatrixView<T>& A, rsMatrixView<T>* B)
  {
    //rsAssert(A.dataPointer != B->dataPointer, "can't be used in place"); 
    //rsAssert(getDataOverlap(A, *B) == 0, "can't be used with overlapping matrices"); 
    rsAssert(A.numRows == B->numCols);
    rsAssert(A.numCols == B->numRows);
    for(int i = 0; i < A.numRows; i++)
      for(int j = 0; j < A.numCols; j++)
        (*B)(j,i) = A.at(i,j);
  }
  // B is a pointer and not a reference because it's an output - adopt that idiom generally:
  // output variables are always passed as pointers, never as references

  /** Transposes the square matrix A in place. */
  static void transposeSquare(rsMatrixView<T>* A)
  {
    rsAssert(A->isSquare());
    for(int i = 0; i < A->getNumRows(); i++)
      for(int j = i+1; j < A->getNumRows(); j++)
        rsSwap((*A)(i,j), (*A)(j,i));
  }

  /** Computes the Kronecker product between matrices A and B and stores the result in C. Assumes, 
  that C has the right dimensions. For more info, see the documentation of 
  rsMatrix::kroneckerProduct. */
  static void kroneckerProduct(
    const rsMatrixView<T>& A, const rsMatrixView<T>& B, rsMatrixView<T>* C)
  {
    rsAssert(C->numRows == A.numRows * B.numRows);
    rsAssert(C->numCols == A.numCols * B.numCols);
    for(int ia = 0; ia < A.numRows; ia++) {
      for(int ja = 0; ja < A.numCols; ja++) {
        int startRow = ia*B.numRows;
        int startCol = ja*B.numCols;
        for(int ib = 0; ib < B.numRows; ib++) {
          for(int jb = 0; jb < B.numCols; jb++) {
            (*C)(startRow+ib, startCol+jb) = A.at(ia,ja) * B.at(ib, jb); }}}}
  }

  //-----------------------------------------------------------------------------------------------
  /** \name Accessors */

  /** Read and write access to matrix elements with row-index i and column-index j. */
  T& operator()(const int i, const int j) { return dataPointer[flatIndex(i, j)]; }

  /** Read only access to matrix elements with row-index i and column-index j. */
  const T& operator()(const int i, const int j) const
  {
    return dataPointer[flatIndex(i, j)];
  }

  /** Read only access - used mainly internally with const reference arguments (for example,
  in add). */
  const T& at(const int i, const int j) const { return dataPointer[flatIndex(i, j)]; }
  // maybe rename to get - do we actually need this? - if not, get rid!

  // void set(i, j, val) ...to make it compatible with old implementation

  /** Converts a row index i and a column index j to a flat array index. */
  int flatIndex(const int i, const int j) const
  {
    return numCols*i + j;
    // todo:
    //  -be more general: colStride*i + rowStride*j. goal: allow row-major and column-major storage
    //   while the syntax of the () operator is always row-major (as is conventional in math)
    //   regardless whatever the internal storage format is - column major storage is required for
    //   compatibility with lapack
    // -maybe be even more general: colOffset + colStride*i + (rowOffset + rowStride)*j
    //  -> may allow to access sub-matrices with the same syntax (todo: verify formula)
    //  ...but maybe that should be done in a class rsSubMatrixView
  }


protected:

  /** \name Data */

  int numRows = 0, numCols = 0;  // number of rows and columns
  T *dataPointer = nullptr;      // pointer to the actual data

};


//=================================================================================================

/** This is a class for representing matrices and doing mathematical operations with them. It's 
implemented as subclass of rsMatrixView and stores the actual matrix data in a std::vector. Copy- 
and move constructors and -assignment operators have been implemented in order to avoid 
unnecessary heap allocations in arithmetic expressions with matrices (return value copy 
elision). */

template<class T>
class rsMatrix : public rsMatrixView<T>
{

public:

  //-----------------------------------------------------------------------------------------------
  /** \name Construction/Assignment/Destruction */

  /** Standard constructor. You must pass the initial number of rows and columns */
  rsMatrix(int numRows = 0, int numColumns = 0)
  {
    setSize(numRows, numColumns);
    // todo: optionally init with zeros
  }

  /** Constructor to create a matrix from an array-of-arrays - mostly for conveniently converting
  matrices in the old representation into the new one. */
  rsMatrix(int numRows, int numColumns, T** data)
  {
    setSize(numRows, numColumns);
    for(int i = 0; i < numRows; i++)
      for(int j = 0; j < numColumns; j++)
        (*this)(i,j) = data[i][j];
  }

  /** Destructor. */
  ~rsMatrix()
  {
    //int dummy = 0; // to figure out, when it gets called for debugging
  }

  /** Creates matrix from a std::vector.  */
  rsMatrix(int numRows, int numColumns, const std::vector<T>& newData) : data(newData)
  {
    numHeapAllocations++;   // data(newData) allocates
    rsAssert(numRows*numColumns == newData.size());
    this->numRows = numRows;
    this->numCols = numColumns;
    updateDataPointer();
  }

  /** Creates matrix from an unnamed/temporary/rvalue std::vector - convenient to initialize 
  elements. You can initialize matrices like this:
    rsMatrix<double> A(2, 3, {1.,2.,3., 4.,5.,6.});   */
  rsMatrix(int numRows, int numColumns, std::vector<T>&& newData) : data(std::move(newData))
  {
    numHeapAllocations++;             // we count the allocation that took place in the caller
    rsAssert(newData.size() == 0);
    rsAssert(numRows*numColumns == data.size());
    this->numRows = numRows;
    this->numCols = numColumns;
    updateDataPointer();
  }

  /** Copy constructor. Copies data from B into this object.  */
  rsMatrix(const rsMatrix& B)
  {
    setSize(B.numRows, B.numCols);
    rsArray::copy(B.dataPointer, this->dataPointer, this->getSize());
  }

  /** Move constructor. Takes over ownership of the data stored in B. */
  rsMatrix(rsMatrix&& B) : data(std::move(B.data))
  {
    rsAssert(B.data.size() == 0); // B's data has now become our data
    this->numRows = B.numRows;
    this->numCols = B.numCols;
    updateDataPointer();
    B.reset();                    // invalidates pointer in B
  }

  /** Copy assignment operator. Copies data from rhs into this object. */
  rsMatrix<T>& operator=(const rsMatrix<T>& rhs)
  {
    if (this != &rhs) { // self-assignment check expected
      setSize(rhs.numRows, rhs.numCols);
      rsArray::copy(rhs.dataPointer, this->dataPointer, this->getSize());
    }
    return *this;
  }

  /** Move assignment operator. Takes over ownership of the data stored in rhs. */
  rsMatrix<T>& operator=(rsMatrix<T>&& rhs)
  {
    data = std::move(rhs.data);
    rsAssert(rhs.data.size() == 0);
    this->numRows = rhs.numRows;
    this->numCols = rhs.numCols;
    updateDataPointer();
    rhs.reset();
    return *this;
  }

  /** Creates a zero matrix with given number of rows and columns. */
  static rsMatrix<T> zero(int numRows, int numColumns) 
  { rsMatrix<T> Z(numRows, numColumns); Z.setToZero(); return Z; }

  /** Creates an identity matrix of given size. */
  static rsMatrix<T> identity(int size) 
  { rsMatrix<T> E(size, size); E.setToIdentity(); return E; }

  // todo: diag(int size, T* data), diag(int size, T value)

  //-----------------------------------------------------------------------------------------------
  /** \name Setup */

  /** Sets the number of rows and columns, this matrix should have. ToDo: provide a way to retain
  the data (optionally) - what does std::vector's resize do? Does it retain data...but if it does,
  it would be useless anyway in case the number of columns changed. */
  void setSize(int numRows, int numColumns)
  {
    if(numRows == this->numRows && numColumns == this->numCols)
      return;  // nothing to do

    this->numRows = numRows;
    this->numCols = numColumns;
    data.resize(this->numRows * this->numCols);
    numHeapAllocations++;                        // data.resize() may have re-allocated heap memory
    updateDataPointer();
    // optionally initialize with zeros
  }


  //-----------------------------------------------------------------------------------------------
  /** \name Manipulations */




  /** Transposes this matrix, i.e. the rows become columns and vice versa. Avoids reallocation in 
  case of square-matrices and row- and column vectors. */
  void transpose()
  {
    if(isSquare()) { rsMatrixView<T>::transposeSquare(this); return; }
    if(isVector()) { rsSwap(numRows, numCols); return; } 

    std::vector<T> v(getSize());    // the general case needs reallocation...
    numHeapAllocations++;
    rsMatrixView<T> B(numCols, numRows, &v[0]);
    rsMatrixView<T>::transpose(*this, &B);
    rsSwap(numRows, numCols);
    data = v;
    // don't we have to call updateDataPointer()? -> check, if there's a unit test
  }
  // maybe move to cpp file



  //-----------------------------------------------------------------------------------------------
  /** \name Inquiry */

  /** Returns a const reference to the std::vector that stores the data. Mostly for unit tests. */
  const std::vector<T>& getDataVectorConst() const { return data; }

  // todo: getDeterminant, getInverse, getFrobeniusNorm, get[Other]Norm, isPositiveDefinite, 
  // getEigenvalues, getTrace, isUpperLeftTriangular, getTransposed, getConjugateTransposed


  //-----------------------------------------------------------------------------------------------
  /** \name Computations */

  /** Computes the Kronecker product between matrices A and B. For a 3x2 matrix A, it looks like:

              |a11*B a12*B|
  A (x) B  =  |a21*B a22*B|
              |a31*B a32*B|

  Where each entry aij*B is a submatrix of dimensions of B with the entries of B scaled by the
  respective element from A. This product is also sometimes called tensor product, but i think, 
  Kronecker product is more appropriate, as it explicitly deals with matrices rather than vector
  spaces. Also, in the context of tensor algebra, the tensor product of two rank-2 tensors 
  (i.e. matrices) would give a rank-4 tensor (with 4 indicies), whereas this product here still has
  just 2 indices - it is again a matrix and not some 4-dimensional "block". See here:
  https://en.wikipedia.org/wiki/Kronecker_product
  https://en.wikipedia.org/wiki/Tensor_product     */
  static rsMatrix<T> getKroneckerProduct(const rsMatrix<T>& A, const rsMatrix<T>& B) 
  {
    rsMatrix<T> C(A.numRows*B.numRows, A.numCols*B.numCols);
    rsMatrixView<T>::kroneckerProduct(A, B, &C);
    return C;
  }

  /** Returns a matrix that is the element-wise product of this matrix an the right operand. */
  rsMatrix<T> getElementwiseProduct(const rsMatrixView<T>& rightOperand) const
  {
    rsMatrix<T> result(numRows, numCols);
    rsMatrixView<T>::elementwiseMultiply(*this, rightOperand, &result);
    return result;
  }
  // it's intentional that the rsMatrixView method is called multiply and this here is called 
  // product - the view method just does something (-> use a verb), the method here returns an
  // objcet (-> use a noun)

  // todo: getInverse, getTranspose


  //-----------------------------------------------------------------------------------------------
  /** \name Decompositions */

  // getLowerUpperDecomposition ...or decomposeLU, decomposeQR, decomposeSVD


  //-----------------------------------------------------------------------------------------------
  /** \name Operators */

  /** Compares matrices for equality */
  bool operator==(const rsMatrix<T>& rhs) const
  {
    if(this->numRows != rhs.numRows || this->numCols != rhs.numCols)
      return false;
    return rsArray::equal(this->dataPointer, rhs.dataPointer, this->getSize());
  }
  // maybe move to rsMatrixView, if possible

  /** Compares matrices for inequality */
  bool operator!=(const rsMatrix<T>& rhs) const { return !(*this == rhs); }

  /** Returns the negative of this matrix. */
  rsMatrix<T> operator-()
  {
    rsMatrix<T> C(this->numRows, this->numCols);
    for(int i = 0; i < this->getSize(); i++)
      C.dataPointer[i] = -this->dataPointer[i]; // maybe factor out into "neg" function in baseclass
    return C;
  }

  /** Adds two matrices: C = A + B. */
  rsMatrix<T> operator+(const rsMatrix<T>& B) const
  { rsMatrix<T> C(this->numRows, this->numCols); this->add(*this, B, &C); return C; }

  /** Subtracts two matrices: C = A - B. */
  rsMatrix<T> operator-(const rsMatrix<T>& B) const
  { rsMatrix<T> C(this->numRows, this->numCols); this->subtract(*this, B, &C); return C; }

  /** Multiplies two matrices: C = A * B. */
  rsMatrix<T> operator*(const rsMatrix<T>& B) const
  { 
    //if(!areMultiplicable(*this, B))
    //  return rsMatrix<T>(0, 0); // return empty matrix when attempting to multiply incompatible matrices
    rsMatrix<T> C(this->numRows, B.numCols); 
    this->matrixMultiply(*this, B, &C); 
    return C; 
  }
  // maybe it should return an empty matrix, when attempting to multiply incompatible matrices
  // ...or maybe we should throw an exception in such cases?


  /** Adds another matrix to this matrix and returns the result. */
  rsMatrix<T>& operator+=(const rsMatrix<T>& B)
  { this->add(*this, B, this); return *this; }

  /** Subtracts another matrix from this matrix and returns the result. */
  rsMatrix<T>& operator-=(const rsMatrix<T>& B)
  { this->subtract(*this, B, this); return *this; }

  /** Multiplies this matrix by another and returns the result. This is not an in-place process, i.e. it 
  will allocate temporary heap-memory. */
  rsMatrix<T>& operator*=(const rsMatrix<T>& B)
  { *this = *this * B; return *this; } 


  /** Multiplies this matrix with a scalar s: B = A*s. The scalar is to the right of the matrix. */
  rsMatrix<T> operator*(const T& s) const
  { rsMatrix<T> B(*this); B.scale(s); return B; }

  /** Multiplies this matrix by a scalar and returns the result. */
  rsMatrix<T>& operator*=(const T& s)
  { scale(s); return *this; }

  /** Divides this matrix by a scalar and returns the result. */
  rsMatrix<T>& operator/=(const T& s)
  { scale(T(1)/s); return *this; }


  //-----------------------------------------------------------------------------------------------
  /** \name Misc */

  static int numHeapAllocations;
    // instrumentation for unit-testing - it's actually the number of *potential* heap-allocations,
    // namely, the number of calls to data.resize() which may or may not re-allocate memory
    // maybe get rid of this and implement the allocation test using a custom allocator

protected:

  /** Updates the data-pointer inherited from rsMatrixView to point to the begin of our std::vector
  that holds the actual data. */
  void updateDataPointer()
  {
    if(data.size() > 0)
      this->dataPointer = &data[0];
    else
      this->dataPointer = nullptr;
  }


  /** \name Data */

  std::vector<T> data;

};

template<class T> int rsMatrix<T>::numHeapAllocations = 0;

/** Multiplies a scalar and a matrix. */
template<class T>
inline rsMatrix<T> operator*(const T& s, const rsMatrix<T>& A)
{
  rsMatrix<T> B(A);
  B.scale(s);
  return B;
}






template<class T>
rsMatrix<T> matrixMagnitudes(const rsMatrix<std::complex<T>>& A)
{
  int N = A.getNumRows();
  int M = A.getNumColumns();
  rsMatrix<T> mags(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      mags(i, j) = abs(A(i, j));
  return mags;
}

template<class T>
rsMatrix<T> matrixPhases(const rsMatrix<std::complex<T>>& A)
{
  int N = A.getNumRows();
  int M = A.getNumColumns();
  rsMatrix<T> phases(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      phases(i, j) = arg(A(i, j));
  return phases;
}

// maybe factor out common code...maybe something like applyMatrixFunction with different
// input and output types for the template parameter:

/*
template<class TIn, class TOut, class F>
rsMatrix<TOut> matrixFunction(const rsMatrix<TIn>& A, F func)
{
  int N = A.getNumRows();
  int M = A.getNumColumns();
  rsMatrix<TOut> out(N, M);
  for(int i = 0; i < N; i++)
    for(int j = 0; j < M; j++)
      out(i, j) = func(A(i, j));
  return out;
}
*/


//-------------------------------------------------------------------------------------------------
// Notes:
// -design goals:
//  -use std::vector to hold the data in a flat array (so we can inspect it in the debugger)
//  -storage format should be compatible with lapack routines (maybe not by default, but can be
//   made so) - that means to support column major storage
// -maybe we should just work with our inherited dataPointer and use new/delete
//  -saves a little bit of storage
//  -but then we can't look easily at the data in the debugger anymore -> very bad! so, nope!
//  -the little storage overhead of std::vector becomes negligible for all but the smallest
//   matrices - on the other hand, very small matrices may be common
//  -maybe use std::vector in debug builds and new/delete in release builds
//  -maybe the numHeapAllocations variable can also be used only in debug builds? i guess we may 
//   need to define some function incrementAllocationCounter that reduces to no-op in release 
//   builds
//  -in expressions like rsMatrix<float> C = B*(A + B) + B; we want to avoid copying the data
//   unnecessarily - i.e. avoid that the temporaries that occur inside this expression use heap
//   allocation only when absolutely necessarry
//   ...this especially means, we need to pass the return values of the arithmetic operators by
//   reference rather than by value - typically, in an implementation like
//  -ideally, i want to be able to use it in production code for realtime processing...but that
//   may not be possible...but maybe with rsMatrixView, it is?
//
//    rsMatrix<T> operator+(const rsMatrix<T>& B) const
//    { rsMatrix<T> C(this->numRows, this->numCols); this->add(this, &B, &C); return C; }
//
//   we have one constructor call (-> heap allocation) to create C - but C is a local variable - to
//   return it to the caller, there would be *another* (copy?)constructor call - ...right? and that
//   second call is what we want to get rid of

// ToDo: 
//  -maybe rename to rsMatrixOnHeap and have a similar class rsMatrixOnStack (maybe using
//   std::dynarray instead of std::vector)...or by defining the size via template parameters at 
//   compile time ...maybe the std::vector vs std::dynarrray distinction can determined by passing
//   the storage container as template argument like so:
//   template<class ElemType, class ContainerType>
//   class rsMatrix { ContainerType<ElemType> data; };
//  -maybe make subclasses rsRowVector, rsColumnVector with a simplified element access operator 
//   that takes only one index


// maybe see here:
// https://www.youtube.com/watch?v=PNRju6_yn3o
// https://www.ibm.com/developerworks/community/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/RVO_V_S_std_move?lang=en
// https://en.cppreference.com/w/cpp/language/copy_elision

#endif

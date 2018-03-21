#ifndef Tensor_h
#define Tensor_h

#include <vector>

/** Implements a simple proof-of-concept class for tensors 

A tensor can be seen as an r-dimensional array of values. Here, r is called the "rank" of the 
tensor and it is the number of indices. This is not to be confused with the dimensionality of the 
underlying vector space - which is the number of values over which each of the indices can run. For
example, 3x3 matrix is a 2-dimensional array (2 indices) where each index runs over 3 values. Such 
a matrix could represent a tensor of rank 2. Tensors are always (hyper) cubical in shape - which in 
the case of rank 2 boils down to a square matrix - a 2x3 matrix, for example, can not be 
interpreted as tensor.

A tensor can also be interpreted as a function that takes a list of vectors and/or covectors as 
inputs and produces a scalar as result. This function is linear in all its inputs. If normal 
vectors are represented column-vectors, covectors can be seen as row-vectors - they are also 
functions that take one vector as input and produce a scalar, which in this case, is basically the 
familiar scalar product. Covectors live in a vector space that is dual to the regular space in 
which the vectors live. 

In tensors, there are two kinds of indices: upper and lower, i.e. superscripts and subscripts. The 
upper indices are also called contravariant and the lower indices covariant. It is sometimes 
convenient to split the total rank of a tensor into an upper and lower part such that r = m+n and
the pair of upper/lower ranks is written as (m,n). A vector has a single upper index and a covector
has a single lower index. In this notation, they have ranks (1,0) or (0,1) respectively.


*/

class Tensor
{

public:

  Tensor(int numDimensions, int numSuperscripts, int numSubscripts);

  //-----------------------------------------------------------------------------------------------
  // \name Setup



  //-----------------------------------------------------------------------------------------------
  // \name Inquiry

  /** Returns the total number of indices of this tensor which is also knwon as the rank. */
  int getRank() const { return rank; }

  /** Returns the number of superscripts (upper, contravariant indices) of this tensor. */
  int getNumSuperscripts() const;

  /** Returns the number of subscripts (lower, covariant indices) of this tensor. */
  int getNumSubscripts() const;

  /** Returns whether or not the given index is a superscript. */
  bool isSuperscript(int index) const { return subscript[index] == false; }

  /** Returns whether or not the given index is a subscript. */
  bool isSubscript(int index) const { return subscript[index] == true; }
  
  /** Returns the number of dimensions of the underlying vector space. Each index from 0 to rank-1 
  can run from 0 to this number minus one. */
  int getNumDimensions() const { return dim; }

  /** Returns true, if this tensor is a scalar, i.e. a rank-0 tensor. */
  bool isScalar() const { return rank == 0; }
  
  /** Returns true, if this tensor is a vector, i.e. a rank-1 tensor with one superscript. */
  bool isVector() const  { return rank == 1 && subscript[0] == false; }
  
  /** Returns true, if this tensor is a covector, i.e. a rank-1 tensor with one subscript. 
  Covectors are also called 1-forms. */
  bool isCovector() const { return rank == 1 && subscript[0] == true;  }

  /** Returns true, of the tensor is symmetric with respect to the given pair of indices. When you
  retrieve the the component with exchanged indices, you'll get the same value as without 
  exchange. Also, when you exchange two vectors/covectors in an input list at these indices, the 
  result of applying the tensor to the input list will be the same. */
  bool isSymmetric(int index1, int index2) const;

  /** Returns true, of the tensor is anti symmetric with respect to the given pair of indices. When 
  you retrieve the the component with exchanged indices, you'll get the negative of the value without 
  exchange.*/
  bool isAntiSymmetric(int index1, int index2) const;

  // getSymmetricPart, getAntiSymmetricPart
  // maybe have simplified versions for rank2 tensors



  //double at(/*list of indices*/);


  //-----------------------------------------------------------------------------------------------
  // \name Operations
  
  /** A contraction is an operation on a tensor that returns a tensor of lower rank. The number of
  superscripts and the number of subscripts each derease by one. A contraction must always involve
  a superscript and a subscript ... */
  Tensor contract(int index1, int index2) const;

  /** An outer product between two tensors of rank (m,n) and (p,q) yields a tensor of rank 
  (m+p.n+q). */
  Tensor outerProduct(const Tensor& A, const Tensor& B) const;
  
  /** .... An inner product between two tensors can be seen as an outer product followed by a 
  contraction. */
  Tensor innerProduct(const Tensor& A, const Tensor& B, int indexA, int indexB) const;
   // not yet sure, if the signature should be like that

  // double apply(/*list of vectors and/or covectors*/); 
  // should produce a scalar for a given input list of vectors and or covectors...maybe have a 
  // generalized version that returns a tensor of any rank (the tensor is applied only partially)
  // we may need to have a way to pass in a special placeholder tensor for empty slots in the input
  // list
  // raiseIndex(int index, const Tensor& metric); lowerIndex(...)
  
  // operators: +,-,==,unary-, element access (i,j,k,...)

  

protected:

  int rank = 0;                // number of indices of the tensor
  int dim = 1;                 // number of dimensions of the (co)vector space
  std::vector<double> values;  // holds the values as flat array (size = dim^rank)
  std::vector<bool> subscript; // indicates if a particular index is a subscript
  
};



#endif
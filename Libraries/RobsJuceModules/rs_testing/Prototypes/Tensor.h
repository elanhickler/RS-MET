#ifndef Tensor_h
#define Tensor_h

#include <vector>

/** Implements a simple proof-of-concept class for tensors 

A tensor can be seen as an r-dimensional array of values. Here, r is called the "rank" of the 
tensor and it is the number of indices. This is not to be confused with the dimensionality of the 
underlying vector space - which is the number of values over which each of the indices can run. For
example, a 3x3 matrix is a 2-dimensional array (2 indices) where each index runs over 3 values. 
Such a matrix could represent a tensor of rank 2 when the underlying vector space is 3 dimensional.
Tensors are always (hyper) cubical in shape - which in the case of rank 2 boils down to a square 
matrix - a 2x3 matrix, for example, can not be interpreted as tensor.

...is that actually ture - see here, at around 5:30:
https://www.youtube.com/watch?v=dtvM-CzNe50&list=PLJHszsWbB6hrkmmq57lX8BV-o-YIOFsiG&index=10
maybe, we need a more general definition that is not necessarily hypercubic?

Mathematically, tensors are defined to obey certain transformation rules when you change the basis
of the vector space. Not just any hypercubical array of values qualifies as a tensor. It must obey
these transformation rules - otherwise, it's not a tensor. In a regular vector, for example - seen 
as a tensor of rank 1, the component values must change inversely to the way the basis vectors 
change under a given change of basis (which is a linear transformation, represented by a matrix). 
That's why the components of a vector are called contravariant - they change contrary to the basis 
vectors. This equal and opposite change ensures, besides other things, that the scalar product of 
two vectors stays invariant under a change of basis. So, the scalar product - and therefore also 
the norm of a vector - is independent from the basis chosen - as it must be to be interpreted as a 
geometrical quantity. The same must be true for any physical quantity: they just are what they are 
and their values should not depend on any particular choice of coordinate system. That's why 
physical quantities usually *are* tensors.

In addition to the vector space of the regular vectors, there's a dual space of so-called 
covectors. The components of these covectors are covariant - they change excatly in the same way as
the basis vectors do under a change of basis. A tensor can also be interpreted as a function that 
takes a bunch of vectors and/or covectors as inputs and produces a scalar as result. This function 
is linear in all its inputs. That's why a tensor is also sometimes called a multilinear form. If 
regular vectors are represented as column-vectors, covectors can be seen as row-vectors - they are 
also functions that take one vector as input and produce a scalar, which in this case, is basically 
the familiar scalar product. But this scalar product is usually generalized to include a matrix in 
the middle - which in this context is known as the metric tensor that defines the "metric", i.e. 
the distance measure, of the vector space. If the metric tensor is the identity matrix, then it 
reduces to the plain old scalar product. 

In tensors, there are two kinds of indices: upper and lower, i.e. superscripts and subscripts. The 
upper indices are the contravariant ones and the lower indices the covariant ones. It is sometimes 
convenient to split the total rank r of a tensor into an upper and lower part such that r = m+n and
the pair of upper/lower ranks is written as (m,n). A vector has a single upper index and a covector
has a single lower index. In this notation, they have ranks (1,0) or (0,1) respectively. In this
notation, a rank (m,n) tensor would take n vectors and m covectors as inputs (or is it the other 
way around? look up..) and produce a scalar output. */

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
  Covectors are also called 1-forms or linear forms. */
  bool isCovector() const { return rank == 1 && subscript[0] == true;  }

  /** Returns true, if this tensor is a linear form which is the same thing as a covector. Linear 
  forms take a vector as input and produce a scalar output. */
  bool isLinearForm() const { return isCovector(); } 

  /** Returns true, if this tensor is a linear map. Linear maps may take a vector as input and 
  produce another vector as output (V -> V) or take a covector input and produce a covector output 
  (V* -> V*). It may also take a vector and covector as input and produce a scalar (V x V* -> R
  or V* x V -> R).
  
  I think in matrix notation, the first case would be y = A * x and the second case y = x^T * A 
  where x is a column vector in both cases and y is a column in the 1st and a row in the 2nd
  */
  bool isLinearMap() const { return rank == 2 && subscript[0] == false && subscript[1] == true; }
  // is it true the the 2nd index must be a subscript - or should it be the 1st? ...but i think
  // 2nd is correct - in an expression y^i = A^i_j x^j with implied summation, we sum over the 
  // 2nd index in the matrix A - and ina scalar product, the 2nd index is the column
  // linear maps can be created by taking appropriate linear combinations of outer products of all 
  // pairs of basis-vectors and basis-covectors (these outer products form basis linear maps - the
  // space of linear maps is itself a vector space)

  /** Returns true, if this tensor is a bilinear form. Bilinear forms may take two vectors as input
  and return a scalar (V x V -> R). That implies that they have two subscripts which are summed 
  over when they act on two vectors (each having a superscript). The metric tensor is an example of
  such a bilinear map. They may also take a single vector as input in which case they produce a 
  covector (= linear form) as output (V -> V*) - that's a bit like a partial evaluation. In this 
  V -> V* case, two different covectors / linear maps are possible as outputs, depending on which 
  index is summed over
  
  */
  bool isBilinearForm() const 
  { return rank == 2 && subscript[0] == true && subscript[1] == true; }
  // bilinear forms can be created by taking appropriate linear combinations of outer products of 
  // all basis covector-covector pairs

  // is bilinear form the same as quadratic form? i think so..or well, only when both input vectors
  // are the same vector

  // what if subscript[0] is true for rank-2 tensors? 



  /** Returns true, of the tensor is symmetric with respect to the given pair of indices. When you
  retrieve the component with exchanged indices, you'll get the same value as without 
  exchange. Also, when you exchange two vectors/covectors in an input list at these indices, the 
  result of applying the tensor to the input list will be the same. */
  bool isSymmetric(int index1, int index2) const;

  /** Returns true, of the tensor is anti symmetric with respect to the given pair of indices. When 
  you retrieve the component with exchanged indices, you'll get the negative of the value without 
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
  // list ...maybe just the scalar 1 can serve this role? 



  // Tensor raiseIndex(int index, const Tensor& metric); 
  
  /** Given a (contravariant) vector v^i (1 superscript i) and a metric g_ij tensor (two subscripts
  i,j), this function computes the (covariant) covector v_i (1 subscript i) that corresponds to v^i. 
  It is given by: v_i = g_ij v^j
  ....what if v is not a vector but a general tensor?  */
  Tensor lowerIndex(const Tensor& v, int index, const Tensor& metric);
  
  /** Inverse operation of lowerIndex: v^k = g^ki v_i where g^ki is the inverse metric tensor with
  two superscripts k,i. the inverse metric tensor is sometimes called contravariant metric tensor
  */
  Tensor raiseIndex(const Tensor& v, int index, const Tensor& inverseMetric);
  
  // raising the 2nd index of Q = Q^i_jk in V x V* x V*
  // would be achieved like this: Q^i_jk g^jx = Q^ix_k
  // pick the index to raise (j), multiply it by the inverse metric tensor where the 1st index
  // corresponds to our target index (j) and the 2nd index is a dummy (x). in the result, the dummy
  // index (x) can then be renamed back to j. the result is now in V x V x V* - the 2nd V has no 
  // star anymore
  //

  // operators: +,-,==,unary-, element access (i,j,k,...) - make different versions for various 
  // numbers of indices

  

protected:

  int rank = 0;                // number of indices of the tensor
  int dim = 1;                 // number of dimensions of the (co)vector space
  std::vector<double> values;  // holds the values as flat array (size = dim^rank)
  std::vector<bool> subscript; // indicates if a particular index is a subscript
  
};



#endif
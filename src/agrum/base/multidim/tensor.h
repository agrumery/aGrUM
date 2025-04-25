/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief Header of the Tensor class.
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_TENSOR_H
#define GUM_TENSOR_H

#include <functional>
#include <vector>

#include <agrum/agrum.h>

#include <agrum/base/multidim/implementations/multiDimDecorator.h>

namespace gum {
  // ==========================================================================
  // ===                          TENSOR                                 ===
  // ==========================================================================

  /**
   * @class Tensor
   * @headerfile tensor.h <agrum/base/multidim/tensor.h>
   * @ingroup multidim_group
   *
   * @brief aGrUM's Tensor is a multi-dimensional array with tensor operators.
   * It is used to represent probabilities and utilities in aGrUMs'
   * multidimensional (graphical) models.
   *
   * Using the decorator pattern, this representation is independent from the
   * implementation of the multidimensional matrix.
   *
   * @tparam GUM_SCALAR The type of the scalar stored in this multidimensional
   * matrix.
   *
   * @warning  The Tensor class is not thread-safe. If you want to use it in a
   * threaded context, you have to use a mutex to protect the access to the
   * class.
   *
   * @warning The Tensor class does not contain its variables. It only contains
   * (maybe dangling) pointers to the variables.
   */
  template <typename GUM_SCALAR>
  class Tensor final: public MultiDimDecorator< GUM_SCALAR > {
    public:
    static Tensor< GUM_SCALAR > deterministicTensor(const DiscreteVariable& var,
                                                    Idx                     value);
    static Tensor< GUM_SCALAR > deterministicTensor(const DiscreteVariable& var,
                                                    const std::string&      label);
    static Tensor< GUM_SCALAR > uniformTensor(const DiscreteVariable& var);
    // =========================================================================
    /// @name Constructors, Destructors and Copy
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Creates an empty null dimensional matrix with a MultiDimArray as
     * its implementation.
     */
    Tensor();

    /**
     * @brief Default constructor.
     *
     * Creates an empty null dimensional matrix with a MultiDimArray as
     * its implementation and add the vars
     */
    Tensor(const std::vector< const DiscreteVariable* >& vars);

    /**
     * @brief Creates an tensor around aContent.
     * @param aContent The implementation of this Tensor.
     */
    explicit Tensor(MultiDimImplementation< GUM_SCALAR >* aContent);

    /**
     * @brief Copy constructor.
     *
     * The newly created Tensor share the variables and the values
     * from src, but no instantiation is associated to it. It allows to force
     * the chosen implementation and to copy the data from src.
     *
     * @param aContent The implementation to use in this Tensor.
     * @param src The MultiDimContainer to copy.
     */
    Tensor(MultiDimImplementation< GUM_SCALAR >*  aContent,
           const MultiDimContainer< GUM_SCALAR >& src);
    /**
     * @brief Copy constructor & assignment
     */
    Tensor(const Tensor< GUM_SCALAR >& src);
    Tensor< GUM_SCALAR >& operator=(const Tensor< GUM_SCALAR >& src);

    /**
     * move constructor & assignement
     **/
    Tensor(Tensor< GUM_SCALAR >&& from);
    Tensor< GUM_SCALAR >& operator=(Tensor< GUM_SCALAR >&& src);


    /**
     * @brief Destructor.
     */
    ~Tensor() final;

    /// @}
    // ========================================================================
    /// @name MultiDimContainer implementation
    // ========================================================================
    /// @{

    Tensor< GUM_SCALAR >* newFactory() const final;

    /// @}
    // ========================================================================
    /// @name Class operation for Tensor instances
    // ========================================================================
    ///@{
    /** generate a random Tensor with each parameter in [0,1]*/
    const Tensor< GUM_SCALAR >& random() const;

    /** generate a random Distribution in the Tensor */
    const Tensor< GUM_SCALAR >& randomDistribution() const;

    /** generate a random CPT in the Tensor */
    const Tensor< GUM_SCALAR >& randomCPT() const;

    /** add a noise in a CPT by mixing (1-alpha)this+alpha.randomCPT()
     * @warning alpha in [0,1]
     */
    const Tensor< GUM_SCALAR >& noising(GUM_SCALAR alpha) const;

    /**
     * Projection using sum as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     */
    Tensor< GUM_SCALAR > sumOut(const gum::VariableSet& del_vars) const;

    /**
     * Projection using sum as operation (and implementation-optimized
     * operations)
     * @param kept_vars is the set of vars to keep
     */
    Tensor< GUM_SCALAR > sumIn(const gum::VariableSet& kept_vars) const;

    /**
     * Projection using multiplication as operation (and
     * implementation-optimized operations)
     * @param del_vars is the set of vars to eliminate
     */
    Tensor< GUM_SCALAR > prodOut(const gum::VariableSet& del_vars) const;

    /**
     * Projection using multiplication as operation (and
     * implementation-optimized operations)
     * @param kept_vars is the set of vars to keep
     */
    Tensor< GUM_SCALAR > prodIn(const gum::VariableSet& kept_vars) const;

    /**
     * Projection using min as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     */
    Tensor< GUM_SCALAR > minOut(const gum::VariableSet& del_vars) const;

    /**
     * Projection using min as operation (and implementation-optimized
     * operations)
     * @param kept_vars is the set of vars to keep
     */
    Tensor< GUM_SCALAR > minIn(const gum::VariableSet& kept_vars) const;

    /**
     * Projection using max as operation (and implementation-optimized
     * operations)
     * @param del_vars is the set of vars to eliminate
     */
    Tensor< GUM_SCALAR > maxOut(const gum::VariableSet& del_vars) const;

    /**
     * Projection using max as operation (and implementation-optimized
     * operations)
     * @param kept_vars is the set of vars to keep
     */
    Tensor< GUM_SCALAR > maxIn(const gum::VariableSet& kept_vars) const;

    /**
     * create a boolean-like tensor using the predicate isNonZero
     */
    Tensor< GUM_SCALAR > isNonZeroMap() const;

    /// sum of all elements in the Tensor
    GUM_SCALAR sum() const;
    /// product of all elements in the Tensor
    GUM_SCALAR product() const;
    /// max of all elements in the Tensor
    GUM_SCALAR max() const;
    /// min of all elements in the Tensor
    GUM_SCALAR min() const;
    /// max of all non one elements in the Tensor
    /// @warning can return 1 if no other value than 1 ...
    GUM_SCALAR maxNonOne() const;
    /// min of all non zero elements in the Tensor
    /// @warning can return 0 if no other value than 0 ...
    GUM_SCALAR minNonZero() const;

    /// set of instantiation corresponding to the parameter v in the Tensor
    Set< Instantiation > findAll(GUM_SCALAR v) const;

    /// Pair of the set of instantiation corresponding to the max and this max in the Tensor
    std::pair< Set< Instantiation >, GUM_SCALAR > argmax() const;

    /// Pair of the set of instantiation corresponding to the min and this min in the Tensor
    std::pair< Set< Instantiation >, GUM_SCALAR > argmin() const;

    /** ExpectedValue computes the expectation of f over *this
     *
     * Warnings
     * --------
     * ExpectedValue assumes (and does not check) that the Tensor contains a joint distribution.
     */
    GUM_SCALAR expectedValue(std::function< GUM_SCALAR(const gum::Instantiation&) >) const;

    /// entropy of the Tensor
    GUM_SCALAR entropy() const;

    /** create a new Tensor with another order
     * @throw InvalidArgument if not all and only the vars of the tensor are
     * in vars
     */
    Tensor< GUM_SCALAR > reorganize(const std::vector< const DiscreteVariable* >& vars) const;

    /** create a new Tensor with another order
     * @throw InvalidArgument if not all and only the vars of the tensor are
     * in vars
     */
    Tensor< GUM_SCALAR > reorganize(const std::vector< std::string >& vars) const;

    /** create a new Tensor extracted from *this given a partial
     * instantiation
     */
    Tensor< GUM_SCALAR > extract(const Instantiation& inst) const;

    /** create a new Tensor with a certain variable in first
     * @throw InvalidArgument if the var is not in the tensor
     */
    Tensor< GUM_SCALAR > putFirst(const DiscreteVariable* var) const;

    /** create a new Tensor with a certain variable in first
     * @throw InvalidArgument if the var is not in the tensor
     */
    Tensor< GUM_SCALAR > putFirst(const std::string& varname) const;

    /**
     * @brief copy a Tensor data using name of variables and labels (not
     * necessarily the same variables in the same orders)
     *
     * @warning a strict control on names of variables and labels are made
     *
     * @throw InvalidArgument if the Tensor is not compatible with this
     */
    const Tensor< GUM_SCALAR >& fillWith(const Tensor< GUM_SCALAR >& src) const;

    /**
     * @brief copy a Tensor data using the sequence of names in mapSrc to find
     * the corresponding variables.
     *
     * For instance, to copy the tensor P(A,B,C) in Q(D,E,A) with the mapping
     * P.A<->Q.E, P.B<->Q.A, P.C<->Q.D (assuming that the corresponding variables
     * have the same domain size and the order of labels):
     *
     * @code
     *   Q.fillWith(P,{"C","A","B"});
     * @endcode
     *
     * @warning a strict control on names of variables and labels are made
     *
     * @throw InvalidArgument if the Tensor is not compatible with this
     * */
    const Tensor< GUM_SCALAR >& fillWith(const Tensor< GUM_SCALAR >&       src,
                                         const std::vector< std::string >& mapSrc) const;

    /**
     * @brief Automatically fills the tensor with the values in
     * v.
     *
     * @param v Vector of values.
     * @throw SizeError Raised if v size's does not matches this
     * MultiDimContainer domain size.
     */
    const Tensor< GUM_SCALAR >& fillWith(const std::vector< GUM_SCALAR >& data) const;

    /**
     * @brief Automatically fills this MultiDimContainer with the value v
     *
     * @param v contains the data.
     */
    const Tensor< GUM_SCALAR >& fillWith(const GUM_SCALAR& val) const;


    /**
     * @brief Apply abs on every element of the container
     */
    const Tensor< GUM_SCALAR >& abs() const;

    /**
     * @brief apply $x^2$ on every element of the container
     */
    const Tensor< GUM_SCALAR >& sq() const;

    /**
     * @brief apply $log_2(x)$ on every element of the container
     */
    const Tensor< GUM_SCALAR >& log2() const;

    /**
     * @brief apply sgn(x)$ on every element of the container
     *
     * fill the tensor with -1 for a GUM_SCALAR<0,1 if >0 and 0 if ==0
     */
    const Tensor< GUM_SCALAR >& sgn() const;

    /**
     * @brief Create a new tensor and apply abs on every element of the
     * container
     */
    Tensor< GUM_SCALAR > new_abs() const;

    /**
     * @brief Create a new tensor and apply $x^2$ on every element of the
     * container
     */
    Tensor< GUM_SCALAR > new_sq() const;

    /**
     * @brief Create a new tensor and apply $log_2(x)$ on every element of the
     * container
     */
    Tensor< GUM_SCALAR > new_log2() const;

    /**
     * @brief Create a new tensor and apply sgn(x)$ on every element of the container
     *
     * fill the tensor with -1 for a GUM_SCALAR<0,1 if >0 and 0 if ==0
     */
    Tensor< GUM_SCALAR > new_sgn() const;


    /**
     * @brief normalisation of this do nothing if sum is 0
     */
    const Tensor< GUM_SCALAR >& normalize() const;

    /**
     * @brief compute KL divergence between this and p
     * Checks the compatibility and then compute KL divergence
     * @throws gum::InvalidArgument if p is not compatible with $this (dimension,
     * variables)
     * @throws gum::FatalError if a zero is found in p or this and not in the
     * other.
     */
    GUM_SCALAR KL(const Tensor< GUM_SCALAR >& p) const;

    /**
     * @brief normalisation of this as a CPT for the variable varId
     *
     * If the Tensor is empty, the argument is not used.
     *
     * @throw FatalError it some distribution sums to 0, or if varId>=nbrDim()
     */
    const Tensor< GUM_SCALAR >& normalizeAsCPT(const Idx& varId = 0) const;

    /**
     * @brief multiply (each value of) *this by v
     */
    const Tensor< GUM_SCALAR >& scale(GUM_SCALAR v) const;

    /**
     * @brief add v to (each value of) *this
     */
    const Tensor< GUM_SCALAR >& translate(GUM_SCALAR v) const;

    /**
     * @brief the function to inverse (each value of) *this
     */
    const Tensor< GUM_SCALAR >& inverse() const;

    /**
     * @brief get a value at random from a 1-D distribution
     */
    [[nodiscard]] Idx draw() const;

    /**
     * @brief compute the (approximated) footprint in memory of the tensor
     * @return the size in bytes
     */
    Size memoryFootprint() const;

    ///@}

    // ========================================================================
    /// @name Tensor algebra operators
    // ========================================================================
    ///@{
    Tensor< GUM_SCALAR >  operator+(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR >  operator+(const GUM_SCALAR& v) const;
    Tensor< GUM_SCALAR >  operator-(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR >  operator-(const GUM_SCALAR& v) const;
    Tensor< GUM_SCALAR >  operator*(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR >  operator*(const GUM_SCALAR& v) const;
    Tensor< GUM_SCALAR >  operator/(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR >  operator/(const GUM_SCALAR& v) const;
    Tensor< GUM_SCALAR >& operator+=(const Tensor< GUM_SCALAR >& r);
    Tensor< GUM_SCALAR >& operator+=(const GUM_SCALAR& v);
    Tensor< GUM_SCALAR >& operator*=(const Tensor< GUM_SCALAR >& r);
    Tensor< GUM_SCALAR >& operator*=(const GUM_SCALAR& v);
    Tensor< GUM_SCALAR >& operator-=(const Tensor< GUM_SCALAR >& r);
    Tensor< GUM_SCALAR >& operator-=(const GUM_SCALAR& v);
    Tensor< GUM_SCALAR >& operator/=(const Tensor< GUM_SCALAR >& r);
    Tensor< GUM_SCALAR >& operator/=(const GUM_SCALAR& v);

    // these operations are only defined for boolean-like Tensor (evidence/likelihood)
    [[nodiscard]] bool   isEvidence() const;
    Tensor< GUM_SCALAR > operator|(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR > operator&(const Tensor< GUM_SCALAR >& p2) const;
    Tensor< GUM_SCALAR > operator~() const;

    bool operator==(const Tensor< GUM_SCALAR >& r) const;

    Tensor< GUM_SCALAR >& operator<<(const DiscreteVariable& v);

    [[nodiscard]] std::string toString() const final;

    ///@}
    ///@{
    /// numerical evidence generator
    static Tensor< GUM_SCALAR > evEq(const DiscreteVariable& v, double val);
    static Tensor< GUM_SCALAR > evIn(const DiscreteVariable& v, double val1, double val2);
    static Tensor< GUM_SCALAR > evGt(const DiscreteVariable& v, double val);
    static Tensor< GUM_SCALAR > evLt(const DiscreteVariable& v, double val);
    ///@}

    protected:
    [[nodiscard]] gum::VariableSet complementVars_(const gum::VariableSet& del_vars) const;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class Tensor< double >;
#endif

  template <typename GUM_SCALAR>
  inline Tensor< GUM_SCALAR > log2(const Tensor< GUM_SCALAR >& arg) { return arg.new_log2(); }

  template <typename GUM_SCALAR>
  inline Tensor< GUM_SCALAR > abs(const Tensor< GUM_SCALAR >& arg) { return arg.new_abs(); }

  template <typename GUM_SCALAR>
  inline Tensor< GUM_SCALAR > sq(const Tensor< GUM_SCALAR >& arg) { return arg.new_sq(); }
} /* namespace gum */

#include <agrum/base/multidim/tensor_tpl.h>
#endif /* GUM_TENSOR_H */

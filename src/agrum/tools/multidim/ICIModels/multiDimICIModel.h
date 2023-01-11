/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Abstract base class for all multi dimensionnal Causal Independency
 * models
 *
 * Independance of Causal Influence (ICI) is a method of defining a discrete
 * distribution that can dramatically reduce the number of prior probabilities
 * necessary to define a distribution.  (see "The Noisy-Average Model for Local
 * Probability Distributions", Zagorecki, 2003) (see also "Canonical
 * Probabilistic Models for Knowledge Engineering", Diez, Druzdzel, 2007)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_ICI_MODEL_H
#define GUM_MULTI_DIM_ICI_MODEL_H

#include <agrum/tools/core/bijection.h>
#include <agrum/tools/multidim/implementations/multiDimReadOnly.h>

namespace gum {

  // ===========================================================================
  // ===                       GUM_MULTI_DIM_AGGREGATOR                      ===
  // ===========================================================================
  // clang-format off
  /**
   * @class MultiDimICIModel
   * @headerfile multiDimICIModel.h <agrum/tools/multidim/ICIModels/multiDimICIModel.h>
   * @ingroup multidim_group
   *
   * @brief abstract class for Conditional Indepency Models
   *
   * @warning The first variable is assumed to be the children. The latter are
   * the causes.
   */
  // clang-format on
  template < typename GUM_SCALAR >
  class MultiDimICIModel: public MultiDimReadOnly< GUM_SCALAR > {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimICIModel(GUM_SCALAR external_weight, GUM_SCALAR default_weight = (GUM_SCALAR)1.0);

    /**
     * Default constructor.
     */
    MultiDimICIModel(const MultiDimICIModel< GUM_SCALAR >& from);

    /**
     * @brief Copy constructor using a bijection to swap variables from source.
     *
     * @param bij First variables are new variables, seconds are in from.
     * @param from the copied instance
     */
    MultiDimICIModel(const Bijection< const DiscreteVariable*, const DiscreteVariable* >& bij,
                     const MultiDimICIModel< GUM_SCALAR >&                                from);

    /**
     * Destructor.
     */
    virtual ~MultiDimICIModel();

    /// @}
    // ============================================================================
    /// @name Accessors / Modifiers
    // ============================================================================
    /// @{

    public:
    std::string toString() const override;

    virtual void changeNotification(const gum::Instantiation&,
                                    const gum::DiscreteVariable* const,
                                    gum::Idx,
                                    gum::Idx) override{};

    virtual void setFirstNotification(const gum::Instantiation&) override{};

    virtual void setLastNotification(const gum::Instantiation&) override{};

    virtual void setIncNotification(const gum::Instantiation&) override{};

    virtual void setDecNotification(const gum::Instantiation&) override{};

    virtual void setChangeNotification(const gum::Instantiation&) override{};

    std::string toString(const gum::Instantiation* i) const override { return i->toString(); };

    /**
     * @brief Copy of a multiDimICIModel.
     *
     * This method is virtual because it should be optimized in certain
     * MultiDimContainer.
     *
     * @throw OperationNotAllowed Raised if src does not have the same domain
     * size than this MultiDimContainer.
     **/
    virtual void copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const override;

    /**
     * @return the real number of parameters used for this table. This function
     * is used for compute @see compressionRatio()
     */
    virtual Size realSize() const override { return this->nbrDim(); };

    GUM_SCALAR causalWeight(const DiscreteVariable& v) const;

    void causalWeight(const DiscreteVariable& v, GUM_SCALAR w) const;

    GUM_SCALAR externalWeight() const;

    void externalWeight(GUM_SCALAR w) const;

    /**
     * @brief returns the real name of the multiDimArray
     *
     * In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table
     * stored in extension is a "MultiDimArray", one that stores only non zero
     * elements is a "MultiDimSparseArray", and so on. These names are unique
     * for each type of implementation and is used by the system to determine
     * which is the best functions to use, say, when we wish to use operators
     * such as operator+ on two MultiDimImplementations.
     */
    virtual const std::string& name() const override;

    /// @}
    protected:
    /// \f$ p_0 \f$ in Henrion (89).
    mutable GUM_SCALAR _external_weight_;

    // ============================================================================
    /// @name causal weights
    // ============================================================================
    /// @{

    /// \f$ P(e | c_i) \f$ in Henrion (89) in a hashtable with a default_value.
    mutable GUM_SCALAR _default_weight_;

    mutable HashTable< const DiscreteVariable*, GUM_SCALAR > _causal_weights_;
    /// @}

    virtual void replace_(const DiscreteVariable* x, const DiscreteVariable* y) override;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimICIModel< double >;
#endif

} /* namespace gum */

#include <agrum/tools/multidim/ICIModels/multiDimICIModel_tpl.h>

#endif /* GUM_MULTI_DIM_ICI_MODEL_H */

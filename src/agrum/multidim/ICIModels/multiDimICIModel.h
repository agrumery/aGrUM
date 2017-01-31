/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
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
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES
 */
#ifndef GUM_MULTI_DIM_ICI_MODEL_H
#define GUM_MULTI_DIM_ICI_MODEL_H

#include <agrum/core/bijection.h>
#include <agrum/multidim/multiDimReadOnly.h>

namespace gum {

  // ===========================================================================
  // ===                       GUM_MULTI_DIM_AGGREGATOR                      ===
  // ===========================================================================
  // clang-format off
  /**
   * @class MultiDimCIModel
   * @headerfile multiDimICIModel.h <agrum/multidim/ICIModels/multiDimICIModel.h>
   * @ingroup multidim_group
   *
   * @brief abstract class for Conditional Indepency Models
   *
   * @warning The first variable is assumed to be the children. The latter are
   * the causes.
   */
  // clang-format on
  template <typename GUM_SCALAR>
  class MultiDimICIModel : public MultiDimReadOnly<GUM_SCALAR> {
    public:
    // ============================================================================
    /// @name Constructors / Destructors
    // ============================================================================
    /// @{

    /**
     * Default constructor.
     */
    MultiDimICIModel( GUM_SCALAR external_weight,
                      GUM_SCALAR default_weight = (GUM_SCALAR)1.0 );

    /**
     * Default constructor.
     */
    MultiDimICIModel( const MultiDimICIModel<GUM_SCALAR>& from );

    /**
     * @brief Copy constructor using a bijection to swap variables from source.
     *
     * @param bij First variables are new variables, seconds are in from.
     * @param from the copied instance
     */
    MultiDimICIModel(
        const Bijection<const DiscreteVariable*, const DiscreteVariable*>& bij,
        const MultiDimICIModel<GUM_SCALAR>& from );

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
    const std::string toString( void ) const override;

    // @todo : optimisation with a always up-to-date value associated to each
    // instantiation
    virtual void changeNotification( gum::Instantiation&,
                                     const gum::DiscreteVariable* const,
                                     const gum::Idx&,
                                     const gum::Idx& ) override{};

    virtual void setFirstNotification( gum::Instantiation& ) override{};

    virtual void setLastNotification( gum::Instantiation& ) override{};

    virtual void setIncNotification( gum::Instantiation& ) override{};

    virtual void setDecNotification( gum::Instantiation& ) override{};

    virtual void setChangeNotification( gum::Instantiation& ) override{};

    const std::string toString( const gum::Instantiation* i ) const override {
      return i->toString();
    };

    /**
     * @brief Copy of a multiDimICIModel.
     *
     * This method is virtual because it should be optimized in certain
     * MultiDimContainer.
     *
     * @throw OperationNotAllowed Raised if src does not have the same domain
     * size than this MultiDimContainer.
     **/
    virtual void
    copyFrom( const MultiDimContainer<GUM_SCALAR>& src ) const override;

    /**
     * @return the real number of parameters used for this table. This function
     * is used for compute @see compressionRatio()
     */
    virtual Size realSize() const override { return this->nbrDim(); };

    GUM_SCALAR causalWeight( const DiscreteVariable& v ) const;

    void causalWeight( const DiscreteVariable& v, GUM_SCALAR w ) const;

    GUM_SCALAR externalWeight() const;

    void externalWeight( GUM_SCALAR w ) const;

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
    mutable GUM_SCALAR __external_weight;

    // ============================================================================
    /// @name causal weights
    // ============================================================================
    /// @{

    /// \f$ P(e | c_i) \f$ in Henrion (89) in a hashtable with a default_value.
    mutable GUM_SCALAR __default_weight;

    mutable HashTable<const DiscreteVariable*, GUM_SCALAR> __causal_weights;
    /// @}

    virtual void _swap( const DiscreteVariable* x,
                        const DiscreteVariable* y ) override;
  };


  extern template class MultiDimICIModel<float>;
  extern template class MultiDimICIModel<double>;

} /* namespace gum */

#include <agrum/multidim/ICIModels/multiDimICIModel_tpl.h>

#endif /* GUM_MULTI_DIM_ICI_MODEL_H */

/**
 *
 *   Copyright 2005-2019 Pierre-Henri WUILLEMIN & Christophe GONZALES(_at_AMU)
 *   {prenom.nom}_at_lip6.fr
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
 * @brief Headers of AggregatorDecomposition.
 *
 * @author Gaspard DUCAMP
 */

#ifndef AGRUM_AGGREGATORDECOMPOSITION_H
#define AGRUM_AGGREGATORDECOMPOSITION_H

#include <agrum/PRM/PRM.h>
#include <ctime>

namespace gum {
  /**
   * @class AggregatorDecomposition aggregatorDecomposition.h
   *<agrum/BN/inference/tools/aggregatorDecomposition.h>
   * @brief This class is used to decompose aggregator in PRM<GUM_SCALAR>.
   */
  template < typename GUM_SCALAR >
  class AggregatorDecomposition {
    public:
    // ========================================================================
    /// @name Constructor & destructor.
    // ========================================================================
    /// @{

    /// Default constructor.
    AggregatorDecomposition();

    /// Destructor.
    virtual ~AggregatorDecomposition();

    BayesNet< GUM_SCALAR >& getDecomposedAggregator(BayesNet< GUM_SCALAR >& bn);

    /// @}
    // ========================================================================
    /// @name Getters & setters.
    // ========================================================================
    /// @{

    virtual std::string name() const;

    gum::Size getMaximumArity();

    void setMaximumArity(gum::Size arity);

    /// @}
    protected:
    // ========================================================================
    /// @name Private evidence handling methods and members.
    // ========================================================================
    /// @{
    NodeId addAggregator_(BayesNet< GUM_SCALAR >& bn,
                          std::string             aggType,
                          const DiscreteVariable& var,
                          Idx                     value);

    BayesNet< GUM_SCALAR >& decomposeAggregator_(BayesNet< GUM_SCALAR >& bn, NodeId node);

    Set< NodeId > addDepthLayer_(BayesNet< GUM_SCALAR >& bn,
                                 Set< NodeId >           nodes,
                                 NodeId                  initialAggregator,
                                 int&                    j);
    /// @}

    private:
    gum::Size _arity_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class AggregatorDecomposition< double >;
#endif

} /* namespace gum */

#include <agrum/BN/inference/tools/aggregatorDecomposition_tpl.h>

#endif   // AGRUM_AGGREGATORDECOMPOSITION_H

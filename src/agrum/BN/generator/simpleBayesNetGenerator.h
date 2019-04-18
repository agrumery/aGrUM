
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
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


/** @file
 * @brief Class for generating bayesian networks.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN and Ariele-Paolo
 *MAESANO
 */
#ifndef GUM_SIMPLE_BAYES_NET_GENERATOR_H
#define GUM_SIMPLE_BAYES_NET_GENERATOR_H

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

#include <agrum/BN/generator/IBayesNetGenerator.h>
#include <agrum/agrum.h>

namespace gum {

  /**
   * @class SimpleBayesNetGenerator simpleBayesNetGenerator.h
   *<agrum/BN/generator/simpleBayesNetGenerator.h>
   * @brief Class for generating bayesian networks.
   * @ingroup bn_generator
   *
   * This class randomly generates a bayesian network given three parameters:
   * the number of nodes and the max number of arcs and the number of maximum
   *modality for each nodes.
   * @warning  Be Careful when entering the parameters, high Values may cause
   *the
   *density of the Bayesian Network to be too high
   * resulting in the failure of most of the inference Methods.
   */
  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator = SimpleCPTGenerator >
  class SimpleBayesNetGenerator
      : public IBayesNetGenerator< GUM_SCALAR, ICPTGenerator > {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{

    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT.
     * @param nbrNodes The number of nodes imposed on the generator.
     * @param maxArcs The number of maximum arcs imposed on the generator.
     * @param maxModality Each DRV has from 2 to maxModality modalities
     */
    SimpleBayesNetGenerator(Size nbrNodes, Size maxArcs, Size maxModality = 2);

    /**
     * Destructor.
     */
    ~SimpleBayesNetGenerator() override;
    /// @}

    // ############################################################################
    /// @name BN generation methods
    // ############################################################################
    /// @{
    /**
     * function that generates a bayesian networks.
     * @param bayesNet Bayesian Network to be completed after initialisation
     * @return null but modify inputed Bayesian Network
     */
    void generateBN(BayesNet< GUM_SCALAR >& bayesNet) override;

    /// @}
  }; /* class SimpleBayesNetGenerator */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class SimpleBayesNetGenerator< double >;
#endif

} /* namespace gum */

#include <agrum/BN/generator/simpleBayesNetGenerator_tpl.h>

#endif /* GUM_SIMPLE_BAYES_NET_GENERATOR_H */

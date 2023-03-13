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


/** @file
 * @brief Class for generating Bayesian networks using MC algorithm
 * cf. [Ide and Cozman, 2002]
 *
 * @author Ariele MAESANO and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_MAX_PARENTS_MC_BAYES_NET_GENERATOR
#define GUM_MAX_PARENTS_MC_BAYES_NET_GENERATOR

#include <agrum/agrum.h>

#include <fstream>
#include <iostream>
#include <set>
#include <vector>

#include <sstream>

#ifdef HAVE_DIRENT_H
#  include <dirent.h>
#else
#endif


#include <agrum/BN/generator/MCBayesNetGenerator.h>

namespace gum {
  /**
   * @class MaxParentsMCBayesNetGenerator maxParentsMCBayesNetGenerator.h
   *<agrum/BN/generator/SimpleMCayesNetGenerator.h>
   * @brief Class for generating Bayesian networks with Markov chains.
   * @ingroup bn_generator
   *
   * This class is inherited from MCBayesNetGenerator and is an example of
   *Markov
   *Chain Bayesian network Generator that can be implemented.
   * Here a constraint is added which is the maximum number of parents that a
   *node
   *can have.
   * To create our own MCBayesNetGenerator with a specific constraint you
   *must:\n
   * 1) Create the class
   *XXXXXXMCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>.\n
   * 2) Add a variable specific to the constraint (here maxParents_). \n
   * 3) Verify in the function checkCondition_() the wanted constraint and then
   *launch
   * MCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>:: _checkConditions_()
   *to verify the default constraint.\n
   * 4) Define the getters and setters of the constraint variable.\n
   * 5) And verify in the constructor that the constraint is not in
   *contradiction
   *with the generator here maxParent must be >= 1.\n
   *
   * @warning  Be Careful when entering the parameters, high Values may cause
   *the
   *density of the Bayesian network to be too high
   * resulting in the failure of most of the inference Methods. \n
   * */
  template < typename GUM_SCALAR,
             template < typename > class ICPTGenerator = SimpleCPTGenerator,
             template < typename > class ICPTDisturber = SimpleCPTDisturber >
  class MaxParentsMCBayesNetGenerator:
      public MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber > {
    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{

    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT
     * and the SimpleCPTDisturber to tweak the CPT when the dimension of the
     * table
     * changes.
     * @param nbrNodes The number of nodes in the generated BN.
     * @param maxArcs The maximum number of Arcs.
     * @param maxModality Each DRV has from 2 to maxModality modalities
     * @param maxParents The number of maximum parents for each node imposed on
     * the
     * generator
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @param p probability for the change of the state (see \ref probability_p_q
     * "use
     * of p and q" )
     * @param q probability for the change of the state (see \ref probability_p_q
     * "use
     * of p and q" )
     */
    MaxParentsMCBayesNetGenerator(Size nbrNodes,
                                  Size maxArcs,
                                  Size maxModality = 2,
                                  Size maxParents  = 1,
                                  Idx  iteration   = NB_INIT_ITERATIONS,
                                  Idx  p           = 30,
                                  Idx  q           = 40);

    /**
     * Constructor.
     * Use by default the SimpleCPTGenerator for generating the BNs CPT
     * and the SimpleCPTDisturber to tweak the CPT when the dimension of the
     * table
     * changes.
     * @param bayesNet the IBayesNet used as reference to fill the parameters
     * nbrNodes, maxArcs and maxModality
     * @param maxParents The number of maximum parents for each node imposed on
     * the
     * generator
     * @param iteration The number of iterations wanted to repeat the algorithm
     * @param p probability for the change of the state (see \ref probability_p_q
     * "use
     * of p and q" )
     * @param q probability for the change of the state (see \ref probability_p_q
     * "use
     * of p and q" )
     */
    explicit MaxParentsMCBayesNetGenerator(BayesNet< GUM_SCALAR > bayesNet,
                                           Size                   maxParents = 2,
                                           Idx                    iteration  = NB_INIT_ITERATIONS,
                                           Idx                    p          = 30,
                                           Idx                    q          = 40);

    /**
     * Destructor.
     */
    ~MaxParentsMCBayesNetGenerator();

    /// @}

    // ############################################################################
    /// @name Getters and Setters
    // ############################################################################
    /// @{
    ///@name Getters

    /**
     * Return a constant reference to the number of maximum parents imposed on
     * the
     * Markov Chain BayesNetGenerator.
     */
    Size maxParents() const;

    ///@name Setters
    /**
     * Modifies the value of the number of maximum parents imposed on the
     * BayesNetGenerator
     */
    void setMaxParents(Size maxParents);
    /// @}

    protected:
    Size maxParents_;

    private:
    /**
     * function to holding the the specification wanted for the Bayesian markov.
     * @return boolean state that verify the conditions
     */

    bool _checkConditions_() final;
  };

#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MaxParentsMCBayesNetGenerator< double >;
#endif

} /*namespace gum*/

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator_tpl.h>
#endif   // MCBAYESNETGENERATOR

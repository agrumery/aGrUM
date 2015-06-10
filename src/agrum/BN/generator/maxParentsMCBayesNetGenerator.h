/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
/** @file
 * @brief Class for generating bayesian networks using MC algorithm
 * cf. [Ide and Cozman, 2002]
 *
 * @author Ariele MAESANO
 */

#ifndef GUM_MAX_PARENTS_MC_BAYES_NET_GENERATOR
#define GUM_MAX_PARENTS_MC_BAYES_NET_GENERATOR

#include <set>
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>

#include <dirent.h>
#include <sys/time.h>

#include <agrum/config.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/BN/generator/simpleCPTDisturber.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/variables/labelizedVariable.h>

namespace gum {
  /**
   * @class MaxParentsMCBayesNetGenerator maxParentsMCBayesNetGenerator.h
   *<agrum/BN/generator/SimpleMCayesNetGenerator.h>
   * @brief Class for generating bayesian networks with Markov chains.
   * @ingroup bn_group
   *
   * This class is inherited from MCBayesNetGenerator and is an example of Markov
   *Chain Bayesian Network Generator that can be implemented.
   * Here a constraint is added which is the maximum number of parents that a node
   *can have.
   * To create our own MCBayesNetGenerator with a specific constraint you must:\n
   * 1) Create the class
   *XXXXXXMCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>.\n
   * 2) Add a variable specific to the constraint (here _maxParents). \n
   * 3) Verify in the function _checkCondition() the wanted constraint and then
   *launch
   * MCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>::__checkConditions()
   *to verify the default constraint.\n
   * 4) Define the getters and setters of the constraint variable.\n
   * 5) And verify in the constructor that the constraint is not in contradiction
   *with the generator here maxParent must be >= 1.\n
   *
   * @warning  Be Careful when entering the parameters, high Values may cause the
   *density of the Bayesian Network to be too high
   * resulting in the failure of most of the inference Methods. \n
   * */
  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator = SimpleCPTGenerator,
            template <class> class ICPTDisturber = SimpleCPTDisturber>
  class MaxParentsMCBayesNetGenerator
      : public MCBayesNetGenerator<GUM_SCALAR, ICPTGenerator, ICPTDisturber> {

    public:
    // ############################################################################
    /// @name Constructors / Destructor
    // ############################################################################
    /// @{

    /**
    * Constructor.
    * Use by default the SimpleCPTGenerator for generating the BNs CPT
    * and the SimpleCPTDisturber to tweak the CPT when the dimension of the table
    * changes.
    * @param nbrNodes The number of nodes in the generated BN.
    * @param maxArcs The maximum number of Arcs.
    * @param maxModality Each DRV has from 2 to maxModality modalities
    * @param maxParents The number of maximum parents for each node imposed on the
    * generator
    * @param iteration The number of iterations wanted to repeat the algorithm
    * @param p probability for the change of the state (see \ref probability_p_q "use
    * of p and q" )
    * @param q probability for the change of the state (see \ref probability_p_q "use
    * of p and q" )
    */
    MaxParentsMCBayesNetGenerator(Size nbrNodes, Size maxArcs, Size maxModality = 2,
                                  Size maxParents = 1, Idx iteration = 5000,
                                  Idx p = 30, Idx q = 40);

    /**
    * Constructor.
    * Use by default the SimpleCPTGenerator for generating the BNs CPT
    * and the SimpleCPTDisturber to tweak the CPT when the dimension of the table
    * changes.
    * @param bayesNet the IBayesNet used as reference to fill the parameters
    * nbrNodes, maxArcs and maxModality
    * @param maxParents The number of maximum parents for each node imposed on the
    * generator
    * @param iteration The number of iterations wanted to repeat the algorithm
    * @param p probability for the change of the state (see \ref probability_p_q "use
    * of p and q" )
    * @param q probability for the change of the state (see \ref probability_p_q "use
    * of p and q" )
    */
    MaxParentsMCBayesNetGenerator(BayesNet<GUM_SCALAR> bayesNet, Size maxParents = 2,
                                  Idx iteration = 5000, Idx p = 30, Idx q = 40);

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
    * Return a constant reference to the number of maximum parents imposed on the
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
    Size _maxParents;

    private:
    /**
     * function to holding the the specification wanted for the Bayesian markov.
     * @return boolean state that verify the conditions
     */

    virtual bool __checkConditions();
  };

  extern template class MaxParentsMCBayesNetGenerator<float>;
  extern template class MaxParentsMCBayesNetGenerator<double>;
} /*namespace gum*/

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.tcc>
#endif // MCBAYESNETGENERATOR

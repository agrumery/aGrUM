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

#ifndef GUM_MAX_INDUCED_WIDTH_MC_BAYES_NET_GENERATOR
#define GUM_MAX_INDUCED_WIDTH_MC_BAYES_NET_GENERATOR

#include <set>
#include <vector>
#include <iostream>
#include <fstream>

#include <sstream>

#include <dirent.h>
#include <sys/time.h>


#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/defaultCPTGenerator.h>
#include <agrum/BN/generator/defaultCPTDisturber.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/variables/labelizedVariable.h>

namespace gum {
/**
 * @class MaxInducedWidthMCBayesNetGenerator maxInducedWidthBayesNetGenerator.h <agrum/BN/generator/SimpleMCayesNetGenerator.h>
 * @brief Class for generating bayesian networks with Markov chains.
 * @ingroup bn_group
 *
 * This class is inherited from MCBayesNetGenerator and is an example of Markov Chain Bayesian Network Generator that can be implemented.
 * Here a constraint is added which is the maximum number of parents that nodes can have.
 * @WARNING  Be Careful when entering the parameters, high Values may cause the density of the Bayesian Network to be too high 
 * resulting in the failure of most of the inference Methods. 
 * */
  template<typename T_DATA, template<class> class ICPTGenerator = DefaultCPTGenerator, template<class> class ICPTDisturber = DefaultCPTDisturber>
  class MaxInducedWidthMCBayesNetGenerator : public MCBayesNetGenerator<T_DATA,ICPTGenerator,ICPTDisturber>{

    public:
      
             // ############################################################################
  /// @name Constructors / Destructor
  // ############################################################################
  /// @{
  
   /**
   * Constructor.
   * Use by default the DefaultCPTGenerator for generating the BNs CPT 
   * and the DefaultCPTDisturber to tweak the CPT when the dimension of the table changes.
   * @param nbrNodes The number of nodes in the generated BN.
   * @param maxArcs The maximum number of Arcs.
   * @param maxModality Each DRV has from 2 to maxModality modalities
   * @param maxInducedWidth The number of maximum variable allow in the cliques of the junction tree of the bayesian Network.
   * @param iteration The number of iterations wanted to repeat the algorithm
   * @param p probability for the change of the state (see \ref probability_p_q "use of p and q" ) 
   * @param q probability for the change of the state (see \ref probability_p_q "use of p and q" ) 
   */
      
      MaxInducedWidthMCBayesNetGenerator( Size nbrNodes,  Size maxArcs, Size maxModality=2, Size maxInducedWidth=3, Idx iteration = 5000,  Idx p=30, Idx q=40);
     
   /**
   * Constructor.
   * Use by default the DefaultCPTGenerator for generating the BNs CPT 
   * and the DefaultCPTDisturber to tweak the CPT when the dimension of the table changes.
   * @param bayesNet the BayesNet used as reference to fill the parameters nbrNodes, maxArcs and maxModality
   * @param iteration The number of iterations wanted to repeat the algorithm
   * @param p probability for the change of the state (see \ref probability_p_q "use of p and q" ) 
   * @param q probability for the change of the state (see \ref probability_p_q "use of p and q" ) 
   */
      
      MaxInducedWidthMCBayesNetGenerator( BayesNet<T_DATA> bayesNet, Size maxInducedWidth=3, Idx iteration= 5000,  Idx p=30, Idx q=40);
  /**
   * Destructor.
   */
      
      ~MaxInducedWidthMCBayesNetGenerator();

        /// @}
      
           // ############################################################################
  /// @name Getters and Setters
  // ############################################################################
  /// @{
    ///@name Getters
      
      
             /**
       * Return a constant reference to the number of maximum parents imposed on the Markov Chain BayesNetGenerator.
       */
      Size maxlog10InducedWidth() const;
      
    ///@name Setters
      /**
      * Modifies the value of the number of maximum parents imposed on the BayesNetGenerator
      */
      void setMaxlog10InducedWidth(Size maxlog10InducedWidth);
 /// @}
      
  protected:
          Size _maxlog10InducedWidth;
      
       private:  
  /**
   * function to holding the the specification wanted for the Bayesian markov.
   * @return boolean state that verify the conditions
   */   
 
      bool __checkConditions();

// à virtualiser par la suite si on definit plusieur condition

  };
}/*namespace gum*/

#include <agrum/BN/generator/maxInducedWidthMCBayesNetGenerator.tcc>
#endif // MCBAYESNETGENERATOR

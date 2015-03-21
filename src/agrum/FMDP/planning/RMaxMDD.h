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
/**
 * @file
 * @brief Headers of the RMax planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_RMAX_MDD_H
#define GUM_RMAX_MDD_H
// =========================================================================
/*#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
#include <agrum/multidim/decisionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>*/
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/spumdd.h>
#include <agrum/FMDP/learning/spimddi.h>
// =========================================================================

namespace gum {


  /**
   * @class RMaxMDD RMaxMDD.h <agrum/FMDP/planning/RMaxMDD.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a RMax planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class RMaxMDD : public SPUMDD<GUM_SCALAR> {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

       /**
        * Default constructor
        */
        RMaxMDD ( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, SPIMDDI* spim, double minExplo, GUM_SCALAR epsilon = 0.00001 );

       /**
        * Default destructor
        */
        ~RMaxMDD();

      /// @}

      // ==========================================================================
      /// @name Planning Methods
      // ==========================================================================
      /// @{

        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        void initialize();

      /// @}


  protected:

        /// Performs a single step of value iteration
        MultiDimDecisionGraph< GUM_SCALAR >* _valueIteration();

        MultiDimDecisionGraph<GUM_SCALAR>* _evalQaction( const MultiDimDecisionGraph<GUM_SCALAR>*, Idx );

        NodeId __createRMax(NodeId currentNodeId,
                            MultiDimDecisionGraph<GUM_SCALAR>* dTemp,
                            MultiDimDecisionGraph<GUM_SCALAR>* sortie,
                            double valSi,
                            double valSinon);

  private :
        SPIMDDI* __spim;
        HashTable<Idx, MultiDimDecisionGraph<GUM_SCALAR>* > __rmaxMap;
        HashTable<Idx, MultiDimDecisionGraph<GUM_SCALAR>* > __dispatchMap;
        double __minExplo;
  };

//  extern template class RMaxMDD<float>;
  extern template class RMaxMDD<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/RMaxMDD.tcc>

#endif // GUM_RMAX_MDD_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


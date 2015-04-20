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
#ifndef GUM_RMAX_MDD_PLANER_H
#define GUM_RMAX_MDD_PLANER_H
// =========================================================================
/*#include <agrum/core/argMaxSet.h>
#include <agrum/core/functors.h>
#include <agrum/core/inline.h>
#include <agrum/core/smallobjectallocator/smallObjectAllocator.h>
// =========================================================================
#include <agrum/multidim/multiDimFunctionGraph.h>
#include <agrum/multidim/FunctionGraphUtilities/terminalNodePolicies/SetTerminalNodePolicy.h>*/
// =========================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/planning/spumdd.h>
#include <agrum/FMDP/learning/spimddi.h>
// =========================================================================

namespace gum {


  /**
   * @class RMaxMDDPlaner rmaxmddplaner.h <agrum/FMDP/planning/rmaxmddplaner.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a RMax planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class RMaxMDDPlaner : public MDDOperatorStrategy<GUM_SCALAR> {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        RMaxMDDPlaner ( FMDP<GUM_SCALAR>* fmdp, SPIMDDI* spim, double minExplo, GUM_SCALAR epsilon = 0.00001 );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        ~RMaxMDDPlaner();

      /// @}

      // ###################################################################
      /// @name Planning Methods
      // ###################################################################
      /// @{

        // ==========================================================================
        /**
         * Initializes data structure needed for making the planning
         * @warning No calling this methods before starting the first makePlaninng
         * will surely and definitely result in a crash
         */
        // ==========================================================================
        void initialize();

      /// @}

      // ###################################################################
      /// @name Value Iteration Methods
      // ###################################################################
      /// @{

  protected:

        // ==========================================================================
        /// Performs a single step of value iteration
        // ==========================================================================
        virtual MultiDimFunctionGraph< GUM_SCALAR >* _valueIteration();

        // ==========================================================================
        /// Performs the P(s'|s,a).V^{t-1}(s') part of the value itération
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _evalQaction( const MultiDimFunctionGraph<GUM_SCALAR>*, Idx );

      /// @}



      // ###################################################################
      /// @name Optimal policy extraction methods
      // ###################################################################
      /// @{

  protected:
        // ==========================================================================
        /// Perform the required tasks to extract an optimal policy
        // ==========================================================================
        virtual void _evalPolicy ();

      /// @}



      // ###################################################################
      /// @name Graph Function Operations Methods
      // ###################################################################
      /// @{

  protected:

        // ==========================================================================
        /// Maximizes between RMax and QAction
        /// @param qAction : the computed Q(s,a)
        /// @param rMax : the RMax for that action
        /// @warning given  qAction is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _rmaximize(const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                              const MultiDimFunctionGraph< GUM_SCALAR >* rMax);

        // ==========================================================================
        /// Maximizes between QAction and its dispatch
        /// @param qAction : the computed Q(s,a)
        /// @param dispatch : for the maximization xwith rmax after
        /// @warning given  qAction is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _multiply(const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                            const MultiDimFunctionGraph< GUM_SCALAR >* dispatch);

        // ==========================================================================
        /// Minimizes between d(X_i | s,a) and d(s|a)
        /// @param dxisa : d(X_i | s,a)
        /// @param dsa : d(s|a)
        /// @warning given d(s|a) is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _minimizes(const MultiDimFunctionGraph< GUM_SCALAR >* dxisa,
                                                            const MultiDimFunctionGraph< GUM_SCALAR >* dsa);


      /// @}

      // ###################################################################
      /// @name RMax Methods
      // ###################################################################
      /// @{

  private:

        // ==========================================================================
        /// Creates for each action a Rmax function to incite the visit of not enough
        /// visited states.
        // ==========================================================================
        NodeId __createRMax(NodeId currentNodeId,
                            MultiDimFunctionGraph<GUM_SCALAR>* dTemp,
                            MultiDimFunctionGraph<GUM_SCALAR>* sortie,
                            double valSi,
                            double valSinon);

      /// @}

  private :
        SPIMDDI* __spim;
        HashTable<Idx, MultiDimFunctionGraph<GUM_SCALAR>* > __rmaxMap;
        HashTable<Idx, MultiDimFunctionGraph<GUM_SCALAR>* > __dispatchMap;
        double __minExplo;
  };

//  extern template class RMaxMDDPlaner<float>;
  extern template class RMaxMDDPlaner<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/rmaxmddplaner.tcc>

#endif // GUM_RMAX_MDD_PLANER_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


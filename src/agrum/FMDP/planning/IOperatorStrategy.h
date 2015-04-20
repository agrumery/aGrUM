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
 * @brief Headers of the Operator Strategy interface.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */



// =========================================================================
#ifndef GUM_SDYNA_OPERATOR_STRATEGY_H
#define GUM_SDYNA_OPERATOR_STRATEGY_H
// =========================================================================
#include <agrum/core/types.h>
// =========================================================================
#include <agrum/FMDP/planning/actionSet.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class IOperatorStrategy IOperatorStrategy.h <agrum/FMDP/SDyna/IOperatorStrategy.h>
   * @brief Interface for manipulating Operator center
   * @ingroup fmdp_group
   *
   */
  template <typename GUM_SCALAR>
  class IOperatorStrategy {

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{
    public :

        // ==========================================================================
        /// Destructor (virtual and empty since it's an interface)
        // ==========================================================================
        virtual ~IOperatorStrategy(){}

      /// @}



      // ###################################################################
      /// @name
      // ###################################################################
      /// @{

        virtual MultiDimFunctionGraph<GUM_SCALAR, ExactTerminalNodePolicy>* getFunctionInstance() = 0;
        virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* getArgMaxFunctionInstance() = 0;
        virtual MultiDimFunctionGraph<ActionSet, SetTerminalNodePolicy>* getAggregatorInstance() = 0;

      /// @}



      // ###################################################################
      /// @name Graph Function Operations Methods
      // ###################################################################
      /// @{

    public:

        // ==========================================================================
        /// Performs a multiplication/projection on given qAction
        /// @param qAction : the computed Q(s,a)
        /// @warning given qAction is deleted, return the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                           Idx actionId,
                                                           const FMDP< GUM_SCALAR >* fmdp,
                                                           const Set<const DiscreteVariable*>& elVarSeq ) = 0;

        // ==========================================================================
        /// Maximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* maximize(const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* qAction) = 0;

        // ==========================================================================
        /// ArgMaximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* argmaximize(
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction) = 0;

        // ==========================================================================
        /// Adds reward to given function( whether a qAction or vFunction)
        /// @param reward : R(s) or R(s,a)
        /// @param function : either V(s) or Q(s,a)
        /// @warning given function is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* add(const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                                        const MultiDimFunctionGraph< GUM_SCALAR >* reward) = 0;

        // ==========================================================================
        /// Subtract current VFunction from old VFunction to see if threshold is
        /// reached or not
        /// @param old and new VFuntion
        /// @warning this time, nothing is deleted
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* oldVF) = 0;


      /// @}
  };

}
#endif // GUM_SDYNA_OPERATOR_STRATEGY_H

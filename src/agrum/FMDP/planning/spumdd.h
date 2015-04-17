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
 * @brief Headers of the SPUMDD planer class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPUMDD_H
#define GUM_SPUMDD_H
// =========================================================================
#include <agrum/FMDP/planning/IOperatorStrategy.h>
// =========================================================================

namespace gum {

  /**
   * @class SPUMDD spumdd.h <agrum/FMDP/planning/spumdd.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SPUDD planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class SPUMDD : public IOperatorStrategy<GUM_SCALAR> {

    public:

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{

       /**
        * Default constructor
        */
        SPUMDD (  );

       /**
        * Default destructor
        */
        ~SPUMDD();

      /// @}

      // ###################################################################
      /// @name Graph Function Operations Methods
      // ###################################################################
      /// @{

    protected:

        // ==========================================================================
        /// Performs a multiplication/projection on given qAction
        /// @param qAction : the computed Q(s,a)
        /// @param pxip : the transition probabilities for the xip variable
        /// @param xip : the variable we eliminate on the projection
        /// @warning given qAction is deleted, return the new one
        // ==========================================================================
        MultiDimFunctionGraph<GUM_SCALAR>* regress(const MultiDimFunctionGraph< GUM_SCALAR >* Vold,
                                                   Idx actionId,
                                                   const FMDP< GUM_SCALAR >* fmdp,
                                                   const Set<const DiscreteVariable*>& elVarSeq);

        // ==========================================================================
        /// Maximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        MultiDimFunctionGraph<GUM_SCALAR>* maximize(const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                    const MultiDimFunctionGraph< GUM_SCALAR >* qAction);

        // ==========================================================================
        /// ArgMaximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* argmaximize(
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction);

        // ==========================================================================
        /// Adds reward to given function( whether a qAction or vFunction)
        /// @param reward : R(s) or R(s,a)
        /// @param function : either V(s) or Q(s,a)
        /// @warning given function is deleted, returns the new one
        // ==========================================================================
        MultiDimFunctionGraph<GUM_SCALAR>* add(const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                               const MultiDimFunctionGraph< GUM_SCALAR >* reward);

        // ==========================================================================
        /// Subtract current VFunction from old VFunction to see if threshold is
        /// reached or not
        /// @param old and new VFuntion
        /// @warning this time, nothing is deleted
        // ==========================================================================
        MultiDimFunctionGraph<GUM_SCALAR>* subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                    const MultiDimFunctionGraph< GUM_SCALAR >* oldVF);


      /// @}

    protected :
        // ==========================================================================
        /// Indicates if whether or not given var is to be eliminated.
        /// Called by the evalQaction.
        // ==========================================================================
        INLINE bool _shouldEleminateVar( const DiscreteVariable* v, const FMDP<GUM_SCALAR>* fmdp ){
          return v==nullptr?false:fmdp->mapMainPrime().existsSecond(v);
        }

        // ==========================================================================
        /// Returns the last var in the var order for given graph function
        /// Called by the evalQaction.
        // ==========================================================================
        INLINE const DiscreteVariable* _lastVar( const MultiDimFunctionGraph< GUM_SCALAR >* function ){
          return function->variablesSequence().size() == 0 ? nullptr :
              function->variablesSequence().atPos( function->variablesSequence().size() - 1 );
        }
  };
} /* namespace gum */


#include <agrum/FMDP/planning/spumdd.tcc>

#endif // GUM_SPUMDD_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


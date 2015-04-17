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
 * @brief Headers of the SVI planer class.
 *
 * @author Jean-Christophe MAGNAN
 */

// =========================================================================
#ifndef GUM_SVI_H
#define GUM_SVI_H
// =========================================================================
#include <agrum/FMDP/planning/abstractSVI.h>
// =========================================================================

namespace gum {

  /**
   * @class SVI svi.h <agrum/FMDP/planning/svi.h>
   * @brief A class to find optimal policy for a given FMDP.
   * @ingroup fmdp_group
   *
   * Perform a SVI planning on given in parameter factored markov decision process
   *
   */
  template<typename GUM_SCALAR>
  class SVI : public AbstractSVI<GUM_SCALAR> {

    public:

      // ###################################################################
      /// @name Constructor & destructor.
      // ###################################################################
      /// @{

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        SVI ( FMDP<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.00001 );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        virtual ~SVI();

      /// @}

        // ###################################################################
        /// @name Value Iteration Methods
        // ###################################################################
        /// @{

    protected:

          // ==========================================================================
          /// Performs the P(s'|s,a).V^{t-1}(s') part of the value itération
          // ==========================================================================
          MultiDimFunctionGraph<GUM_SCALAR>* _evalQaction( const MultiDimFunctionGraph<GUM_SCALAR>*, Idx );

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
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _regress(const MultiDimFunctionGraph< GUM_SCALAR >* qAction,
                                                            const MultiDimFunctionGraph< GUM_SCALAR >* pxi,
                                                            const DiscreteVariable* xi);

        // ==========================================================================
        /// Maximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _maximize(const MultiDimFunctionGraph< GUM_SCALAR >* vFunction,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* qAction);

        // ==========================================================================
        /// ArgMaximizes between QAction and VFunction
        /// @param qAction : the computed Q(s,a)
        /// @param vFunction : the vFunction so far
        /// @warning given vFunction and qAction are deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy>* _argmaximize(
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* vFunction,
                            const MultiDimFunctionGraph< ArgMaxSet<GUM_SCALAR, Idx>, SetTerminalNodePolicy >* qAction);

        // ==========================================================================
        /// Adds reward to given function( whether a qAction or vFunction)
        /// @param reward : R(s) or R(s,a)
        /// @param function : either V(s) or Q(s,a)
        /// @warning given function is deleted, returns the new one
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _add(const MultiDimFunctionGraph< GUM_SCALAR >* function,
                                                        const MultiDimFunctionGraph< GUM_SCALAR >* reward);

        // ==========================================================================
        /// Subtract current VFunction from old VFunction to see if threshold is
        /// reached or not
        /// @param old and new VFuntion
        /// @warning this time, nothing is deleted
        // ==========================================================================
        virtual MultiDimFunctionGraph<GUM_SCALAR>* _subtract(const MultiDimFunctionGraph< GUM_SCALAR >* newVF,
                                                             const MultiDimFunctionGraph< GUM_SCALAR >* oldVF);


      /// @}
  };

  extern template class SVI<float>;
  extern template class SVI<double>;
} /* namespace gum */


#include <agrum/FMDP/planning/svi.tcc>

#endif // GUM_SVI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


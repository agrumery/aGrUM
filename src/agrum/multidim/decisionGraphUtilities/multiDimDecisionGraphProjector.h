/****************************************************************************
 *  Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES     *
 *  {prenom.nom}_at_lip6.fr                                                 *
 *                                                                          *
 *  This program is free software; you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation; either version 2 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program; if not, write to the                           *
 *  Free Software Foundation, Inc.,                                         *
 *  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                *
 ****************************************************************************/
/**
* @file
* @brief Class used to compute the projection of a decision graph
*
* @author Jean-Christophe Magnan
*/

// =======================================================
#ifndef GUM_MULTI_DIM_DECISION_GRAPH_PROJECTOR_H
#define GUM_MULTI_DIM_DECISION_GRAPH_PROJECTOR_H
// =======================================================
#include <agrum/multidim/multiDimDecisionGraph.h>
// =======================================================

namespace gum {

/**
 * @class MultiDimDecisionGraphProjector multiDimDecisionGraphProjector.h <agrum/multidim/patterns/multiDimDecisionGraphProjector.h>
 * @brief Class used to perform Decision Graph projections
 * @ingroup multidim_group
 *
 */

  template <typename GUM_SCALAR, template <typename> class FUNCTOR >
  class MultiDimDecisionGraphProjector
  {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

      // ============================================================================
      /// Default constructor.
      // ============================================================================
      MultiDimDecisionGraphProjector( const MultiDimDecisionGraph<GUM_SCALAR>* src,
                                      const Set<const DiscreteVariable*>& delVars,
                                      const GUM_SCALAR neutral );

      // ============================================================================
      /// Default destructor.
      // ============================================================================
      ~MultiDimDecisionGraphProjector();

    /// @}

    // ############################################################################
    /// @name Main Method
    // ############################################################################
    /// @{

      // ============================================================================
      /// Computes and builds the Decision Graph that is the result of the Projection
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR>* project();

//      void __maker(Set<const DiscreteVariable*> remainingVar, Sequence<const DiscreteVariable*> seq, std::string tab);
//      MultiDimDecisionGraph<GUM_SCALAR>* __project(Sequence<const DiscreteVariable*> seq);

    /// @}


    private :

      // ============================================================================
      /// One of the two decision graphs used for the Projection
      // ============================================================================
      const MultiDimDecisionGraph<GUM_SCALAR>* __src;

      // ============================================================================
      /// The list of variables on which the projection is performed
      // ============================================================================
      const Set<const DiscreteVariable*>& __delVars;

      // ============================================================================
      /// The resulting decision graph
      // ============================================================================
      MultiDimDecisionGraph<GUM_SCALAR>* __rd;

      // ============================================================================
      /// The function to be performed on the leaves
      // ============================================================================
      const FUNCTOR<GUM_SCALAR> __function;

      // ============================================================================
      /// The function to be performed on the leaves
      // ============================================================================
      const GUM_SCALAR __neutral;

  };

} // namespace gum

#include <agrum/multidim/decisionGraphUtilities/multiDimDecisionGraphProjector.tcc>

#endif // GUM_MULTI_DIM_DECISION_GRAPH_PROJECTOR_H

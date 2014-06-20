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
 * @brief Headers of the SPIMDDI class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_SPIMDDI_H
#define GUM_SPIMDDI_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/spumdd.h>
#include <agrum/FMDP/learning/FMDPLearner.h>
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/multidim/instantiation.h>
#include <agrum/multidim/multiDimDecisionGraph.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class SPIMDDI spimddi.h <agrum/FMDP/learning/spimddi.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template<typename GUM_SCALAR>
  class SPIMDDI {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        SPIMDDI (GUM_SCALAR discountFactor, double epsilon, double learningThreshold, Idx nbStep);

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~SPIMDDI ();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void addAction( Idx actionId, std::string actionName );
        void addVariable( const DiscreteVariable* var );

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void feedback( const Instantiation*, Idx );

        // ###################################################################
        ///
        // ###################################################################
        Idx askForAction();

      /// @}

    private :

      /// The learnt Markovian Decision Process
      FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;

      /// The model learner.
      /// It will handle by itself the different decision graph learner
      /// needed to learn the FMDP
      FMDPLearner<GUM_SCALAR>* __learner;

      /// The planer we use to find the optimal policy
      SPUMDD<GUM_SCALAR>* __planer;

      /// The optimal policy to use to make some decision
      MultiDimDecisionGraph<Idx>* __optimalPolicy;

      Instantiation __lastState;

  };

  extern template class SPIMDDI<float>;
  extern template class SPIMDDI<double>;

} /* namespace gum */


#include <agrum/FMDP/learning/spimddi.tcc>

#endif // GUM_SPIMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


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

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_SPIMDDI_H
#define GUM_SPIMDDI_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/planning/SPUDDPlanning.h>
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/learning/decision graph/IMDDI.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class SPIMDDI SPIMDDI.h <agrum/FMDP/learning/SPIMDDI.h>
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
        SPIMDDI ();

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
        addVariable(const DiscreteVariable*);

        addAction(std::string);

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        addObservation(Idx, const Observation*);

      /// @}

    private :

      ///
      FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;

      SPUDDPlanning<GUM_SCALAR>* __planer;

      HashTable<Idx, List<IMDDI<GUM_SCALAR>*>*> __actionLearners;
      IMDDI<GUM_SCALAR>* __rewardLearner;

      Set<const DiscreteVariable*> __primedVariables;
      Set<const DiscreteVariable*> __mainVariables;

  };

} /* namespace gum */


#include <agrum/FMDP/learning/SPIMDDI.tcc>

#endif // GUM_SPIMDDI_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


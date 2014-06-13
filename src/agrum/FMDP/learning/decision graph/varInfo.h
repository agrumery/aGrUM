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
 * @brief Headers of the VarInfo class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_VARIABLE_INFORMATION_H
#define GUM_VARIABLE_INFORMATION_H
// =========================================================================
#include <cmath>
// =========================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/set.h>
// =========================================================================
#include <agrum/FMDP/learning/observation.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class VarInfo VarInfo.h <agrum/FMDP/learning/varInfo.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template<typename GUM_SCALAR>
  class VarInfo {

    public:

      // ##########################################################################
      /// @name Constructor & destructor.
      // ##########################################################################
      /// @{

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        VarInfo ( const DiscreteVariable*, const DiscreteVariable* );
        VarInfo ( const DiscreteVariable*, const DiscreteVariable*, const Set<const Observation<GUM_SCALAR>*>* );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        ~VarInfo();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ==========================================================================
        /// Returns the pValue for this attribute
        // ==========================================================================
        void addObservation( const Observation<GUM_SCALAR>* );

        // ==========================================================================
        /// Returns the pValue for this attribute
        // ==========================================================================
        double pValue ( ) { return /* TO DO */__GStat;}

        const Set<const Observation<GUM_SCALAR>*>* observationSet(Idx modality){ return __modality2Observations[modality];}

      /// @}

    private :

      double __GVal(Idx, Idx);

      /// Table giving for every variables its instantiation
      HashTable<Idx, Set<const Observation<GUM_SCALAR>*>*> __modality2Observations;

      ///
      Idx __nbObservation;

      /**
       * The contingency table used to compute the GStat
       * Left Idx is for the attribute
       * Right Idx for the value
       *
       * NB: This is a silly and in a hurry implementation of contingency table
       * If someone ever use this class and has time to correctly implements
       * a efficient contingency table, you're welcome
       */
      HashTable<Idx, HashTable<Idx, Idx>*> __contingencyTable;
      HashTable<Idx, Idx> __attrMarginalTable;
      HashTable<Idx, Idx> __valueMarginalTable;

      /// The attribute using this class
      const DiscreteVariable* __attr;

      /// The value used to compute pValue
      const DiscreteVariable* __value;

      /// The Gstat
      double __GStat;
  };

} /* namespace gum */


#include <agrum/FMDP/learning/decision graph/varInfo.tcc>

#endif // GUM_VARIABLE_INFORMATION_H

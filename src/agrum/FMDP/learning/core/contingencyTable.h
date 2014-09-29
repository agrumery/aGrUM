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
 * @brief Headers of the ContingencyTable class.
 *
 * @author Jean-Christophe MAGNAN
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
   * @class ContingencyTable.h <agrum/FMDP/learning/core/contingencyTable.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

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
        void addObservation( const Observation* );

        // ==========================================================================
        /// Used to know if enough information are stored on this node
        /// to make pvalue relevant
        // ==========================================================================
        INLINE bool isPValueRelevant() { return __isRelevant;}

        // ==========================================================================
        /// Returns the pValue for this attribute
        // ==========================================================================
        INLINE double pValue ( ) { return __pvalue;}

        INLINE const Set<const Observation*>* observationSet(Idx modality){ return __modality2Observations[modality];}

      /// @}

  private :

      void __checkRelevance();
      void __computeG();


      /// The attribute using this class
      const DiscreteVariable* __attr;

      /// The value used to compute pValue
      const DiscreteVariable* __value;

      /// Table giving for every variables its instantiation
      HashTable<Idx, Set<const Observation*>*> __modality2Observations;

      /// The number of observation taken into account
      /// Used for the GStat
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

      /// The Gstat,
      /// the degree of freedom used for the computation of the p-value
      /// andn the p-value
      double __GStat;
      Idx __degreeOfFreedom;
      double __pvalue;

      /// Boolean to indicates whether p-value is relevant or not
      bool __isRelevant;
//      std::vector<bool> __areRelevant;
  };

} /* namespace gum */

#endif // GUM_VARIABLE_INFORMATION_H

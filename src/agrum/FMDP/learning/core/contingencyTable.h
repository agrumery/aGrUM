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

  class ContingencyTable {

    public:

      // ##########################################################################
      /// @name Constructor & destructor.
      // ##########################################################################
      /// @{

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        ContingencyTable ( const DiscreteVariable*, const DiscreteVariable* );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        ~ContingencyTable();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ==========================================================================
        /// Increments the number of sample for case( iattr, ivalue )
        // ==========================================================================
        void add( Idx iattr, Idx ivalue);

        // ==========================================================================
        /// Returns the number of samples for case (iattr, ivalue)
        // ==========================================================================
        Idx joint( Idx iattr, Idx ivalue){ return (*__contingencyTable[iattr])[ivalue]; }

        // ==========================================================================
        /// Returns the number of samples for line iattr
        // ==========================================================================
        Idx aMarginal( Idx iattr ) { return __attrMarginalTable[iattr]; }

        // ==========================================================================
        /// Returns the number of samples for column ivalue
        // ==========================================================================
        Idx vMarginal( Idx ivalue ) { return __valueMarginalTable[ivalue]; }

        // ==========================================================================
        /// Returns the number of samples for line iattr
        // ==========================================================================
        Idx attrSize() { return __attrDomainSize; }

        // ==========================================================================
        /// Returns the number of samples for column ivalue
        // ==========================================================================
        Idx valueSize() { return __valueDomainSize; }

      /// @}

  private :

      /// The attribute using this class
      Idx __attrDomainSize;

      /// The value used to compute pValue
      Idx __valueDomainSize;

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

      Idx __nbObservation;
  };

} /* namespace gum */

#endif // GUM_VARIABLE_INFORMATION_H

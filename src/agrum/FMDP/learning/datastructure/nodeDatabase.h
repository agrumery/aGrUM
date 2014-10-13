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
 * @brief Headers of the NodeDatabase class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

// =========================================================================
#ifndef GUM_NODE_DATABASE_H
#define GUM_NODE_DATABASE_H
// =========================================================================
#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
// =========================================================================
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/learning/decisionGraph/varInfo.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class NodeDatabase NodeDatabase.h <agrum/FMDP/learning/decisionGraph/nodeDatabase.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  class NodeDatabase {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        NodeDatabase (const Set<const DiscreteVariable*>*, const DiscreteVariable*);
        NodeDatabase (const Set<const DiscreteVariable*>*, const DiscreteVariable*, const Set<const Observation*>*);

        // ###################################################################
        /// Default destructor
        // ###################################################################
        ~NodeDatabase();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ###################################################################
        ///
        // ###################################################################
        void addObservation( const Observation* );

        // ###################################################################
        ///
        // ###################################################################
        HashTable<Idx, NodeDatabase*> splitOnVar(const DiscreteVariable*);

        // ###################################################################
        ///
        // ###################################################################
        INLINE bool isTestRelevant( const DiscreteVariable* var ) const { return __attrTable[var]->isTestRelevant(); }
        INLINE double testValue( const DiscreteVariable* var ) const { return __attrTable[var]->testValue(); }

        // ###################################################################
        ///
        // ###################################################################
        INLINE Idx nbObservation(){return __nbObservation;}

        // ###################################################################
        ///
        // ###################################################################
        double* effectif();

      /// @}

    private :

      /// Table giving for every variables its instantiation
      HashTable<const DiscreteVariable*, VarInfo*> __attrTable;

      /// A reference to this set of variable is only kept for fast and efficient split
      const Set<const DiscreteVariable*>* __attrSet;
      /// So does this reference on the value observed
      const DiscreteVariable* __value;

      ///
      Idx __nbObservation;
      HashTable<Idx,Idx> __valueCount;
  };


} /* namespace gum */


#endif // GUM_NODE_DATABASE_H


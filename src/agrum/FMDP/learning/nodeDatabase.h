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

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_NODE_DATABASE_H
#define GUM_NODE_DATABASE_H
// =========================================================================
#include <agrum/core/hashTable.h>
// =========================================================================
#include <agrum/FMDP/learning/observation.h>
#include <agrum/FMDP/learning/varInfo.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class NodeDatabase NodeDatabase.h <agrum/FMDP/learning/nodeDatabase.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */

  template<typename GUM_SCALAR>
  class NodeDatabase {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

        // ###################################################################
        /// Default constructor
        // ###################################################################
        NodeDatabase ();

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
        void addObservation( const Observation<GUM_SCALAR>* );

        // ###################################################################
        ///
        // ###################################################################
        double pValue( const DiscreteVariable* var) const { return __varInst[var]->pValue(); }

      /// @}

    private :

      /// Table giving for every variables its instantiation
      HashTable<const DiscreteVariable*, VarInfo*> __varInst;
  };

  extern template class NodeDatabase<float>;
  extern template class NodeDatabase<double>;
} /* namespace gum */


#include <agrum/FMDP/learning/varInfo.tcc>

#endif // GUM_NODE_DATABASE_H


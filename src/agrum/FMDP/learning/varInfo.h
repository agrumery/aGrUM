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

// #define  TRACE_ALGO
// =========================================================================
#ifndef GUM_VARIABLE_INFORMATION_H
#define GUM_VARIABLE_INFORMATION_H
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

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

      /**
      * Default constructor
      */
      VarInfo ( const DiscreteVariable*);

      /**
      * Default destructor
      */
      ~VarInfo();

      /// @}

      // ==========================================================================
      /// @name C
      // ==========================================================================
      /// @{

      /**
      *
      */
      double pValue ( );

      /// @}

    private :

      /// Table giving for every variables its instantiation
      HashTable<Idx, Set<Observation<GUM_SCALAR>*>> __modality2Observations;

      /// An eventual observed value for the instantiation
      HashTable<Idx, double> __modality2GStat;
  };

  extern template class VarInfo<float>;
  extern template class VarInfo<double>;
} /* namespace gum */


#include <agrum/FMDP/learning/varInfo.tcc>

#endif // GUM_VARIABLE_INFORMATION_H

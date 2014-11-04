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
#include <agrum/FMDP/learning/core/testPolicy/GTestPolicy.h>
//#include <agrum/FMDP/learning/core/contingencyTable.h>
// =========================================================================
#include <agrum/variables/discreteVariable.h>
// =========================================================================

namespace gum {

  /**
   * @class VarInfo varInfo.h <agrum/FMDP/learning/decisionGraph/varInfo.h>
   * @brief
   * @ingroup fmdp_group
   *
   *
   *
   */
  template < typename GUM_SCALAR, template <class> class TestPolicy = GTestPolicy >
  class VarInfo : public TestPolicy<GUM_SCALAR> {

    public:

      // ##########################################################################
      /// @name Constructor & destructor.
      // ##########################################################################
      /// @{

        // ==========================================================================
        /// Default constructor
        // ==========================================================================
        VarInfo ( const DiscreteVariable* );
        VarInfo ( const DiscreteVariable*, const Set<const Observation*>* );

        // ==========================================================================
        /// Default destructor
        // ==========================================================================
        virtual ~VarInfo();

      /// @}

      // ==========================================================================
      /// @name
      // ==========================================================================
      /// @{

        // ==========================================================================
        /// Returns the pValue for this attribute
        // ==========================================================================
        void addObservation( const Observation* newObs, Idx moda, GUM_SCALAR value );

        INLINE const Set<const Observation*>* observationSet( Idx modality ){
          if( !__modality2Observations.exists(modality) )
            __modality2Observations.insert(modality, new Set<const Observation*>() );
          return __modality2Observations[modality];

        }

      /// @}

  private :

      /// The attribute using this class
      const DiscreteVariable* __attr;

      /// Table giving for every variables its instantiation
      HashTable<Idx, Set<const Observation*>*> __modality2Observations;
  };

} /* namespace gum */

#include <agrum/FMDP/learning/datastructure/varInfo.tcc>

#endif // GUM_VARIABLE_INFORMATION_H

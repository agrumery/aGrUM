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
 * @brief Headers of the SPUDDInference class.
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_SPUDD_INEFERENCE_H
#define GUM_SPUDD_INEFERENCE_H

#include <string>
#include <vector>

#include <agrum/multidim/multiDimDecisionDiagramBase.h>

#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>


namespace gum {

  /**
   * @class SPUDDInference SPUDDInference.h <agrum/FMDP/inference/SPUDDInference.h>
   * @brief A factory class to ease Factored Markov Decision Process construction.
   * @ingroup fmdp_group
   * Perform a SPUDD inference on given in parameter factored markov decision process
   *
   */

  template<typename GUM_SCALAR>

  class SPUDDInference {

    public:

      // ==========================================================================
      /// @name Constructor & destructor.
      // ==========================================================================
      /// @{

      /**
       * Default constructor
       */
      SPUDDInference( FactoredMarkovDecisionProcess<GUM_SCALAR>* fmdp, GUM_SCALAR epsilon = 0.001 );

      /**
       * Default destructor
       */
      ~SPUDDInference();

      /// @}
      // ==========================================================================
      /// @name Inference Methods
      // ==========================================================================
      /// @{

      /**
       * Makes a spudd inference on FMDP
       */
      void makeInference();

      /// @}

    private:

      FactoredMarkovDecisionProcess<GUM_SCALAR>* __fmdp;
      HashTable< NodeId, List<Idx>* > __bestPolicy;
      GUM_SCALAR __epsilon;

  };
} /* namespace gum */


#include <agrum/FMDP/inference/SPUDDInference.tcc>

#endif // GUM_SPUDD_INEFERENCE_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;


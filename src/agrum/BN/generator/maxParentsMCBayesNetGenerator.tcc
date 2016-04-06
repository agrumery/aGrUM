/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN & Ariele-Paolo MAESANO   *
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
/** @file
 * @brief Source implementation of MaxParentsMCBG
 *
 * @author Pierre-Henri WUILLEMIN and Ariele Maesano
 *
 */

#include <agrum/BN/generator/maxParentsMCBayesNetGenerator.h>

namespace gum {

#define MCBG MCBayesNetGenerator<GUM_SCALAR,ICPTGenerator,ICPTDisturber>

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  INLINE
  MaxParentsMCBG<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::
      MaxParentsMCBG( Size nbrNodes,
                                     Size maxArcs,
                                     Size max_modality,
                                     Size maxParents,
                                     Idx iteration,
                                     Idx p,
                                     Idx q )
      : MCBG(nbrNodes, maxArcs, max_modality, iteration, p, q ) {
    if ( maxParents == 0 )
      GUM_ERROR(
          OperationNotAllowed,
          "maxParents must be at least equal to 1 to have a connexe graph" );

    _maxParents = maxParents;
    GUM_CONSTRUCTOR( MaxParentsMCBG );
  }

  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  INLINE
  MaxParentsMCBG<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::
      MaxParentsMCBG( BayesNet<GUM_SCALAR> bayesNet,
                                     Size maxParents,
                                     Idx iteration,
                                     Idx p,
                                     Idx q )
      : MCBG(bayesNet, iteration, p, q ) {
    _maxParents = maxParents;
    GUM_CONSTRUCTOR( MaxParentsMCBG );
  }

  // Destructor.
  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  INLINE
      MaxParentsMCBG<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::
          ~MaxParentsMCBG() {
    GUM_DESTRUCTOR( MaxParentsMCBG );
  }

  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  bool MaxParentsMCBG<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::__checkConditions() {
    for ( auto node : this->_bayesNet.nodes() )
      if ( this->_bayesNet.dag().parents( node ).size() > _maxParents )
        return false;

    return MCBG::__checkConditions();
  }

  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  INLINE Size MaxParentsMCBG<GUM_SCALAR,
                                            ICPTGenerator,
                                            ICPTDisturber>::maxParents() const {
    return _maxParents;
  }
  template <typename GUM_SCALAR,
            template <class> class ICPTGenerator,
            template <class> class ICPTDisturber>
  INLINE void
  MaxParentsMCBG<GUM_SCALAR, ICPTGenerator, ICPTDisturber>::
      setMaxParents( Size maxParents ) {
    if ( maxParents == 0 )
      GUM_ERROR(
          OperationNotAllowed,
          "maxParents must be at least equal to 1 to have a connexe graph" );

    _maxParents = maxParents;
  }
} /* namespace gum */

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
/** @file
 * @brief the class for all the scores that are like BIC, etc, whose
 * formula is asymmetric w.r.t. the nodes contained in the formula.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


namespace gum {

  
  namespace learning {

    
    /// a function that determines the db single ids needed for the score
    ALWAYS_INLINE void AsymmetricScore::_computeInducedSingleIds
    ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids ) {
      _db_induced_ids.resize ( db_pair_ids.size () );
      for (unsigned int i = 0; i < db_pair_ids.size (); ++i ) {
        _db_induced_ids[i] = db_pair_ids[i].first;
      }
    }

    
    /// computes the "unconditional" counting tree for a set of single targets
    ALWAYS_INLINE void
    AsymmetricScore::_createTree
    ( const std::vector<unsigned int>& db_single_ids ) {
      _tree.setNodes ( __empty_cond_set, db_single_ids, __empty_pair_set );
    }

    
    /// compute the counting tree of the set of targets conditioned on some nodes
    ALWAYS_INLINE void
    AsymmetricScore::_createTree
    ( const std::vector<unsigned int>& db_conditioning_ids,
      const std::vector<unsigned int>& db_single_ids ) {
      _tree.setNodes ( db_conditioning_ids, db_single_ids, __empty_pair_set );
    }

    
    /// computes the "unconditional" counting tree of a set of pairs of targets
    ALWAYS_INLINE void
    AsymmetricScore::_createTree
    ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids ) {
      _computeInducedSingleIds ( db_pair_ids );
      _tree.setNodes ( __empty_cond_set, _db_induced_ids, db_pair_ids );
    }
      

    /// compute the counting tree of the set of targets conditioned on some nodes
    ALWAYS_INLINE void
    AsymmetricScore::_createTree
    ( const std::vector<unsigned int>& db_conditioning_ids,
      const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids ) {
      _computeInducedSingleIds ( db_pair_ids );
      _tree.setNodes ( db_conditioning_ids, _db_induced_ids, db_pair_ids );
    }
      

  } /* namespace learning */
  
  
} /* namespace gum */


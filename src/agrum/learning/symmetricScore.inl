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
 * @brief the class for all the scores whose formula is symmetric w.r.t. the
 * nodes it contains.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


namespace gum {

  
  namespace learning {

    
    /// a function that determines the db single ids needed for the score
    ALWAYS_INLINE void SymmetricScore::_computeInducedSingleIds
    ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids ) {
      HashTable<unsigned int, bool> exist_ids;
      _db_induced_ids.clear ();
      for (unsigned int i = 0; i < db_pair_ids.size (); ++i ) {
        if ( ! exist_ids.exists ( db_pair_ids[i].first ) ) {
          exist_ids.insert ( db_pair_ids[i].first, true );
          _db_induced_ids.push_back ( db_pair_ids[i].first );
        }
        if ( ! exist_ids ( db_pair_ids[i].second ) ) {
          exist_ids.insert ( db_pair_ids[i].second, true );
          _db_induced_ids.push_back ( db_pair_ids[i].second );
        }
      }
    }
    

  } /* namespace learning */
  
  
} /* namespace gum */


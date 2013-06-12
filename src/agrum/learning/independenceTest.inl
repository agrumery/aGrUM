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
 * @brief the abstract class for all the independence tests
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


namespace gum {

  
  namespace learning {

    
    /// modifies the max size of the counting trees
    ALWAYS_INLINE void IndependenceTest::setMaxSize ( unsigned int new_size ) {
      Score::setMaxSize ( new_size );
    }
 

    /// computes the "unconditional" scores of a set of pairs of targets
    ALWAYS_INLINE void IndependenceTest::computeScores
    ( const std::vector< std::pair<unsigned int,
                                   unsigned int> >& db_pair_ids ) {
      Score::computeScores ( db_pair_ids );
    }

    
    /// compute the scores of the set of targets conditioned on some nodes
    ALWAYS_INLINE void IndependenceTest::computeScores
    ( const std::vector<unsigned int>& db_conditioning_ids,
      const std::vector< std::pair<unsigned int,
                                   unsigned int> >& db_pair_ids ) {
      Score::computeScores ( db_pair_ids );
    }


    /// returns the score of a given pair X,Y given the conditioning nodes
    ALWAYS_INLINE float IndependenceTest::score
    ( const std::pair<unsigned int,unsigned int>& XY_pair ) const {
      return Score::score ( XY_pair );
    }


  } /* namespace learning */
  
  
} /* namespace gum */

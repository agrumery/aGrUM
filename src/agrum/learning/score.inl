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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// returns the log in base 2
    ALWAYS_INLINE float Score::_logf ( unsigned int x ) const {
      if (x < __has_logf.size () ) {
        if ( __has_logf[x] ) {
          return __logf[x];
        }
        else {
          __has_logf[x] = true;
          __logf[x] = logf ( x );
          return __logf[x];
        }
      }
      else {
        return logf ( x );
      }
    }
      
    
    /// modifies the max size of the counting trees
    ALWAYS_INLINE void Score::setMaxSize ( unsigned int new_size ) {
      // mostly, the memory used by CountingTree boxes is for storing
      // counts, i.e., they are unsigned ints
      __max_tree_size = ceilf ( (float ) new_size / sizeof (unsigned int) );
    }

    
    /// computes the "unconditional" scores of a set of single targets
    ALWAYS_INLINE void
    Score::computeScores ( const std::vector<unsigned int>& db_single_ids ) {
      // set the conditioning nodes to the empty set
      _db_conditioning_ids = &__empty_cond_set;

      // if there is no size limitation, do not try to cut db_single_ids into
      // pieces and perform computations directly
      if ( ! __max_tree_size ) {
        // create the tree
        _tree.setNodes ( __empty_cond_set, db_single_ids, __empty_pair_set );

        // compute the scores and store them into _single_scores
        _single_scores.clear ();
        _computeScores ( db_single_ids );
        return;
      }
      
      // here there are size limitations
      // create a vector to store the ids of the nodes we wish to compute. Due
      // to memory size limitations, we may not be able to compute the scores
      // of all the ids in one pass
      std::vector<unsigned int> ids;

      _single_scores.clear ();

      unsigned int current_index = 0;
      while ( true ) {
        // determine up to which element we can compute scores before reaching
        // the max size of the tree
        unsigned int current_size = 0;
        unsigned int i = 0;
        ids.resize ( db_single_ids.size () );
        for ( ; current_index < db_single_ids.size (); ++current_index, ++i ) {
          current_size +=
            _database->nbrModalities ( db_single_ids[current_index] );
          ids[i] = db_single_ids[current_index];
          if ( current_size >= __max_tree_size ) {
            ++current_index;
            ++i;
            break;
          }
        }
        if ( !i ) break;
        ids.resize ( i );
        
        // create the tree and perform the computations
        _tree.setNodes ( __empty_cond_set, ids, __empty_pair_set );
        _computeScores ( ids );
      }
    }


    /// compute the scores of the set of targets conditioned on some nodes
    ALWAYS_INLINE void
    Score::computeScores ( const std::vector<unsigned int>& db_conditioning_ids,
                           const std::vector<unsigned int>& db_single_ids ) {
      // keep track of the conditioning nodes
      _db_conditioning_ids = &db_conditioning_ids;

      // if there is no size limitation, do not try to cut db_single_ids into
      // pieces and perform computations directly
      if ( ! __max_tree_size ) {
        // create the tree
        _tree.setNodes ( db_conditioning_ids, db_single_ids, __empty_pair_set );

        // compute the scores and store them into _single_scores
        _single_scores.clear ();
        _computeScores ( db_single_ids );
        return;
      }
      
      // here there are size limitations
      // create a vector to store the ids of the nodes we wish to compute. Due
      // to size limitations in memory, we may not be able to compute
      // the scores of all the ids in one pass
      std::vector<unsigned int> ids;

      _single_scores.clear ();

      // compute the size of the conditioning set
      unsigned int size = 0;
      unsigned int cond_size = 1;
      for ( unsigned int i = 0; i < db_conditioning_ids.size (); ++i ) {
        cond_size *= _database->nbrModalities ( db_conditioning_ids[i] );
        size += cond_size;
      }

      bool first_time = true;
      unsigned int current_index = 0;
      while ( true ) {
        // determine up to which element we can compute scores before reaching
        // the max size of the tree
        unsigned int current_size = size;
        unsigned int i = 0;
        ids.resize ( db_single_ids.size () );
        for ( ; current_index < db_single_ids.size (); ++current_index, ++i ) {
          current_size +=
            cond_size * _database->nbrModalities ( db_single_ids[current_index] );
          ids[i] = db_single_ids[current_index];
          if ( current_size >= __max_tree_size ) {
            ++current_index;
            ++i;
            break;
          }
        }
        if ( !i ) break;
        ids.resize ( i );
        
        // create the tree and perform the computations
        if ( first_time ) {
          first_time = false;
          _tree.setNodes ( db_conditioning_ids, ids, __empty_pair_set );
        }
        else {
          _tree.setTargetNodes ( ids );
        }
        _computeScores ( ids );
      }
    }


    /// computes the "unconditional" scores of a set of pairs of targets
    ALWAYS_INLINE void
    Score::computeScores
    ( const std::vector< std::pair<unsigned int, unsigned int> >& db_pair_ids ) {
      // set the conditioning nodes to the empty set
      _db_conditioning_ids = &__empty_cond_set;

      // if there is no size limitation, do not try to cut db_pair_ids into
      // pieces and perform computations directly
      if ( ! __max_tree_size ) {
        // compute the single targets that are needed for computing the scores
        // of the target pairs
        _computeInducedSingleIds ( db_pair_ids );

        // create the tree
        _tree.setNodes ( __empty_cond_set, _db_induced_ids, db_pair_ids );
        
        // compute the scores and store them into _pair_scores
        _pair_scores.clear ();
        _computeScores ( db_pair_ids );
        return;
      }
      
      // here there are size limitations
      // create a vector to store the ids of the nodes we wish to compute. Due
      // to memory size limitations, we may not be able to compute the scores
      // of all the ids in one pass
      std::vector< std::pair<unsigned int,unsigned int> > ids;

      _pair_scores.clear ();

      unsigned int current_index = 0;
      while ( true ) {
        // determine up to which element we can compute scores before reaching
        // the max size of the tree
        unsigned int current_size = 0;
        unsigned int i = 0;
        ids.clear ();
        for ( ; current_index < db_pair_ids.size (); ++current_index, ++i ) {
          current_size +=
            _database->nbrModalities ( db_pair_ids[current_index].first ) *
            _database->nbrModalities ( db_pair_ids[current_index].second );
          ids.push_back ( db_pair_ids[current_index] );
          if ( current_size >= __max_tree_size ) {
            ++current_index;
            ++i;
            break;
          }
        }
        if ( !i ) break;

        // compute the single targets that are needed for computing the scores
        // of the target pairs
        _computeInducedSingleIds ( ids );
       
        // create the tree and perform the computations
        _tree.setNodes ( __empty_cond_set, _db_induced_ids, ids );
        _computeScores ( ids );
      }
    }


    /// compute the scores of the set of targets conditioned on some nodes
    ALWAYS_INLINE void Score::computeScores
    ( const std::vector<unsigned int>& db_conditioning_ids,
      const std::vector< std::pair<unsigned int,
                                   unsigned int> >& db_pair_ids ) {
      // keep track of the conditioning nodes
      _db_conditioning_ids = &db_conditioning_ids;

      // if there is no size limitation, do not try to cut db_pair_ids into
      // pieces and perform computations directly
      if ( ! __max_tree_size ) {
        // compute the single targets that are needed for computing the scores
        // of the target pairs
        _computeInducedSingleIds ( db_pair_ids );

        // create the tree
        _tree.setNodes ( db_conditioning_ids, _db_induced_ids, db_pair_ids );
        
        // compute the scores and store them into _pair_scores
        _pair_scores.clear ();
        _computeScores ( db_pair_ids );
        return;
      }
      
      // here there are size limitations
      // create a vector to store the ids of the nodes we wish to compute. Due
      // to memory size limitations, we may not be able to compute the scores
      // of all the ids in one pass
      std::vector< std::pair<unsigned int,unsigned int> > ids;

      _pair_scores.clear ();

      // compute the size of the conditioning set
      unsigned int size = 0;
      unsigned int cond_size = 1;
      for ( unsigned int i = 0; i < db_conditioning_ids.size (); ++i ) {
        cond_size *= _database->nbrModalities ( db_conditioning_ids[i] );
        size += cond_size;
      }

      bool first_time = true;
      unsigned int current_index = 0;
      while ( true ) {
        // determine up to which element we can compute scores before reaching
        // the max size of the tree
        unsigned int current_size = size;
        unsigned int i = 0;
        ids.clear ();
        for ( ; current_index < db_pair_ids.size (); ++current_index, ++i ) {
          current_size +=
            _database->nbrModalities ( db_pair_ids[current_index].first ) *
            _database->nbrModalities ( db_pair_ids[current_index].second ) *
            cond_size;
          ids.push_back ( db_pair_ids[current_index] );
          if ( current_size >= __max_tree_size ) {
            ++current_index;
            ++i;
            break;
          }
        }
        if ( !i ) break;

        // compute the single targets that are needed for computing the scores
        // of the target pairs
        _computeInducedSingleIds ( ids );
        
        // create the tree and perform the computations
        if ( first_time ) {
          first_time = false;
          _tree.setNodes ( db_conditioning_ids, _db_induced_ids, ids );
        }
        else {
          _tree.setTargetNodes ( _db_induced_ids, ids );
        }
        _computeScores ( ids );
      }
    }

    
    /// returns the score of a given single target Y given the conditioning nodes
    ALWAYS_INLINE float
    Score::score ( unsigned int Y ) const {
      return _single_scores[Y];
    }
    
      
    /// returns the score of a given pair X,Y given the conditioning nodes
    ALWAYS_INLINE float
    Score::score ( const std::pair<unsigned int,unsigned int>& XY_pair ) const {
      return _pair_scores[XY_pair];
    }


    /// clears all the data structures from memory
    ALWAYS_INLINE void Score::clear () {
      _tree.clear ();
      _db_induced_ids.clear ();
      _single_scores.clear ();
      _pair_scores.clear ();
    }
    
    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */


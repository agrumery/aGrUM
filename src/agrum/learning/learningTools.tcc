/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                                  *
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
 * @brief Common tools used by learning algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides common tools used by various learning algorithms.
 * */
#include <iostream>
#include <utility>
#include <agrum/config.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===            CACHE FOR CONDITIONAL INDEPENDENCE TEST VALUES            === */
  /* ============================================================================ */
  /* ============================================================================ */


  /// default constructor: creates an empty cache

  template <typename Score>
  IndepCache<Score>::IndepCache() {
    /// for debugging purposes
    GUM_CONSTRUCTOR ( IndepCache );
  }


  /// destructor

  template <typename Score>
  IndepCache<Score>::~IndepCache() {
    /// for debugging purposes
    GUM_DESTRUCTOR ( IndepCache );
  }


  /// creates the tree necessary for the insertion/update of a new test value

  template <typename Score>
  CacheNode<Score>& IndepCache<Score>::_prepareInsertVal
  ( const Id& node1, const Id& node2, const std::vector<Id>& conditional_set,
    const Score& val, bool safe_mode ) {
    // in safe mode, check that the ids in the conditional set are
    // sorted in increasing order
    if ( safe_mode ) {
      for ( unsigned int i = 1; i < conditional_set.size(); ++i )
        if ( conditional_set[i] < conditional_set[i - 1] ) {
          GUM_ERROR ( FatalError, "the conditional set is not sorted properly" );
        }
    }

    std::pair<Id, Id> edge ( std::min ( node1, node2 ), std::max ( node1, node2 ) );

    // check if the edge node1 - node2 has not a cache yet. In this case, add one

    if ( !cache.exists ( edge ) )
      cache.insert ( edge, CacheNode<Score>() );

    // now get the cache corresponding to the edge and follow the tree until all
    // the conditioning set has been parsed
    CacheNode<Score>* tree = &cache[edge];

    for ( unsigned int i = 0; i < conditional_set.size(); ++i ) {
      if ( !tree->children.exists ( conditional_set[i] ) )
        tree = & ( tree->children.insert ( conditional_set[i], CacheNode<Score>() ) );
      else
        tree = & ( tree->children[conditional_set[i]] );
    }

    return *tree;
  }


  /// adds a value into the cache

  template <typename Score>
  void IndepCache<Score>::insertTestVal
  ( const Id& node1, const Id& node2, const std::vector<Id>& conditional_set,
    const Score& val, bool safe_mode ) {
    // create the tree appropriate for storing the value of the independence test
    CacheNode<Score>& tree =
      _prepareInsertVal ( node1, node2, conditional_set, val, safe_mode );
    // check if tree is pointing to an already computed test

    if ( tree.has_score ) {
      GUM_ERROR ( DuplicateElement, "the cache contains already the score" );
    }

    tree.score = val;

    tree.has_score = true;
  }


  /// updates a value in the cache (or adds it if it does not already exist)

  template <typename Score>
  void IndepCache<Score>::setTestVal
  ( const Id& node1, const Id& node2, const std::vector<Id>& conditional_set,
    const Score& val, bool safe_mode ) {
    // create the tree appropriate for storing the value of the independence test
    CacheNode<Score>& tree =
      _prepareInsertVal ( node1, node2, conditional_set, val, safe_mode );
    // update the value of the independence test
    tree.score = val;
    tree.has_score = true;
  }


  /// returns a pointer to the value of a given independence test

  template <typename Score>
  Score* IndepCache<Score>::getPtrTestVal
  ( const Id& node1, const Id& node2,
    const std::vector<Id>& conditional_set, bool safe_mode ) const  {
    // in safe mode, check that the ids in the conditional set are
    // sorted in increasing order
    if ( safe_mode ) {
      for ( unsigned int i = 1; i < conditional_set.size(); ++i )
        if ( conditional_set[i] < conditional_set[i - 1] ) {
          GUM_ERROR ( FatalError, "the conditional set is not sorted properly" );
        }
    }

    std::pair<Id, Id> edge ( std::min ( node1, node2 ), std::max ( node1, node2 ) );

    // check if the edge node1 - node2 has not a cache yet. In this case, add one

    if ( !cache.exists ( edge ) )
      return 0;

    // now get the cache corresponding to the edge and follow the tree until all
    // the conditioning set has been parsed
    CacheNode<Score>* tree = const_cast<CacheNode<Score>*> ( &cache[edge] );

    for ( unsigned int i = 0; i < conditional_set.size(); ++i ) {
      if ( !tree->children.exists ( conditional_set[i] ) )
        return 0;

      tree = & ( tree->children[conditional_set[i]] );
    }

    if ( tree->has_score )
      return & ( tree->score );
    else
      return 0;
  }


  /// returns the value of a given test from the cache

  template <typename Score> INLINE
  Score& IndepCache<Score>::getTestVal
  ( const Id& node1, const Id& node2, const std::vector<Id>& conditional_set,
    bool safe_mode ) const {
    // get a pointer on the score
    Score* score = getPtrTestVal ( node1, node2, conditional_set, safe_mode );
    // check if the pointer is different from 0 before returning it

    if ( !score ) {
      GUM_ERROR ( NotFound, "the value is not in the cache" );
    }

    return *score;
  }


  /// default constructor

  template <typename Score>
  CacheNode<Score>::CacheNode() : has_score ( false ) {
    // for debugging purposes
    GUM_CONSTRUCTOR ( CacheNode );
  }


  /// copy constructor

  template <typename Score>
  CacheNode<Score>::CacheNode ( const CacheNode<Score>& from ) :
    score ( from.score ), has_score ( from.has_score ), children ( from.children ) {
    // for debugging purposes
    GUM_CONS_CPY ( CacheNode );
  }


  /// copy operator

  template <typename Score> INLINE
  CacheNode<Score>&
  CacheNode<Score>::operator= ( const CacheNode<Score>& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY ( CacheNode );
      score = from.score;
      has_score = from.has_score;
      children = from.children;
    }

    return *this;
  }


  /// destructor

  template <typename Score>
  CacheNode<Score>::~CacheNode() {
    // for debugging purposes
    GUM_DESTRUCTOR ( CacheNode );
  }


  /// operator << for the cache tree

  template <typename Score>
  std::ostream& operator<< ( std::ostream& stream,
                             const CacheNode<Score>& tree ) {
    return stream;
  }


} /* namespace gum */



#endif    // DOXYGEN_SHOULD_SKIP_THIS


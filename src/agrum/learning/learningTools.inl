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
 * @brief Common tools used by learning algorithms
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides common tools used by various learning algorithms.
 * */

#include <agrum/config.h>


#include <agrum/learning/learningTools.h>

namespace gum {

#ifndef DOXYGEN_SHOULD_SKIP_THIS


  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                    GUM_COUNTING_BOX IMPLEMENTATION                   === */
  /* ============================================================================ */
  /* ============================================================================ */


  /// returns a new box for the LearnCounting tree

  INLINE LearnCounting::CountingBox::CountingBox( unsigned int modal ) :
    next( 0 ), nb_cases( modal,0 ), children( modal,0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( CountingBox );
  }


  /// destructor

  INLINE LearnCounting::CountingBox::~CountingBox() {
    // for debugging purposes
    GUM_DESTRUCTOR( CountingBox );
  }


  /// returns the vector of occurrence counters of the box

  INLINE const std::vector<int>&
  LearnCounting::CountingBox::getCounters() const  {
    return nb_cases;
  }


  /// returns the number of occurrences of modality i found in the database

  INLINE int LearnCounting::CountingBox::getCounter( unsigned int i ) const {
    return nb_cases[i];
  }


  /// returns the vector of the children of the box

  INLINE const std::vector<LearnCounting::CountingBox*>&
  LearnCounting::CountingBox::getChildren() const  {
    return children;
  }


  /// returns the vector of the children of the box

  INLINE LearnCounting::CountingBox*
  LearnCounting::CountingBox::getNextBox() const  {
    return next;
  }





  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                   GUM_LEARN_COUNTING IMPLEMENTATION                  === */
  /* ============================================================================ */
  /* ============================================================================ */


  /// clears the tree structure

  INLINE void LearnCounting::clear() {
    // store the current tree into the garbage for future reuse
    for ( unsigned int i = 0; i != tree_level; ++i ) {
      level_end[i]->next = garbage;
      garbage = level_beg[i];
    }

    // update the tree
    tree_level = 0;
  }


  /// a function that parses the database and store the results into the tree

  INLINE void LearnCounting::parseDatabase( unsigned int min_level,
      unsigned int max_level )  {
    CountingBox* box;
    // check if min_level and max_level are OK

    if ( tree_level == 0 ) return;

    if ( ( min_level > max_level ) || ( max_level >= tree_level ) ) {
      GUM_ERROR( OutOfBounds, "min_level and/or max_level not ok" );
    }

    // parse the database
    unsigned int level, val;

    for ( Database::iterator iter=database.begin(); iter!=database.end(); ++iter ) {
      for ( level = 0, box = level_beg[0]; level != min_level;
            box = box->children[iter[level_row[level++]]] );

      while ( level < max_level ) {
        val = iter[level_row[level]];
        ++( box->nb_cases[val] );
        box = box->children[val];
        ++level;
      }

      ++( box->nb_cases[iter[level_row[level]]] );
    }
  }


  /// a function that parses the database and store the results into the tree

  INLINE void LearnCounting::parseDatabase()  {
    parseDatabase( 0, tree_level - 1 );
  }


  /// returns a new box for the tree

  INLINE LearnCounting::CountingBox*
  LearnCounting::newCountingBox( unsigned int modal ) {
    // check if we can take a box from the garbage
    if ( garbage ) {
      CountingBox* box = garbage;
      garbage = garbage->next;
      box->next = 0;
      box->nb_cases.resize( modal );
      box->children.resize( modal );

      for ( unsigned int i = 0; i<modal; ++i )
        box->nb_cases[i] = 0;

      return box;
    } else
      // here we need allocate a new box
      return new CountingBox( modal );
  }


  /// remove the last level of the tree, if any

  INLINE void LearnCounting::deleteLastLevel() {
    if ( tree_level ) {
      level_end[tree_level - 1]->next = garbage;
      garbage = level_beg[tree_level - 1];
      --tree_level;
    }
  }


  /// remove a given level of the tree as well as the levels below

  INLINE void LearnCounting::deleteLevel( unsigned int level ) {
    if ( tree_level <= level ) return;

    while ( level < tree_level ) {
      --tree_level;
      level_end[tree_level]->next = garbage;
      garbage = level_beg[tree_level];
    }
  }


  /// remove a whole level that does not belong to the tree

  INLINE void LearnCounting::deleteLevel( CountingBox* level_beg,
                                          CountingBox* level_end ) {
    level_end->next = garbage;
    garbage = level_beg;
  }


  /// extracts a level from the tree

  INLINE
  std::pair<LearnCounting::CountingBox*,LearnCounting::CountingBox*>
  LearnCounting::removeNGetLastLevel() {
    if ( tree_level ) {
      --tree_level;
      return std::pair<LearnCounting::CountingBox*,
             LearnCounting::CountingBox*> ( level_beg[tree_level],
                                            level_end[tree_level] );
    } else
      return std::pair<LearnCounting::CountingBox*,
             LearnCounting::CountingBox*> ( 0,0 );
  }


  /// extracts and remove a level from the tree

  INLINE
  std::pair<LearnCounting::CountingBox*,LearnCounting::CountingBox*>
  LearnCounting::removeNGetLevel( unsigned int level ) {
    if ( tree_level <= level )
      return std::pair<LearnCounting::CountingBox*,
             LearnCounting::CountingBox*> ( 0,0 );
    else {
      deleteLevel( level+1 );
      --tree_level;
      return std::pair<LearnCounting::CountingBox*,
             LearnCounting::CountingBox*> ( level_beg[level], level_end[level] );
    }
  }


  /// extracts a level from the tree but keep it in the tree

  INLINE LearnCounting::CountingBox* LearnCounting::getLastLevel()
  const  {
    if ( tree_level == 0 ) return 0;

    return level_beg[tree_level - 1];
  }


  /// extracts a level from the tree but keep it in the tree

  INLINE LearnCounting::CountingBox*
  LearnCounting::getLevel( unsigned int level )
  const  {
    if ( tree_level <= level ) return 0;

    return level_beg[level];
  }


  /// returns the number of levels in the tree

  INLINE unsigned int LearnCounting::getNbrLevels() const  {
    return tree_level;
  }

#endif  // DOXYGEN_SHOULD_SKIP_THIS


} /* namespace gum */

/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *   test $Id: $                                                           *
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
 * @brief Outlined implementation of the hash tables
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/core/hashTable.h>


namespace gum {

  /** The default number of slots in hashtables. By default, hashtables can store
   * up to thrice the number of slots elements before their size is increased. */
  const Size GUM_HASHTABLE_DEFAULT_SIZE = 4;

  /** the mean number of values admissible by slots. Once this number
   * is reached, the size of the hashtable is automatically doubled
   * if we are in automatic resize mode. */
  const unsigned int GUM_HASHTABLE_DEFAULT_MEAN_VAL_BY_SLOT = 3;

  /** a boolean indicating whether inserting too many values into the hashtable
   * makes it resize itself automatically. true = automatic, false = manual */
  const bool GUM_HASHTABLE_DEFAULT_RESIZE_POLICY = true;

  /** a boolean indicating the default behavior when trying to insert more than
   * once elements with identical keys. true = do not insert the elements but the
   * first one and throw an exception after the first insertion; false = insert
   * the elements without complaining */
  const bool GUM_HASHTABLE_DEFAULT_UNIQUENESS_POLICY = true;



  // creates (if needed) and returns the iterator __HashTableIterEnd
  const HashTableIterator<int,int>* HashTableIteratorStaticEnd::end4Statics() {
    static bool first_time = true;

    if ( first_time ) {
      first_time = false;
      __HashTableIterEnd = new HashTableIterator<int,int>;
    }

    return __HashTableIterEnd;
  }


  // creates (if needed) and returns the iterator __HashTableIterEnd
  const HashTableConstIterator<int,int>*
  HashTableIteratorStaticEnd::constEnd4Statics() {
    return
      reinterpret_cast<const HashTableConstIterator<int,int>*>( end4Statics() );
  }


  /// create the end iterator for all hash tables
  const HashTableIterator<int,int>*
  HashTableIteratorStaticEnd::__HashTableIterEnd =
    HashTableIteratorStaticEnd::end4Statics();


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

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
 * @brief the different boxes used by Learning Counting Trees.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides the different boxes used by learning Counting Trees. Those
 * are extensively used by learning algorithms: each time some conditionial
 * independence tests or local scores are computed by learning algorithms, some
 * tree of these boxes is constructed and the tests or scores are computed
 * by parsing the boxes.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <agrum/learning/countingTreeBoxes.h>


/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/countingTreeBoxes.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  namespace learning {

    
    /// the pools of boxes
    gum::ListBase<CountingTreeConditioningBox*>
    CountingTreeConditioningBox::__pool;
    gum::ListBase<CountingTreeTargetSetBox*>
    CountingTreeTargetSetBox::__pool;
    gum::ListBase<CountingTreeTargetBox*>
    CountingTreeTargetBox::__pool;


    /// the maximal size of the pools
    unsigned int CountingTreeConditioningBox::__pool_max_size =
      LEARNING_COUNTING_TREE_CONDITIONING_BOX_POOL_MAX_SIZE;
    unsigned int CountingTreeTargetSetBox::__pool_max_size =
      LEARNING_COUNTING_TREE_TARGET_SET_BOX_POOL_MAX_SIZE;
    unsigned int CountingTreeTargetBox::__pool_max_size =
      LEARNING_COUNTING_TREE_TARGET_BOX_POOL_MAX_SIZE;


 
 
    
    /// set the maximal capacity (number of boxes) of the pool
    void CountingTreeConditioningBox::setPoolCapacity ( unsigned int new_size ) {
      // if the new size is less than the current size, we shall discard the
      // boxes that are in excess
      while ( new_size < __pool.size() ) {
        delete __pool.front ();
        __pool.popFront ();
      }

      __pool_max_size = new_size;
    }
    

    /// clear the pool
    void CountingTreeConditioningBox::clearPool () {
      while ( __pool.size () ) {
        delete __pool.front ();
        __pool.popFront ();
      }
    }

    
   /// set the maximal capacity (number of boxes) of the pool
    void CountingTreeTargetBox::setPoolCapacity ( unsigned int new_size ) {
      // if the new size is less than the current size, we shall discard the
      // boxes that are in excess
      while ( new_size < __pool.size() ) {
        delete __pool.front ();
        __pool.popFront ();
      }

      __pool_max_size = new_size;
    }
    

    /// clear the pool
    void CountingTreeTargetBox::clearPool () {
      while ( __pool.size () ) {
        delete __pool.front ();
        __pool.popFront ();
      }
    }

    
    /// set the maximal capacity (number of boxes) of the pool
    void CountingTreeTargetSetBox::setPoolCapacity ( unsigned int new_size ) {
      // if the new size is less than the current size, we shall discard the
      // boxes that are in excess
      while ( new_size < __pool.size() ) {
        delete __pool.front ();
        __pool.popFront ();
      }

      __pool_max_size = new_size;
    }
    

    /// clear the pool
    void CountingTreeTargetSetBox::clearPool () {
      while ( __pool.size () ) {
        delete __pool.front ();
        __pool.popFront ();
      }
    }
    
    
  } /* namespace gum */

} /* namespace learning */
 

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


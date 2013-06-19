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
 * @brief a generic Counting Tree class designed for learning.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides a Counting Tree implementation that fills itself by
 * directly parsing a database. The class can also produce the list of
 * target set boxes of interest. However, it does not compute scores by itself.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


#include <limits>
#include <agrum/learning/countingTree.h>


/// include the inlined functions if necessary
#ifdef GUM_NO_INLINE
#include <agrum/learning/countingTree.inl>
#endif /* GUM_NO_INLINE */


namespace gum {

  namespace learning {


    /// default constructor
    CountingTree::CountingTree ( const Database& database ) :
      __database ( &database ),
      __db_conditioning_ids ( 0 ),
      __db_single_target_ids ( 0 ),
      __db_pair_target_ids ( 0 ),
      __has_conditioning_nodes ( false ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( CountingTree );
      __root.Conditioning = 0;
    }
      

    /// destructor
    CountingTree::~CountingTree () {
      // for debugging purposes
      GUM_DESTRUCTOR ( CountingTree );

      // clear the data structures
      clear ();
    }

    /// clear the tree and the set of target set nodes, if any
    void CountingTree::clear () {
      // remove the tree, if any
      if ( __root.Conditioning ) {
        if ( __has_conditioning_nodes ) {
          CountingTreeConditioningBox::deleteBox ( __root.Conditioning );
        }
        else {
          CountingTreeTargetSetBox::deleteBox ( __root.TargetSet );
        }
      }

      // empty the hashtables and vectors
      __db_conditioning_ids = 0;
      __db_single_target_ids = 0;
      __db_pair_target_ids = 0;
      __dbSingle2target.clear ();
      __dbPair2target.clear ();
      __target_modalities.clear ();
      __root.Conditioning = 0;
      __has_conditioning_nodes = false;
      __target_records.clear ();
    }

    
    /// fill the single targets
    void CountingTree::__fillSingleTargetTreeFromPairs () {
      // first, we determine for each single target from which pair the single
      // target counting should be computed. To do so, create a vector
      // best_pair_index which assigns to each element of __single_ids the
      // index of the best pair to be used. In addition, vector
      // is_best_pair_first_node indicates whether the single node for which
      // we need some countings is the first one in the pair.
      std::vector<unsigned int>
        best_pair_index ( __db_single_target_ids->size () );
      std::vector<bool>
        is_best_pair_first_node ( __db_single_target_ids->size () );
      {
        // create a hashtable indicating for each target id, its index
        // in __db_single_ids
        HashTable<unsigned int, unsigned int>
          id2index ( 2 * __db_single_target_ids->size () );
        for ( unsigned int i = 0; i < __db_single_target_ids->size (); ++i ) {
          id2index.insert ( __db_single_target_ids->operator[] ( i ), i );
        }
          
        // create a vector assigning to each single target node, the
        // number of operations needed to fill its boxes found so far
        std::vector<unsigned int>
          best_score ( __db_single_target_ids->size (),
                       std::numeric_limits<unsigned int>::max () );

        // parse the __db_pair_target_ids and select the best pair for each
        // single target node
        for ( unsigned int i = 0; i < __db_pair_target_ids->size (); ++i ) {
          const std::pair<unsigned int, unsigned int>& pair =
            __db_pair_target_ids->operator[] ( i );
          const unsigned int pair_size = __target_modalities[i];
          if ( id2index.exists ( pair.first ) ) {
            unsigned int index = id2index[pair.first];
            if ( best_score[index] > pair_size ) {
              best_score[index] = pair_size;
              best_pair_index[index] = i;
              is_best_pair_first_node[index] = true;
            }
          }
          if ( id2index.exists ( pair.second ) ) {
            unsigned int index = id2index[pair.second];
            if ( best_score[index] > pair_size ) {
              best_score[index] = pair_size;
              best_pair_index[index] = i;
              is_best_pair_first_node[index] = false;
            }
          }
        }
      }

      // here we compute the offset of the first single target node inside
      // a target set box
      unsigned int single_offset = __db_pair_target_ids->size ();
      
      // now, for each single node, we know from which pair we shall perform
      // the computation, so we can parse the target sets and fill the target
      // boxes of the single nodes
      for ( const ListBucket<CountingTreeTargetSetBox*>* iter =
              __target_records.frontBucket (); iter; iter = iter->next () ) {
        // fill all the single target boxes of the current target set box
        CountingTreeTargetSetBox* set_box = **iter;
        for ( unsigned int i = 0; i < __db_single_target_ids->size (); ++i ) {
          if ( ! __single_not_paired[i] ) {
            // fill the ith single target box
            CountingTreeTargetBox*
              single_box = set_box->child ( single_offset + i );
            const CountingTreeTargetBox*
              pair_box = set_box->child ( best_pair_index[i] );

            // fill the countings for all the modalities of the single target
            unsigned int pair_modal = __target_modalities[ best_pair_index[i] ];
            unsigned int nb_modal1  = __target_modalities[ single_offset + i ];
            unsigned int nb_modal2  = pair_modal / nb_modal1;
            if ( is_best_pair_first_node[i] ) {
              for ( unsigned int j = 0; j < nb_modal1; ++j ) {
                unsigned int count = 0;
                for ( unsigned int k = j; k < pair_modal; k+= nb_modal1 ) {
                  count += pair_box->nbRecords ( k );
                }
                single_box->setNbRecords ( j , count );
              }
            }
            else {
              // beware: here, nb_modal2 = domain size of the first variable
              // and nb_modal1 = domain size of the second variable
              for ( unsigned int j = 0, last = nb_modal2; j < nb_modal1;
                    ++j, last += nb_modal2 ) {
                unsigned int count = 0;
                for ( unsigned int k = j * nb_modal2; k < last; ++k ) {
                  count += pair_box->nbRecords ( k );
                }
                single_box->setNbRecords ( j , count );
              }
            }
          }
        }
      }
    }


    /// clear all the boxes pools
    void CountingTree::clearPools () {
      CountingTreeConditioningBox::clearPool ();
      CountingTreeTargetSetBox::clearPool ();
      CountingTreeTargetBox::clearPool ();
    }
 
    
  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

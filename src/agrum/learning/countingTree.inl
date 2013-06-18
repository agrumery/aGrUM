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


namespace gum {

  
  namespace learning {

 
    /// parse one database record to fill a given target pair set box
    ALWAYS_INLINE void
    CountingTree::__fillTargetPairSetBox ( CountingTreeTargetSetBox* box,
                                           const DatabaseIterator& iter ) {
      // increment the record number of parents
      box->incrementNbParentRecords ();
      
      // parse all the pairs for which we shall count the number of
      // occurences in the database
      for ( unsigned int i = 0; i < __db_pair_target_ids->size (); ++i ) {
        // get the modality of the pair for which we shall perform the increment
        const std::pair<unsigned int,unsigned int>& pair =
          __db_pair_target_ids->operator[] ( i );
        box->incrementNbRecords ( i, iter[pair.first] + iter[pair.second] *
                                  __database->nbrModalities ( pair.first ) );
      }
    }

    
    /// parse one database record to fill a given target single node set box
    ALWAYS_INLINE void
    CountingTree::__fillTargetSingleSetBox ( CountingTreeTargetSetBox* box,
                                             const DatabaseIterator& iter ) {
      // increment the record number of parents
      box->incrementNbParentRecords ();
      
      // parse all the single targets for which we shall count the number of
      // occurences in the database
      for ( unsigned int i = 0; i < __db_single_target_ids->size (); ++i ) {
        box->incrementNbRecords ( i,iter[__db_single_target_ids->operator[] (i)] );
      }
    }

    
    /// parse one database record to fill a given target single node set box
    ALWAYS_INLINE void
    CountingTree::__fillTargetSingleSubsetBox ( CountingTreeTargetSetBox* box,
                                                const DatabaseIterator& iter ) {
      // parse all the single targets for which we shall count the number of
      // occurences in the database
      for ( unsigned int i = 0; i < __single_not_paired_indices.size (); ++i ) {
        const unsigned int index = __single_not_paired_indices[i];
        box->incrementNbRecords ( __db_pair_target_ids->size() + index,
                                  iter[__db_single_target_ids->operator[]
                                       ( index ) ] );
      }
    }

    
    /// traverse the conditional nodes of the tree corresponding to one db record
    ALWAYS_INLINE CountingTreeTargetSetBox*
    CountingTree::__fillConditioningBoxes ( const DatabaseIterator& iter ) {
      // start from the root
      CountingTreeConditioningBox* current_box = __root.Conditioning;

      // parse all the nodes that appear in the conditioning set
      // we parse the conditioning set from the last element to the second one
      // because all of them are conditioning nodes. The first element of
      // __db_conditioning_ids will be treated differently because its child
      // is not a conditioning node but rather a target set node
      for ( unsigned int i = __db_conditioning_ids->size () - 1; i > 0; --i ) {
        // get the index of the box at the next level
        const unsigned int index = iter[__db_conditioning_ids->operator[] ( i )];

        // if the box has not been created yet, do it
        if ( ! current_box->child ( index ) ) {
          CountingTreeConditioningBox* new_box;
          if ( i == 1 ) { // when i = 1, this corresponds to the last_level
            new_box = CountingTreeConditioningBox::createBox
              ( __database->nbrModalities
                ( __db_conditioning_ids->operator[] ( i-1 ) ),
                true );
            
            // add the box to the set of conditioning boxes of the last level
            __last_cond_nodes.pushBack ( new_box );
          }
          else {
            new_box = CountingTreeConditioningBox::createBox
              ( __database->nbrModalities
                ( __db_conditioning_ids->operator[] ( i-1 ) ),
                false );
          }

          current_box->setChild ( index, new_box );

          // assign the child as the current box
          current_box = new_box;
        }
        else {
          // assign the child as the current box
          current_box = current_box->child ( index );
        }
      }

      // for the last conditioning box, go to its child and create it if needed
      // but the child will be a target set box
      const unsigned int index = iter[__db_conditioning_ids->operator[] ( 0 )];
      if ( ! current_box->child ( index ) ) {
        // create the box
        CountingTreeTargetSetBox* new_box =
          CountingTreeTargetSetBox::createBox ( __target_modalities );
        new_box->setNbParentRecords ( 0 );

        // assign it as the child of the current box
        current_box->setChild ( index, new_box );

        // do not forget to add the child into the list of target set boxes
        __target_records.pushBack ( new_box );

        // return the box
        return new_box;
      }
      else {
        // return the target set box resulting from the traversal
        return reinterpret_cast<CountingTreeTargetSetBox*>
          ( current_box->child ( index ) );
      }
    }

    
    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__fillUnconditionalPairTree () {
      // first, we shall create the root of the unconditional tree, which is,
      // actually, the only targetSetBox
      __root.TargetSet =
        CountingTreeTargetSetBox::createBox ( __target_modalities );

      // put the root into the list of the target set boxes
      __target_records.pushFront ( __root.TargetSet );

      // now, fill it by parsing the database
      if ( __single_not_paired_indices.size () ) {
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          __fillTargetPairSetBox ( __root.TargetSet, iter );
          __fillTargetSingleSubsetBox ( __root.TargetSet, iter );
        }
      }
      else {
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          __fillTargetPairSetBox ( __root.TargetSet, iter );
        }
      }
    }

    
    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__fillUnconditionalSingleTree () {
      // first, we shall create the root of the unconditional tree, which is,
      // actually, the only targetSetBox
      __root.TargetSet =
        CountingTreeTargetSetBox::createBox ( __target_modalities );

      // put the root into the list of the target set boxes
      __target_records.pushFront ( __root.TargetSet );

      // now, fill it by parsing the database
      for ( DatabaseIterator iter = __database->begin ();
            iter != __database->end (); ++iter ) {
        __fillTargetSingleSetBox ( __root.TargetSet, iter );
      }
    }

        
    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__fillConditionalPairTree () {
      // first, we shall create the root of the conditional tree
      const bool last_level = __db_conditioning_ids->size() == 1;
      __root.Conditioning =
        CountingTreeConditioningBox::createBox
        ( __database->nbrModalities
          ( __db_conditioning_ids->operator[]
            ( __db_conditioning_ids->size() - 1 ) ), last_level );
      if ( last_level ) {
        __last_cond_nodes.pushBack ( __root.Conditioning );
      }
      
      // now fill it by parsing the database
      if ( __single_not_paired_indices.size () ) {
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
          __fillTargetPairSetBox ( target_box, iter );
          __fillTargetSingleSubsetBox ( target_box, iter );
        }
      }
      else {
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
          __fillTargetPairSetBox ( target_box, iter );
        }
      }
    }


    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__refillConditionalPairTree () {
      if ( __single_not_paired_indices.size () ) {
        // fill the tree by parsing the database
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
          __fillTargetPairSetBox ( target_box, iter );
          __fillTargetSingleSubsetBox ( target_box, iter );
        }
      }
      else {
        // fill the tree by parsing the database
        for ( DatabaseIterator iter = __database->begin ();
              iter != __database->end (); ++iter ) {
          CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
          __fillTargetPairSetBox ( target_box, iter );
        }
      }
    }


    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__fillConditionalSingleTree () {
      // first, we shall create the root of the conditional tree
      const bool last_level = __db_conditioning_ids->size() == 1;
      __root.Conditioning =
        CountingTreeConditioningBox::createBox
        ( __database->nbrModalities
          ( __db_conditioning_ids->operator[]
            ( __db_conditioning_ids->size() - 1 ) ), last_level );
      if ( last_level ) {
        __last_cond_nodes.pushBack ( __root.Conditioning );
      }

      // now fill it by parsing the database
      for ( DatabaseIterator iter = __database->begin ();
            iter != __database->end (); ++iter ) {
        CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
        __fillTargetSingleSetBox ( target_box, iter );
      }
    }

    
    /// fill a whole tree by parsing the complete database
    ALWAYS_INLINE void CountingTree::__refillConditionalSingleTree () {
      // now fill the tree by parsing the database
      for ( DatabaseIterator iter = __database->begin ();
            iter != __database->end (); ++iter ) {
        CountingTreeTargetSetBox* target_box =__fillConditioningBoxes ( iter );
        __fillTargetSingleSetBox ( target_box, iter );
      }
    }

    
    /// assign a new set of target nodes and compute countings
    ALWAYS_INLINE void CountingTree::setTargetNodes
    ( const std::vector<unsigned int>& db_single_ids,
      const std::vector< std::pair<unsigned int,unsigned int> >& db_pair_ids ) {
      // save the target ids
      __db_single_target_ids = &db_single_ids;
      __db_pair_target_ids   = &db_pair_ids;

      // fill the pair target index and the target modalities
      __dbPair2target.clear ();
      HashTable<unsigned int,bool> all_paired_ids ( 2 * __dbPair2target.size () );
      __target_modalities.resize ( db_single_ids.size () + db_pair_ids.size () );
      for ( unsigned int i = 0; i < db_pair_ids.size (); ++i ) {
        __dbPair2target.insert ( db_pair_ids[i], i );
        __target_modalities[i] =
          __database->nbrModalities ( db_pair_ids[i].first ) *
          __database->nbrModalities ( db_pair_ids[i].second );
        all_paired_ids.set ( db_pair_ids[i].first, true );
        all_paired_ids.set ( db_pair_ids[i].second, true );
      }

      // fill the single target index and the target modalities
      __dbSingle2target.clear ();
      for ( unsigned int i = 0, j = db_pair_ids.size ();
            i < db_single_ids.size (); ++i, ++j ) {
        __dbSingle2target.insert ( db_single_ids[i], j );
        __target_modalities[j] = __database->nbrModalities ( db_single_ids[i] );
      }

      // compute which single target ids need to be computed from the database
      // rather than from pairs
      if ( db_pair_ids.size () ) {
        __single_not_paired_indices.clear ();
        __single_not_paired.resize ( db_single_ids.size () );
        for ( unsigned int i = 0; i < db_single_ids.size (); ++i ) {
          if ( ! all_paired_ids.exists ( db_single_ids[i] ) ) {
            __single_not_paired_indices.push_back ( i );
            __single_not_paired[i] = true;
          }
          else {
            __single_not_paired[i] = false;
          }
        }
      }
             
      // remove the target trees, if any
      bool should_refill = false;
      if ( ! __last_cond_nodes.empty () ) {
        should_refill = true;
        
        // remove the curent target set boxes
        for ( const ListBucket<CountingTreeTargetSetBox*>* iter =
                __target_records.frontBucket (); iter; iter = iter->next () ) {
          CountingTreeTargetSetBox::deleteBox ( **iter );
        }

        // indicate to the conditioning boxes at the last level that their
        // children do not exist anymore
        for ( const ListBucket<CountingTreeConditioningBox*>* iter =
                __last_cond_nodes.frontBucket (); iter; iter = iter->next () ) {
          (**iter)->unsetChildren ();
        }
      }
      else {
        if ( __root.TargetSet ) {
          CountingTreeTargetSetBox::deleteBox ( __root.TargetSet );
          __root.TargetSet = 0;
        }
      }

      // clear the old target records
      __target_records.clear ();

      // recompute the target records
      if  ( db_pair_ids.size () ) {
        // here, there are target pairs, so we shall fill the target boxes
        // of those pairs from the database and, then, fill the single target
        // boxes from those of the pairs
        if ( __has_conditioning_nodes ) {
          if ( should_refill ) {
            __refillConditionalPairTree ();
          }
          else {
            __fillConditionalPairTree ();
          }
        }
        else {
          __fillUnconditionalPairTree ();
        }
        __fillSingleTargetTreeFromPairs ();
      }
      else {
        // here, there are no target pairs, so we whall fill the single pair
        // boxes by parsing the database
        if ( __has_conditioning_nodes ) {
          if ( should_refill ) {
            __refillConditionalSingleTree ();
          }
          else {
            __fillConditionalSingleTree ();
          }
        }
        else {
          __fillUnconditionalSingleTree ();
        }
      }
    }

    
    /// assign a new set of target nodes and compute countings
    ALWAYS_INLINE void CountingTree::setTargetNodes
    ( const std::vector<unsigned int>& db_single_ids ) {
      setTargetNodes ( db_single_ids, __empty_pairs );
    }
 

    /// assign a new set of conditioning and target nodes and compute countings
    ALWAYS_INLINE void CountingTree::setNodes
    ( const std::vector<unsigned int>& db_conditioning_ids,
      const std::vector<unsigned int>& db_single_target_ids,
      const std::vector< std::pair<unsigned int,unsigned int> >&
      db_pair_target_ids ) {
      // remove the tree, if any. It is compulsory to do this operation before
      // changing the conditioning nodes because, currently, the tree may
      // not have any conditioning node and, therefore, we shall remove only
      // target set boxes at this point
      if ( __root.Conditioning ) {
        if ( __has_conditioning_nodes ) {
          CountingTreeConditioningBox::deleteBox ( __root.Conditioning );
        }
        else {
          CountingTreeTargetSetBox::deleteBox ( __root.TargetSet );
        }
      }
      __root.Conditioning = 0;
      __last_cond_nodes.clear ();
      
      // save the conditioning ids
      __db_conditioning_ids = &db_conditioning_ids;
      __has_conditioning_nodes = ! __db_conditioning_ids->empty ();

      // process the target nodes and compute the countings
      setTargetNodes ( db_single_target_ids, db_pair_target_ids );
    }
    
    
    /// returns the index within target sets of a single target node id
    ALWAYS_INLINE unsigned int
    CountingTree::targetIndex ( unsigned int db_target_id ) const {
      return __dbSingle2target[db_target_id];
    }
        

    /// returns the index within target sets of a pair of target nodes
    ALWAYS_INLINE unsigned int
    CountingTree::targetIndex
    ( const std::pair<unsigned int,unsigned int>& db_target_id ) const {
      return __dbPair2target[db_target_id];
    }


    /// returns the list of target set generated by parsing the database
    ALWAYS_INLINE const ListBase<CountingTreeTargetSetBox*>&
    CountingTree::nbRecords () const {
      return __target_records;
    }


  } /* namespace learning */
  
  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

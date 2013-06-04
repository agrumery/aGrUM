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

namespace gum {

  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                       COUNTING TREE TARGET BOX                    === */
    /* ========================================================================= */
    /* ========================================================================= */

    /// default constructor
    ALWAYS_INLINE
    CountingTreeTargetBox::CountingTreeTargetBox ( unsigned int size ) :
      __nb_records ( size, 0 ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( CountingTreeTargetBox );
    }

    
    /// destructor
    ALWAYS_INLINE CountingTreeTargetBox::~CountingTreeTargetBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( CountingTreeTargetBox );
    }

    
    /// select a CountingTreeTargetBox from a pool and return it
    ALWAYS_INLINE CountingTreeTargetBox*
    CountingTreeTargetBox::createBox ( unsigned int size ) {
      if ( __pool.empty () ) {
        return new CountingTreeTargetBox ( size );
      }
      else {
        CountingTreeTargetBox* box = __pool.front ();
        __pool.popFront ();
        box->__resize ( size );
        return box;
      }
    }

    
    /// reset the variable to en empty (no modality) variable
    ALWAYS_INLINE void CountingTreeTargetBox::__clear () {
      __nb_records.clear ();
    }

 
    /// put a CountingTreeTargetBox into the pool
    ALWAYS_INLINE void
    CountingTreeTargetBox::deleteBox ( CountingTreeTargetBox* box ) {
      box->__clear ();
      if ( __pool.size () >= __pool_max_size ) {
        delete box;
      }
      else {
        __pool.pushBack ( box );
      }
    }


    /// get the number of records of the ith modality
    ALWAYS_INLINE unsigned int
    CountingTreeTargetBox::nbRecords ( unsigned int i ) const {
      return __nb_records[i];
    }


    /// returns the number of records for all the modalities;
    ALWAYS_INLINE const std::vector<unsigned int>&
    CountingTreeTargetBox::nbRecords () const {
      return __nb_records;
    }

    
    /// increments the number of observations of the ith modality
    ALWAYS_INLINE void
    CountingTreeTargetBox::incrementNbRecords ( unsigned int i ) {
      ++__nb_records[i];
    }

    
    /// increments the number of observations of the ith modality by k
    ALWAYS_INLINE void
    CountingTreeTargetBox::incrementNbRecords ( unsigned int i, unsigned int k ) {
      __nb_records[i] += k;
    }

    
    /// sets the number of records of the ith modality
    ALWAYS_INLINE void
    CountingTreeTargetBox::setNbRecords ( unsigned int i, unsigned int k ) {
      __nb_records[i] = k;
    }

    
    /// resize the number of values of the target node
    ALWAYS_INLINE void CountingTreeTargetBox::__resize ( unsigned int new_size ) {
      __nb_records.resize ( new_size, 0 );
    }


    

    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                   COUNTING TREE TARGET SET BOX                    === */
    /* ========================================================================= */
    /* ========================================================================= */

    /// default constructor
    ALWAYS_INLINE CountingTreeTargetSetBox::CountingTreeTargetSetBox
    ( const std::vector<unsigned int>& variable_modalities ) :
      __target_boxes ( variable_modalities.size() ),
      __nb_parent_records ( 0 ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( CountingTreeTargetSetBox );

      // create the set of target boxes
      for ( unsigned int i = 0; i < variable_modalities.size(); ++i ) {
        __target_boxes[i] =
          CountingTreeTargetBox::createBox ( variable_modalities[i] );
      }
    }
      

    /// destructor
    ALWAYS_INLINE CountingTreeTargetSetBox::~CountingTreeTargetSetBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( CountingTreeTargetSetBox );

      // delete the children
      for ( unsigned int i = 0; i < __target_boxes.size(); ++i ) {
        delete __target_boxes[i];
      }
    }


    /// returns a CountingTreeTargetSetBox as well as its target boxes
    ALWAYS_INLINE CountingTreeTargetSetBox*
    CountingTreeTargetSetBox::createBox
    ( const std::vector<unsigned int>& variable_modalities ) {
      if ( __pool.empty () ) {
        return new CountingTreeTargetSetBox ( variable_modalities );
      }
      else {
        // get the box from the pool
        CountingTreeTargetSetBox* box = __pool.front ();
        __pool.popFront ();

        // fill it with its children
        box->__target_boxes.resize ( variable_modalities.size () );
        for ( unsigned int i = 0; i < variable_modalities.size (); ++i ) {
          box->__target_boxes[i] = 
            CountingTreeTargetBox::createBox ( variable_modalities[i] );
        }
        
        return box;
      }
    }

    
    /// reset the children to the empty set
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::clear ( bool clear_nb_parent_records ) {
      // remove all the children
      for ( unsigned int i = 0; i < __target_boxes.size (); ++i ) {
        if ( __target_boxes[i] )
          CountingTreeTargetBox::deleteBox ( __target_boxes[i] );
      }

      __target_boxes.clear ();

      if ( clear_nb_parent_records ) {
        __nb_parent_records = 0;
      }
    }

 
    /// put a CountingTreeTargetBox into the pool
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::deleteBox ( CountingTreeTargetSetBox* box ) {
      box->clear ( true );
      if ( __pool.size () >= __pool_max_size ) {
        delete box;
      }
      else {
        __pool.pushBack ( box );
      }
    }


    /// get the number of records observed in the database for the parents
    ALWAYS_INLINE unsigned int CountingTreeTargetSetBox::nbParentRecords () const {
      return __nb_parent_records;
    }
    
      
    /// get the number of cases of the ith variable
    ALWAYS_INLINE const std::vector<unsigned int>&
    CountingTreeTargetSetBox::nbRecords ( unsigned int i ) const {
      return __target_boxes[i]->nbRecords ();
    }


    /// get the number of cases of the jth modality of the ith variable
    ALWAYS_INLINE unsigned int
    CountingTreeTargetSetBox::nbRecords ( unsigned int i, unsigned int j ) const {
      return __target_boxes[i]->nbRecords ( j );
    }
   

    /// returns the target box for the ith variable
    ALWAYS_INLINE CountingTreeTargetBox*
    CountingTreeTargetSetBox::child ( unsigned int i ) const {
      return __target_boxes[i];
    }
    

    /** @brief increment de the number of observations of the jth value of the
     * ith variable */ 
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::incrementNbRecords ( unsigned int i,
                                                  unsigned int j ) const {
      __target_boxes[i]->incrementNbRecords ( j );
    }


    /** @brief increment the number of observations of the jth value of the
     * ith variable by k */
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::incrementNbRecords ( unsigned int i,
                                                  unsigned int j,
                                                  unsigned int k ) const {
      __target_boxes[i]->incrementNbRecords ( j, k );
    }

    
    /// sets the number of cases of the jth value of the ith variable
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::setNbRecords ( unsigned int i,
                                            unsigned int j,
                                            unsigned int k ) const {
      __target_boxes[i]->setNbRecords ( j, k );
    }

    
    /// increment the number of observations of the parents of the container
    ALWAYS_INLINE void CountingTreeTargetSetBox::incrementNbParentRecords () {
      ++__nb_parent_records;
    }

    
    /// increment the number of observations of the parents of the container
    ALWAYS_INLINE void
    CountingTreeTargetSetBox::incrementNbParentRecords ( unsigned int k) {
      __nb_parent_records += k;
    }

   
    /// sets a new set of target variables
    ALWAYS_INLINE void CountingTreeTargetSetBox::setVariables
    ( const std::vector<unsigned int>& variable_modalities,
      bool clear_nb_parent_records ) {
      __target_boxes.resize ( variable_modalities.size() );
      // create the set of target boxes
      for ( unsigned int i = 0; i < variable_modalities.size(); ++i ) {
        __target_boxes[i] =
          CountingTreeTargetBox::createBox ( variable_modalities[i] );
      }

      // if needed, reset the counter of records of the parent nodes
      if ( clear_nb_parent_records ) {
        __nb_parent_records = 0;
      }
    }



    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                   COUNTING TREE CONDITIONING BOX                  === */
    /* ========================================================================= */
    /* ========================================================================= */
     
    /// default constructor
    ALWAYS_INLINE CountingTreeConditioningBox::CountingTreeConditioningBox
    ( unsigned int children_size,
      bool final_level ) :
      __children ( children_size, 0 ),
      __final_level ( final_level ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( CountingTreeConditioningBox );
    }
      

    /// destructor
    INLINE CountingTreeConditioningBox::~CountingTreeConditioningBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( CountingTreeConditioningBox );
      
      // remove all the children
      if ( __final_level ) {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) {
            delete reinterpret_cast<CountingTreeTargetSetBox*> ( __children[i] );
          }
        }
      }
      else {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) delete __children[i];
        }
      }
    }

    
    /// select a CountingTreeConditioningBox from a pool and return it
    ALWAYS_INLINE CountingTreeConditioningBox*
    CountingTreeConditioningBox::createBox ( unsigned int children_size,
                                            bool final_level ) {
      if ( __pool.empty () ) {
        return new CountingTreeConditioningBox ( children_size, final_level );
      }
      else {
        CountingTreeConditioningBox* box = __pool.front ();
        __pool.popFront ();
        box->__resize ( children_size );
        box->__setFinal ( final_level );
        return box;
      }
    }

    
    /// reset the children to the empty set
    INLINE void CountingTreeConditioningBox::clear () {
      // remove all the children, if any
      if ( __final_level ) {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) {
            CountingTreeTargetSetBox::deleteBox
              ( reinterpret_cast<CountingTreeTargetSetBox*> ( __children[i] ) );
          }
        }
      }
      else {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) deleteBox ( __children[i] );
        }
      }
      
      __children.clear ();
    }

 
    /// put a CountingTreeConditioningBox into the pool
    ALWAYS_INLINE void
    CountingTreeConditioningBox::deleteBox ( CountingTreeConditioningBox* box ) {
      // clear the box, so that the next time it is created out of the pool, it
      // has no child
      box->clear ();
      if ( __pool.size () >= __pool_max_size ) {
        delete box;
      }
      else {
        __pool.pushBack ( box );
      }
    }


    /// returns the set of children
    ALWAYS_INLINE const std::vector<CountingTreeConditioningBox*>&
    CountingTreeConditioningBox::children () const {
      return __children;
    }

    
    /// indicates whether there exists a child at the ith modality
    ALWAYS_INLINE bool
    CountingTreeConditioningBox::hasChild ( unsigned int i ) const {
      return ( __children[i] != 0 );
    }

    
    /// returns the ith child
    ALWAYS_INLINE CountingTreeConditioningBox*
    CountingTreeConditioningBox::child ( unsigned int i ) const {
      return __children[i];
    }
      
 
    /// sets the ith child
    ALWAYS_INLINE void
    CountingTreeConditioningBox::setChild ( unsigned int i,
                                           CountingTreeConditioningBox* box ) {
      __children[i] = box;
    }

    
    /// sets the ith child
    ALWAYS_INLINE void
    CountingTreeConditioningBox::setChild ( unsigned int i,
                                           CountingTreeTargetSetBox* box ) {
      __children[i] = reinterpret_cast<CountingTreeConditioningBox*> ( box );
    }

    
    /// indicates whether the box is at the last conditioning level
    ALWAYS_INLINE bool CountingTreeConditioningBox::isAtFinalLevel () const {
      return __final_level;
    }
      

    /// resize the number of children
    ALWAYS_INLINE void 
    CountingTreeConditioningBox::__resize ( unsigned int new_children_size ) {
      __children.resize ( new_children_size, 0 );
    }

    
    /// sets the final_level Boolean
    ALWAYS_INLINE void
    CountingTreeConditioningBox::__setFinal ( bool final_level ) {
      __final_level = final_level;
    }

    
  } /* namespace gum */

  
} /* namespace learning */
 

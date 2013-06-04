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
 * @brief the different boxes used by Learning Scoring Trees.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 *
 * This file provides the different boxes used by learning Scoring Trees. Those are
 * extensively used by learning algorithms: each time some conditionial
 * independence tests or local scores are computed by learning algorithms, some
 * tree of these boxes is constructed and the tests or scores are computed
 * by parsing the boxes.
 */

namespace gum {

  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                       SCORING TREE TARGET BOX                     === */
    /* ========================================================================= */
    /* ========================================================================= */

    /// default constructor
    ALWAYS_INLINE
    ScoringTreeTargetBox::ScoringTreeTargetBox ( unsigned int size ) :
      __nb_records ( size, 0 ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoringTreeTargetBox );
    }

    
    /// destructor
    ALWAYS_INLINE ScoringTreeTargetBox::~ScoringTreeTargetBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoringTreeTargetBox );
    }

    
    /// select a ScoringTreeTargetBox from a pool and return it
    ALWAYS_INLINE ScoringTreeTargetBox*
    ScoringTreeTargetBox::createBox ( unsigned int size ) {
      if ( __pool.empty () ) {
        return new ScoringTreeTargetBox ( size );
      }
      else {
        ScoringTreeTargetBox* box = __pool.front ();
        __pool.popFront ();
        box->__resize ( size );
        return box;
      }
    }

    
    /// reset the variable to en empty (no modality) variable
    ALWAYS_INLINE void ScoringTreeTargetBox::__clear () {
      __nb_records.clear ();
    }

 
    /// put a ScoringTreeTargetBox into the pool
    ALWAYS_INLINE void
    ScoringTreeTargetBox::deleteBox ( ScoringTreeTargetBox* box ) {
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
    ScoringTreeTargetBox::nbRecords ( unsigned int i ) const {
      return __nb_records[i];
    }


    /// returns the number of records for all the modalities;
    ALWAYS_INLINE const std::vector<unsigned int>&
    ScoringTreeTargetBox::nbRecords () const {
      return __nb_records;
    }

    
    /// increments the number of observations of the ith modality
    ALWAYS_INLINE void
    ScoringTreeTargetBox::incrementNbRecords ( unsigned int i ) {
      ++__nb_records[i];
    }

    
    /// increments the number of observations of the ith modality by k
    ALWAYS_INLINE void
    ScoringTreeTargetBox::incrementNbRecords ( unsigned int i, unsigned int k ) {
      __nb_records[i] += k;
    }

    
    /// sets the number of records of the ith modality
    ALWAYS_INLINE void
    ScoringTreeTargetBox::setNbRecords ( unsigned int i, unsigned int k ) {
      __nb_records[i] = k;
    }

    
    /// resize the number of values of the target node
    ALWAYS_INLINE void ScoringTreeTargetBox::__resize ( unsigned int new_size ) {
      __nb_records.resize ( new_size, 0 );
    }


    

    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                    SCORING TREE TARGET SET BOX                    === */
    /* ========================================================================= */
    /* ========================================================================= */

    /// default constructor
    ALWAYS_INLINE ScoringTreeTargetSetBox::ScoringTreeTargetSetBox
    ( const std::vector<unsigned int>& variable_modalities ) :
      __target_boxes ( variable_modalities.size() ),
      __nb_parent_records ( 0 ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoringTreeTargetSetBox );

      // create the set of target boxes
      for ( unsigned int i = 0; i < variable_modalities.size(); ++i ) {
        __target_boxes[i] =
          ScoringTreeTargetBox::createBox ( variable_modalities[i] );
      }
    }
      

    /// destructor
    ALWAYS_INLINE ScoringTreeTargetSetBox::~ScoringTreeTargetSetBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoringTreeTargetSetBox );

      // delete the children
      for ( unsigned int i = 0; i < __target_boxes.size(); ++i ) {
        delete __target_boxes[i];
      }
    }


    /// returns a ScoringTreeTargetSetBox as well as its target boxes
    ALWAYS_INLINE ScoringTreeTargetSetBox*
    ScoringTreeTargetSetBox::createBox
    ( const std::vector<unsigned int>& variable_modalities ) {
      if ( __pool.empty () ) {
        return new ScoringTreeTargetSetBox ( variable_modalities );
      }
      else {
        // get the box from the pool
        ScoringTreeTargetSetBox* box = __pool.front ();
        __pool.popFront ();

        // fill it with its children
        box->__target_boxes.resize ( variable_modalities.size () );
        for ( unsigned int i = 0; i < variable_modalities.size (); ++i ) {
          box->__target_boxes[i] = 
            ScoringTreeTargetBox::createBox ( variable_modalities[i] );
        }
        
        return box;
      }
    }

    
    /// reset the children to the empty set
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::clear ( bool clear_nb_parent_records ) {
      // remove all the children
      for ( unsigned int i = 0; i < __target_boxes.size (); ++i ) {
        if ( __target_boxes[i] )
          ScoringTreeTargetBox::deleteBox ( __target_boxes[i] );
      }

      __target_boxes.clear ();

      if ( clear_nb_parent_records ) {
        __nb_parent_records = 0;
      }
    }

 
    /// put a ScoringTreeTargetBox into the pool
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::deleteBox ( ScoringTreeTargetSetBox* box ) {
      box->clear ( true );
      if ( __pool.size () >= __pool_max_size ) {
        delete box;
      }
      else {
        __pool.pushBack ( box );
      }
    }


    /// get the number of records observed in the database for the parents
    ALWAYS_INLINE unsigned int ScoringTreeTargetSetBox::nbParentRecords () const {
      return __nb_parent_records;
    }
    
      
    /// get the number of cases of the ith variable
    ALWAYS_INLINE const std::vector<unsigned int>&
    ScoringTreeTargetSetBox::nbRecords ( unsigned int i ) const {
      return __target_boxes[i]->nbRecords ();
    }


    /// get the number of cases of the jth modality of the ith variable
    ALWAYS_INLINE unsigned int
    ScoringTreeTargetSetBox::nbRecords ( unsigned int i, unsigned int j ) const {
      return __target_boxes[i]->nbRecords ( j );
    }
   

    /// returns the target box for the ith variable
    ALWAYS_INLINE ScoringTreeTargetBox*
    ScoringTreeTargetSetBox::child ( unsigned int i ) const {
      return __target_boxes[i];
    }
    

    /** @brief increment de the number of observations of the jth value of the
     * ith variable */ 
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::incrementNbRecords ( unsigned int i,
                                                  unsigned int j ) const {
      __target_boxes[i]->incrementNbRecords ( j );
    }


    /** @brief increment the number of observations of the jth value of the
     * ith variable by k */
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::incrementNbRecords ( unsigned int i,
                                                  unsigned int j,
                                                  unsigned int k ) const {
      __target_boxes[i]->incrementNbRecords ( j, k );
    }

    
    /// sets the number of cases of the jth value of the ith variable
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::setNbRecords ( unsigned int i,
                                            unsigned int j,
                                            unsigned int k ) const {
      __target_boxes[i]->setNbRecords ( j, k );
    }

    
    /// increment the number of observations of the parents of the container
    ALWAYS_INLINE void ScoringTreeTargetSetBox::incrementNbParentRecords () {
      ++__nb_parent_records;
    }

    
    /// increment the number of observations of the parents of the container
    ALWAYS_INLINE void
    ScoringTreeTargetSetBox::incrementNbParentRecords ( unsigned int k) {
      __nb_parent_records += k;
    }

   
    /// sets a new set of target variables
    ALWAYS_INLINE void ScoringTreeTargetSetBox::setVariables
    ( const std::vector<unsigned int>& variable_modalities,
      bool clear_nb_parent_records ) {
      __target_boxes.resize ( variable_modalities.size() );
      // create the set of target boxes
      for ( unsigned int i = 0; i < variable_modalities.size(); ++i ) {
        __target_boxes[i] =
          ScoringTreeTargetBox::createBox ( variable_modalities[i] );
      }

      // if needed, reset the counter of records of the parent nodes
      if ( clear_nb_parent_records ) {
        __nb_parent_records = 0;
      }
    }



    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                   SCORING TREE CONDITIONING BOX                   === */
    /* ========================================================================= */
    /* ========================================================================= */
     
    /// default constructor
    ALWAYS_INLINE ScoringTreeConditioningBox::ScoringTreeConditioningBox
    ( unsigned int children_size,
      bool final_level ) :
      __children ( children_size, 0 ),
      __final_level ( final_level ) {
      // for debugging purposes
      GUM_CONSTRUCTOR ( ScoringTreeConditioningBox );
    }
      

    /// destructor
    INLINE ScoringTreeConditioningBox::~ScoringTreeConditioningBox () {
      // for debugging purposes
      GUM_DESTRUCTOR ( ScoringTreeConditioningBox );
      
      // remove all the children
      if ( __final_level ) {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) {
            delete reinterpret_cast<ScoringTreeTargetSetBox*> ( __children[i] );
          }
        }
      }
      else {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) delete __children[i];
        }
      }
    }

    
    /// select a ScoringTreeConditioningBox from a pool and return it
    ALWAYS_INLINE ScoringTreeConditioningBox*
    ScoringTreeConditioningBox::createBox ( unsigned int children_size,
                                            bool final_level ) {
      if ( __pool.empty () ) {
        return new ScoringTreeConditioningBox ( children_size, final_level );
      }
      else {
        ScoringTreeConditioningBox* box = __pool.front ();
        __pool.popFront ();
        box->__resize ( children_size );
        box->__setFinal ( final_level );
        return box;
      }
    }

    
    /// reset the children to the empty set
    INLINE void ScoringTreeConditioningBox::clear () {
      // remove all the children, if any
      if ( __final_level ) {
        for ( unsigned int i = 0; i < __children.size (); ++i ) {
          if ( __children[i] ) {
            ScoringTreeTargetSetBox::deleteBox
              ( reinterpret_cast<ScoringTreeTargetSetBox*> ( __children[i] ) );
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

 
    /// put a ScoringTreeConditioningBox into the pool
    ALWAYS_INLINE void
    ScoringTreeConditioningBox::deleteBox ( ScoringTreeConditioningBox* box ) {
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
    ALWAYS_INLINE const std::vector<ScoringTreeConditioningBox*>&
    ScoringTreeConditioningBox::children () const {
      return __children;
    }

    
    /// indicates whether there exists a child at the ith modality
    ALWAYS_INLINE bool
    ScoringTreeConditioningBox::hasChild ( unsigned int i ) const {
      return ( __children[i] != 0 );
    }

    
    /// returns the ith child
    ALWAYS_INLINE ScoringTreeConditioningBox*
    ScoringTreeConditioningBox::child ( unsigned int i ) const {
      return __children[i];
    }
      
 
    /// sets the ith child
    ALWAYS_INLINE void
    ScoringTreeConditioningBox::setChild ( unsigned int i,
                                           ScoringTreeConditioningBox* box ) {
      __children[i] = box;
    }

    
    /// sets the ith child
    ALWAYS_INLINE void
    ScoringTreeConditioningBox::setChild ( unsigned int i,
                                           ScoringTreeTargetSetBox* box ) {
      __children[i] = reinterpret_cast<ScoringTreeConditioningBox*> ( box );
    }

    
    /// indicates whether the box is at the last conditioning level
    ALWAYS_INLINE bool ScoringTreeConditioningBox::isAtFinalLevel () const {
      return __final_level;
    }
      

    /// resize the number of children
    ALWAYS_INLINE void 
    ScoringTreeConditioningBox::__resize ( unsigned int new_children_size ) {
      __children.resize ( new_children_size, 0 );
    }

    
    /// sets the final_level Boolean
    ALWAYS_INLINE void
    ScoringTreeConditioningBox::__setFinal ( bool final_level ) {
      __final_level = final_level;
    }

    
  } /* namespace gum */

  
} /* namespace learning */
 

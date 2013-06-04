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
 * are extensively used by learning algorithms: each time some conditional
 * independence tests or local scores are computed by learning algorithms, some
 * tree of these boxes is constructed and the tests or scores are computed
 * by parsing the so-called boxes.
 */

#ifndef GUM_LEARNING_COUNTING_TREE_BOXES_H
#define GUM_LEARNING_COUNTING_TREE_BOXES_H


#include <vector>
#include <agrum/config.h>
#include <agrum/core/list.h>


#define LEARNING_COUNTING_TREE_CONDITIONING_BOX_POOL_MAX_SIZE 100000
#define LEARNING_COUNTING_TREE_TARGET_SET_BOX_POOL_MAX_SIZE   200000
#define LEARNING_COUNTING_TREE_TARGET_BOX_POOL_MAX_SIZE       400000


namespace gum {

  
  namespace learning {


    class CountingTreeConditioningBox;
    class CountingTreeTargetSetBox;
    class CountingTreeTargetBox;
    
    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                   COUNTING TREE CONDITIONING BOX                  === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class CountingTreeConditioningBox
     * @brief This class represents the boxes of the conditioning nodes of the tree
     * of CountingTrees
     * @ingroup learning_group
     * CountingTree converts a Database into a tree with three types of levels:
     * the first one corresponds to the conditioning nodes, i.e., those at the
     * right of conditioning bars in statistical tests, and are represented
     * by CountingTreeConditioningBox; the second type corresponds to a container
     * of target nodes' boxes, which is represented by class
     * CountingTreeTargetSetBox. The third type of levels corresponds to the target
     * nodes' boxes, which are represented by class CountingTreeTargetBox.
     *
     * A CountingTreeConditioningBox just contains pointers to the next tree level.
     * These boxes are not intended to be constructed and destructed by calling
     * their constructors or destructors but rather by calling the createBox and
     * deleteBox methods. Those use their own pool of boxes to speed-up
     * allocations/deallocations. Finally, CountingTreeConditioningBox contain a
     * 'final_level' Boolean indicating whether they are at the final conditioning
     * level, i.e., the next level contains CountingTreeTargetSetBox, or not. This
     * is used to quickly remove all the target box sets without removing
     * conditioning boxes when the number of scores to be computed is too
     * high for all of them to be kept in memory at the same time. 
     */
    /* ========================================================================= */
    class CountingTreeConditioningBox {
    public:
      // ##########################################################################
      /// @name Named Constructors / Destructors
      // ##########################################################################
      /// @{

      /// returns a CountingTreeConditioningBox
      /** To speed-up allocations/deallocations, which are frequent in learning
       * algorithms, the user shall use a pool of boxes: whenever a box is no
       * more useful, it should be put back into the pool. Whenever a box is
       * needed, it shall be taken from the pool. If this one is empty, the box
       * returned is newly allocated into the heap.
       * @param children_size the number of modalities of the variable of the
       * current tree level. This also corresponds to the number of children
       * boxes of the currently created box.
       * @param final_level a Boolean indicating whether the next tree level is
       * a target set box (true) or not.
       * @return a new conditioning box */
      static CountingTreeConditioningBox* createBox ( unsigned int children_size,
                                                     bool final_level );

      /// remove a CountingTreeConditioningBox and all its descendants
      /** If the capacity of the pool is not exceeded, the box is stored into the
       * pool, to be used later on, as well as all its descendants. If the
       * capacity of the pool is exceeded, the box and its descendants are simply
       * removed from memory.
       * @param box the root of the subtree to be deleted or put into the pool */
      static void deleteBox ( CountingTreeConditioningBox* box );

      /// @}
      
      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// reset the children to the empty set
      /** @warning This method removes all the children, if any, by putting them
       * back into the pool (or discarding them from memory if the capacity of
       * the pool is exceeded. */
      void clear ();

      /// returns the set of children
      /** @return the set of children of the box
       * @warning note that, if the current box is at the final level, you
       * will have to translate the returned vector into a vector of
       * CountingTreeTargetSetBox*. To avoid warnings indicating that you break
       * strict aliasing optimizations, you should reinterpret_cast each element
       * of the vector separately as shown in the following code:
       * @code
       * const std::vector<CountingTreeConditioningBox*>& vect = box.children ();
       * for ( unsigned int i = 0; i < vect.size(); ++i ) {
       *   cout << reinterpret_cast<CountingTreeTargetSetBox*> ( vect[i] )
       * }
       * Note that reinterpret_cast do not induce any runtime overhead.
       * @endcode */
      const std::vector<CountingTreeConditioningBox*>& children () const;

      /// indicates whether there exists a child at the ith modality
      /** @param i the ith value of the node
       * @return a Boolean indicating whether the box has a child at its ith
       * modality */
      bool hasChild ( unsigned int i ) const;

      /// returns the ith child
      /** @param i the ith value of the node
       * @return a pointer to the ith child (returns 0 if there is no child)
       * @warning note that, if the current box is at the final level, you
       * will have to translate the returned box into a CountingTreeTargetSetBox*
       * using a reinterpret_cast. */
      CountingTreeConditioningBox* child ( unsigned int i ) const;

      /// sets the ith child
      /** @param i the child will be assigned to the ith value of the node
       * @param box the box to be assigned to the ith modality
       * @warning no test is performed checking whether the type of the box is
       * correct, i.e., whether it is a conditioning box if the current box is not
       * at the final level or a target set box otherwise. */
      void setChild ( unsigned int i, CountingTreeConditioningBox* box );
      void setChild ( unsigned int i, CountingTreeTargetSetBox* box );

      /// indicates whether the box is at the last conditioning level
      /** @return a Boolean indicating whether the box is at the last
       * conditioning level */
      bool isAtFinalLevel () const;
      
      /// @}
      

      // ##########################################################################
      /// @name Static members
      // ##########################################################################
      /// @{

      /// set the maximal capacity (number of boxes) of the pool
      /** if the maximal pool size is decreased and the number of boxes currently
       * stored into the pool is greater than new_size, the boxes in excess are
       * freed from memory.
       * @param new_size the new maximal number of boxes in the pool */
      static void setPoolCapacity ( unsigned int new_size );

      /// clear the pool (free all boxes from memory)
      static void clearPool ();
      
      /// @}

     
    private:
      /// the pointers toward the next level of boxes in the tree
      /** By default, the next level of boxes is supposed to contain conditioning
       * boxes. However, at the last level of conditioning boxes, the __children
       * vector should contain CountingTreeTargetSetBox elements. */
      std::vector<CountingTreeConditioningBox*> __children;

      /// indicates whether this is the last conditioning level in the tree
      bool __final_level;
      
      /// the pool of boxes used for the creations/deletions of boxes
      static gum::ListBase<CountingTreeConditioningBox*> __pool;

      /// the maximal size of the pool (umber of boxes)
      static unsigned int __pool_max_size;

      
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor: use createBox instead of this constructor
      /** When the box is created, it reserves some memory to hold its children
       * but those are not created yet.
       * @param children_size the number of children boxes of the created box at
       * the next level of the counting tree.
       * @param final_level a Boolean indicating whether the created box is at the
       * last level of conditioning boxes (true) or not. If final_level is false,
       * then the next tree level contains target set boxes. */
      CountingTreeConditioningBox ( unsigned int children_size,
                                   bool final_level );

      /// copy constructor: prevent its construction
      CountingTreeConditioningBox ( const CountingTreeConditioningBox& );
      
      /// destructor: use deleteBox instead of this destructor
      /** this destructor removes the box and all its descendants from memory */
      ~CountingTreeConditioningBox ();
     
      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator: prevent the use of this operator
      CountingTreeConditioningBox&
      operator= ( const CountingTreeConditioningBox& );
      
      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// resize the number of children
      /** @warning When down-resizing, we assume that all the children that are
       * removed from the container were already freed from memory.
       * @param new_children_size the number of boxes that can be children of the
       * current box */
      void __resize ( unsigned int new_children_size );

      /// sets the final_level Boolean
      /** @param sets the final level Boolean indicating whether or not the box
       * is at the last conditioning level. */
      void __setFinal ( bool final_level );

      /// @}

    };


    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                      COUNTING TREE TARGET BOX                     === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class CountingTreeTargetBox
     * @brief This class represents the boxes of the target nodes of the tree
     * of CountingTrees
     * @ingroup learning_group
     * CountingTree converts a Database into a tree with three types of levels:
     * the first one corresponds to the conditioning nodes, i.e., those at the
     * right of conditioning bars in statistical tests, and are represented
     * by CountingTreeConditioningBox; the second type corresponds to a container
     * of target nodes' boxes, which is represented by class
     * CountingTreeTargetSetBox. The third type of levels corresponds to the target
     * nodes' boxes, which are represented by class CountingTreeTargetBox.
     *
     * A CountingTreeTargetBox just contains the number of records observed for
     * each value of the target nodes in the database (a target node usually
     * represents either a single Discrete Variable or a pair of Discrete
     * Variables).
     */
    /* ========================================================================= */
    class CountingTreeTargetBox {
    public:
      // ##########################################################################
      /// @name Named Constructors / Destructors
      // ##########################################################################
      /// @{

      /// returns a CountingTreeTargetBox
      /** To speed-up allocations/deallocations, which are frequent in learning
       * algorithms, the user shall use a pool of boxes: whenever a box is no
       * more useful, it should be put back into the pool. Whenever a box is
       * needed, it shall be taken from the pool. If this one is empty, the box
       * returned is newly allocated into the heap.
       * @param size the number of modalities of the target node */
      static CountingTreeTargetBox* createBox ( unsigned int size );

      /// remove a CountingTreeTargetBox
      /** If the capacity of the pool is not exceeded, the box is stored into the
       * pool, to be used later on. If the capacity of the pool is exceeded, the
       * box is simply removed from memory.
       * @param box the box to be put into the pool or removed from memory */
      static void deleteBox ( CountingTreeTargetBox* box );
      
      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// get the number of records found in the database of the ith modality
      /** @param i the modality of the target node */
      unsigned int nbRecords ( unsigned int i ) const;

      /** @brief returns the number of records found in the database for all the
       * modalities */
      const std::vector<unsigned int>& nbRecords () const;

      /// increments the number of records of the ith modality found so far
      /** @param i the modality of the target node */
      void incrementNbRecords ( unsigned int i );

      /// increments the number of records of the ith modality by k
      /** @param i the modality of the target node
       * @param k the number by which we shall increase the number of records */
      void incrementNbRecords ( unsigned int i, unsigned int k );

      /// sets the number of records of the ith modality
      /** @param i the modality of the target node
       * @param k the new number of records of the ith modality */ 
      void setNbRecords ( unsigned int i, unsigned int k );

      /// @}

    
      // ##########################################################################
      /// @name Static members
      // ##########################################################################
      /// @{

      /// set the maximal capacity (number of boxes) of the pool
      /** if the maximal pool size is decreased and the number of boxes currently
       * stored into the pool is greater than new_size, the boxes in excess are
       * freed from memory.
       * @param new_size the new maximal number of boxes in the pool */
      static void setPoolCapacity ( unsigned int new_size );

      /// clear the pool (free all boxes from memory)
      static void clearPool ();
      
      /// @}


    private:
      /// the number of records observed in the database
      std::vector<unsigned int> __nb_records;
      
      /// the pool of boxes
      static gum::ListBase<CountingTreeTargetBox*> __pool;

      /// the maximal size of the pool
      static unsigned int __pool_max_size;


      
      // ##########################################################################
      /// @name Private Constructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param size the domain size of the target node */
      CountingTreeTargetBox ( unsigned int size );

      /// copy constructor: use createBox instead of this constructor
      CountingTreeTargetBox ( const CountingTreeTargetBox& from );
      
      /// destructor: use deleteBox instead of this destructor
      ~CountingTreeTargetBox ();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator: prevent its use by making it private
      CountingTreeTargetBox& operator= ( const CountingTreeTargetBox& );
      
      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// resize the number of values of the target node
      void __resize ( unsigned int new_size );

      /// reset the variable to en empty (no modality) variable
      void __clear ();
      
      /// @}
      

      // target set boxes should be able to delete target boxes when needed
      friend class CountingTreeTargetSetBox;

    };

    
  
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                    COUNTING TREE TARGET SET BOX                   === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class CountingTreeTargetBox
     * @brief This class is a container of boxes of target nodes of the tree
     * of CountingTrees
     * @ingroup learning_group
     * CountingTree converts a Database into a tree with three types of levels:
     * the first one corresponds to the conditioning nodes, i.e., those at the
     * right of conditioning bars in statistical tests, and are represented
     * by CountingTreeConditioningBox; the second type corresponds to a container
     * of target nodes' boxes, which is represented by class
     * CountingTreeTargetSetBox. The third type of levels corresponds to the target
     * nodes' boxes, which are represented by class CountingTreeTargetBox.
     *
     * CountingTreeTargetSetBox is a container of CountingTreeTargetSetBox.
     * Basically, its role is just to enable the simultaneous filling of boxes of
     * several target variables parsing the database only once. The
     * CountingTreeTargetSetBox level in the tree is in between the levels of the
     * conditioning nodes boxes and the level of the target nodes boxes.
     */
    /* ========================================================================= */
    class CountingTreeTargetSetBox {
    public:
      // ##########################################################################
      /// @name Named Constructors / Destructors
      // ##########################################################################
      /// @{

      /// returns a CountingTreeTargetSetBox as well as its target boxes
      /** This method creates a new target set box and fills it with its future
       * target boxes. The latter contain no records. Everything is constructed
       * from pools of boxes.
       * 
       * To speed-up allocations/deallocations, which are frequent in learning
       * algorithms, the user shall use a pool of boxes: whenever a box is no
       * more useful, it should be put back into the pool. Whenever a box is
       * needed, it shall be taken from the pool. If this one is empty, the box
       * returned is newly allocated into the heap.
       * @param variable_modalities indicate the number of modalities (domain
       * size) of each variable that shall be taken into account */
      static CountingTreeTargetSetBox*
      createBox ( const std::vector<unsigned int>& variable_modalities );

      /// removes a CountingTreeTargetSetBox (as well as its own targets)
      /** If the capacity of the pool is not exceeded, the box and its target
       * children are stored into pools, to be used later on. If the capacity of
       * the pool is exceeded, the box and its children are simply removed from
       * memory.
       * @param box the root of the subtree to be deleted or put into the pool */
      static void deleteBox ( CountingTreeTargetSetBox* box );

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the box for the ith variable
      /** @param i the index of the variable whose target box we wish to get */
      CountingTreeTargetBox* child ( unsigned int i ) const;

      /// get the number of records of the ith variable
      /** @param i the index of the variable whose records we wish to get */
      const std::vector<unsigned int>& nbRecords ( unsigned int i ) const;

      /// get the number of records of the jth modality of the ith variable
      /** @param i the index of the variable whose record we wish to get
       * @param j the modality of the record we are interested in */
      unsigned int nbRecords ( unsigned int i, unsigned int j ) const;

      /** @brief increment de the number of observations of the jth value of the
       * ith variable
       * @param i the index of the variable whose record we wish to increment
       * @param j the modality of the record */ 
      void incrementNbRecords ( unsigned int i, unsigned int j ) const;

      /** @brief increment the number of observations of the jth value of the
       * ith variable by k
       * @param i the index of the variable whose record we wish to increment
       * @param j the modality of the record
       * @param k the number by which we increment the record */ 
      void incrementNbRecords ( unsigned int i,
                                unsigned int j, unsigned int k ) const;

      /// sets the number of records of the jth value of the ith variable
      /** @param i the index of the variable whose record we wish to set
       * @param j the modality of the record
       * @param k the number assigned to the record */ 
      void setNbRecords ( unsigned int i, unsigned int j, unsigned int k ) const;
      
      /// get the number of records observed in the database for the parents
      unsigned int nbParentRecords () const;
      
      /// increment the number of observations of the parents of the container
      void incrementNbParentRecords ();
      
      /// increment the number of observations of the parents of the container
      /** @param k the number by which we increment the number of parent records */
      void incrementNbParentRecords ( unsigned int k);
      
      /// empty the container
      /** @param clear_nb_parent_records a Boolean indicating whether, while
       * clearing the children, we must also reset the number of records of the
       * parent variables to 0. */
      void clear ( bool clear_nb_parent_records );

      /// sets a new set of target variables
      /** @warning this method assumes that the target set currently contains no
       * no variable (for instance, we have just executed a clear() ), hence, it
       * does not try to remove any child from memory.
       * @param variable_modalities indicate the number of modalities of each
       * (new) variable that shall be taken into account.
       * @param clear_nb_parent_records a Boolean indicating whether, while
       * clearing the children, we must also reset the number of records of the
       * parent variables to 0. */
      void setVariables ( const std::vector<unsigned int>& variable_modalities,
                          bool clear_nb_parent_records );
  
      /// @}


      // ##########################################################################
      /// @name Static members
      // ##########################################################################
      /// @{

      /// set the maximal capacity (number of boxes) of the pool
      /** if the maximal pool size is decreased and the number of boxes currently
       * stored into the pool is greater than new_size, the boxes in excess are
       * freed from memory.
       * @param new_size the new maximal number of boxes in the pool */
      static void setPoolCapacity ( unsigned int new_size );

      /// clear the pool (free all boxes from memory)
      static void clearPool ();
      
      /// @}


    private:
      /// the set of boxes of the target nodes
      std::vector<CountingTreeTargetBox*> __target_boxes;

      /// the number of records observed for the parents in the database
      unsigned int __nb_parent_records;

      /// the pool of boxes
      static gum::ListBase<CountingTreeTargetSetBox*> __pool;

      /// the maximal size of the pool
      static unsigned int __pool_max_size;


      
      // ##########################################################################
      /// @name Private Constructors
      // ##########################################################################
      /// @{

      /// default constructor: use method createBox instead
      /** This constructor creates a target set and fills it with target boxes
       * of the appropriate size. These boxes contain no record.
       * @param variable_modalities indicate the number of modalities of each
       * variable that shall be taken into account */
      CountingTreeTargetSetBox
      ( const std::vector<unsigned int>& variable_modalities );

      /// copy constructor: prevent its use by making it private
      CountingTreeTargetSetBox ( const CountingTreeTargetSetBox& );

      /// destructor: use deleteBox instead
      ~CountingTreeTargetSetBox ();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator: prevent its use by making it private
      CountingTreeTargetBox& operator= ( const CountingTreeTargetBox& );
      
      /// @}


      // conditioning boxes should be able to delete target set boxes when needed
      friend class CountingTreeConditioningBox;

    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/countingTreeBoxes.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_COUNTING_TREE_BOXES_H */


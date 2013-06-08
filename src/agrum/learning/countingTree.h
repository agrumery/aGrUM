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
 * directly parsing a database. More precisely, CountingTree converts a Database
 * into a tree with three types of levels: the first one corresponds to the
 * conditioning nodes, i.e., those at the right of conditioning bars in
 * statistical tests, and are represented by CountingTreeConditioningBox; the
 * second type of levels corresponds to a container of target nodes' boxes, which
 * is represented by class CountingTreeTargetSetBox. The third type of levels
 * corresponds to the target nodes' boxes, which are represented by class
 * CountingTreeTargetBox.
 * 
 * The class can produce the list of target set boxes of interest. However, it
 * does not compute statistical score by itself.
 */

#ifndef GUM_LEARNING_COUNTING_TREE_H
#define GUM_LEARNING_COUNTING_TREE_H


#include <agrum/config.h>
#include <agrum/core/hashTable.h>
#include <agrum/learning/database.h>
#include <agrum/learning/countingTreeBoxes.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ========================================================================= */
    /* ===                          COUNTING TREE BOX                        === */
    /* ========================================================================= */
    /* ========================================================================= */
    /** @class CountingTree is a class whose aim is to parse a database and
     * compute the list of target set boxes corresponding to a given set of
     * conditioning nodes and target nodes. The latter are specified as singletons
     * and pairs. For instance, if we want to check using a chi2 (or a G2) test
     * whether two nodes X and Y are independent given a set of nodes Z, then
     * we shall compute sum_X sum_Y sum_Z ( #XYZ - #XZ * #YZ / #Z )^2 /
     * (#XZ * #YZ / #Z ), where #XYZ and #XZ correspond to the number of occurences
     * of (X,Y,Z) and (X,Z) respectively in the database. In this case, Z is the
     * set of conditioning nodes, and target nodes are X, Y, and (X,Y); the
     * countings of the target boxes of pair (X,Y) correspond to #XYZ and the
     * countings of single target boxes of X correspond to #XZ. The target set
     * boxes also contain countings #Z. The target boxes of pair (X,Y) contain
     * countings in an array ordered like c[Y][X], i.e., the values of X are
     * consecutive and the values of Y are spaced every |X| elements.
     *
     * The CountingTree class is designed to specify sets of single targets (such
     * as X in the above example) and sets of target pairs. As such, it tries to
     * optimize the parsing of the database to fill all the target boxes of the
     * single and pair targets. TargetSetBoxes are vectors of TargetBoxes, each
     * TargetBox corrsponding to a single target variable or to a pair of target
     * variables (like (X,Y)). As such, we need to know the mapping between the
     * indices in the vectors and the corresponding targets. Those are given
     * by methods targetIndex. Finally, the set of TargetSetBoxes of the tree can
     * be retrieved using method nbRecords (). */
    /* ========================================================================= */
    class CountingTree {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param database the database that will be used to fill the tree */
      CountingTree ( const Database& database );

      /// destructor
      ~CountingTree ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// assign a new set of conditioning and target nodes and compute countings
      /** This method updates the data structure to take into account the new
       * set of nodes. It also parses the database and computes all the target
       * set nodes corresponding to those nodes. When you wish to use these
       * target set boxes, use methods nbRecords and targetIndex. */
      void setNodes
      ( const std::vector<unsigned int>& db_conditioning_ids,
        const std::vector<unsigned int>& db_single_target_ids,
        const std::vector< std::pair<unsigned int,unsigned int> >&
        db_pair_target_ids );

      /// assign a new set of target nodes and compute countings
      /** This method is similar to setNodes except that it does not change the
       * set of conditioning nodes. This can prove useful when memory is not
       * sufficiently large to enable the computation of the target set boxes of
       * all the needed targets in one pass. */
      void setTargetNodes
      ( const std::vector<unsigned int>& db_single_ids,
        const std::vector< std::pair<unsigned int,unsigned int> >& db_pair_ids );

      /// returns the index within target sets of a single target node id
      /** targets are put into target set boxes at locations convenient for the
       * counting tree. When you wish to get the target box corresponding to a
       * given target node, you need to know this location and method targetIndex
       * precisely gives you this information: just pass to it the id of your
       * target in the database and it will return the location of the target box
       * within target set boxes.
       * @warning This method should be used after setNodes or setTargetNodes
       * have been executed. As such, if the target id was not set by the
       * aforementioned methods, an exception is raised. */
      unsigned int targetIndex ( unsigned int db_target_id ) const;
        
      /// returns the index within target sets of a pair of target nodes
      /** targets are put into target set boxes at locations convenient for the
       * counting tree. When you wish to get the target box corresponding to a
       * given pair of target nodes, you need to know this location and method
       * targetIndex precisely gives you this information: just pass to it the
       * pair of ids of your targets in the database and it will return the
       * location of the target box within target set boxes.
       * @warning This method should be used after setNodes or setTargetNodes
       * have been executed. As such, if the pair of target ids was not set by
       * the aforementioned methods, an exception is raised.
       * @warning The method assumes that the pair is ordered, i.e., pair
       * (X,Y) is supposed to be different from pair (Y,X). So, when calling this
       * method, be carefull of specifying the pair in the correct order. */
       unsigned int targetIndex ( const std::pair<unsigned int,unsigned int>&
                                  db_target_id ) const;

      /// returns the list of target set boxes generated by parsing the database
      const ListBase<CountingTreeTargetSetBox*>& nbRecords () const;

      /// clear the whole tree
      void clear ();
        
      /// @}

      

    private:
      /// the database we shall parse
      const Database* __database;

      /// a vector indicating the set of conditioning ids (in the database)
      const std::vector<unsigned int>* __db_conditioning_ids;

      /// a vector containing the db single target ids
      const std::vector<unsigned int>* __db_single_target_ids;

      /// a vector containing the db pair of target ids
      const std::vector< std::pair<unsigned int,unsigned int> >*
      __db_pair_target_ids;

      /// the index in the target sets of a given db node id
      HashTable<unsigned int,unsigned int> __dbSingle2target;

      /// the index in the target sets of a given db pair of nodes
      HashTable<std::pair<unsigned int,unsigned int>,unsigned int> __dbPair2target;
        
      /// a vector containing the sizes of each target box (pair and single nodes)
      /** the vector first contains the modalities (domain sizes) of the pair
       * ordered similarly to __db_pair_target_ids, and then those of the single
       * nodes ordered similarly to __db_single_ids */
      std::vector<unsigned int> __target_modalities;
        
      /// the tree itself
      union Root {
        CountingTreeConditioningBox* Conditioning;
        CountingTreeTargetSetBox*    TargetSet;
      } __root;

      /// the list of all target set boxes created by parsing the database
      ListBase<CountingTreeTargetSetBox*> __target_records;


        
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// prevent copy constructor
      CountingTree ( const CountingTree& );
        
      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// prevent copy operator
      CountingTree& operator= ( const CountingTree& );
        
      /// @}

        
      /// parse one database record to fill a given target set box
      void __fillTargetSetBox ( CountingTreeTargetSetBox* box,
                                const DatabaseIterator& iter );

      /// traverse the conditional nodes of the tree corresponding to one db record
      /** this method starts from the root of a conditional tree and traverses
       * the tree, constructing any conditional or target set box needed, until
       * all conditioning nodes have been parsed */
      CountingTreeTargetSetBox*
      __fillConditioningBoxes ( const DatabaseIterator& iter );
        
      /** @brief fill a whole tree by parsing the complete database when there
       * are no conditional nodes
       *
       * This method creates from scratch a new tree containing only pair targets
       * and fills it by parsing the whole database. When the method completes,
       * the __target_records field contains all the countings for target pairs.*/
      void __fillUnconditionalPairTree ();

      /** @brief fill a whole tree by parsing the complete database when there
       * are conditional nodes
       *
       * This method creates from scratch a new tree containing only conditioning
       * nodes and pair targets, and it fills it by parsing the whole database.
       * When the method completes, the __target_records field contains all the
       * countings for target pairs. */      
      void __fillConditionalPairTree ();

      /// fill the single targets
      /** assuming that the tree for all conditioning and target pairs has
       * been successfully created, this method creates the countings for all
       * the single target nodes. To do so, it just summarizes the countings
       * obtained for the pairs of target nodes. */
      void __fillSingleTargetTree ();

    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/countingTree.inl>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_COUNTING_TREE_H */


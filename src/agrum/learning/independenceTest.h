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
 * @brief the abstract class for all the independence tests
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_INDEPENDENCE_TEST_H
#define GUM_LEARNING_INDEPENDENCE_TEST_H


#include <agrum/learning/basicScore.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ===                      INDEPENDENCE TEST CLASS                      === */
    /* ========================================================================= */
    /** @class IndependenceTest the abstract class for all the independence tests
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so,
     * use the appropriate addScores methods. These will compute everything you
     * need. The addScores methods where you do not specify a set of conditioning
     * nodes assume that this set is empty. Once the computations have been
     * performed, use methods score to retrieve the scores computed. */
    template <typename RowFilter,
              typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class IndependenceTest : private BasicScore<RowFilter,IdSetAlloc,CountAlloc> {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{
      
      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      IndependenceTest ( const RowFilter& filter,
                         const std::vector<unsigned int>& var_modalities );
      
      /// destructor
      virtual ~IndependenceTest ();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new target X conditioned by Y to be counted
      /** The function stores #XY as a target count and #Y as a conditioning count
       * at the index it returns and #X at the target count of the next index. 
       * @param X represents the index of the target variable in the
       * filtered rows produced by the database cell filters
       * @param Y represents the index of the conditioning variable in the
       * filtered rows produced by the database cell filters
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to method
       * countTarget to get the corresponding counting vector. */
      unsigned int addNodeSet ( unsigned int X,
                                unsigned int Y );

      /// add a new target vars.first conditioned by vars.second to be counted
      /** @param vars represents the index of the pairs (target variable,
       * conditioning variable) in the filtered rows produced by the database
       * cell filters
       * @return the index of the produced counting vector. If X denotes vars.first
       * and Y denotes vars.second, then the function stores #XY as a target count
       * and #Y as a conditioning count at the index it returns and #X at the
       * target count of the next index. 
       * The user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to method
       * countTarget to get the corresponding counting vector. */
      unsigned int addNodeSet ( const std::pair<unsigned int,unsigned int>& vars );

      /// add a new pair of target conditioned variables to be counted
      /** The function stores #ZXY as a target count and #ZY as a conditioning
       * count at the index it returns and #ZX as the target count and #Z as the
       * conditioning count of the next index.
       * @param X represents the index of the first target variable in the
       * filtered rows produced by the database cell filters
       * @param Y represents the index of the second target variable in the
       * filtered rows produced by the database cell filters 
       * @param Z the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * countTarget and countConditioning to get the corresponding counting
       * vectors. The former contains the observed countings for the set of
       * var and conditioning_ids whereas the countConditioning returns the
       * observed countings only for the conditioning_ids. */
      unsigned int
      addNodeSet ( unsigned int X,
                   unsigned int Y,
                   const std::vector<unsigned int>& Z );

      /// add a new pair of target conditioned variables to be counted
      /** If we denote by X and Y the first and second fields of vars respectively,
       * the function stores #ZXY as a target count and #ZY as a conditioning
       * count at the index it returns and #ZX as the target count and #Z as the
       * conditioning count of the next index.
       * @param vars represents the index of the target variables in the
       * filtered rows produced by the database cell filters
       * @param Z the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to methods
       * countTarget and countConditioning to get the corresponding counting
       * vectors. The former contains the observed countings for the set of
       * var and conditioning_ids whereas the countConditioning returns the
       * observed countings only for the conditioning_ids. */
      unsigned int
      addNodeSet ( const std::pair<unsigned int,unsigned int>& vars,
                   const std::vector<unsigned int>& Z );

      /// add new set of "unconditioned" pairs of targets
      /** This method is a shortcut for the application of addNodeSet on each
       * variable in vector vars.
       * @param vars represents the indices of the target variables in the
       * filtered rows produced by the database cell filters
       * @return the index of the first produced counting vector: the user should
       * use class BasicScore to compute in one pass several scores. These and
       * their corresponding countings in the database are stored into a vector
       * and the value returned by method addNodeSet is the index of the counts
       * for the first variable of vars in this vector. The user shall pass this
       * index as argument to method countTarget to get the corresponding counting
       * vector. The other counting vectors follow the first one in the vector of
       * counting vectors (i.e., their indices follow that of the first var). */
      unsigned int addNodeSets
      ( const std::vector< std::pair<unsigned int,unsigned int> >& vars );

      /// add new set of "conditioned" pairs of targets
      /** This method is a shortcut for the application of addNodeSet on each
       * variable in vector vars.
       * @param vars represents the indices of the target variables in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the first produced counting vector: the user should
       * use class BasicScore to compute in one pass several scores. These and
       * their corresponding countings in the database are stored into a vector
       * and the value returned by method addNodeSet is the index of the counts
       * for the first variable of vars in this vector. The user shall pass this
       * index as argument to method countTarget to get the corresponding counting
       * vector. The other counting vectors follow the first one in the vector of
       * counting vectors (i.e., their indices follow that of the first var). */
      unsigned int addNodeSets
        ( const std::vector< std::pair<unsigned int,unsigned int> >& vars,
          const std::vector<unsigned int>& conditioning_ids );

      /// clears all the data structures from memory
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::clear;

      /// returns the modalities of the variables
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::modalities;

      /// returns the score corresponding to a given nodeset
      virtual float score ( unsigned int nodeset_index ) = 0;

      /// @}


    protected:

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observtion countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSets. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method countConditioning.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes.
       * @warning it is assumed that, after using addNodeSet, you have executed
       * method count() before calling method countTarget. */
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::_getAllCounts;

      /// returns the counting vector for a conditioning set
      /** @warning it is assumed that, after using addNodeSet, you have executed
       * method count() before calling method countTarget. */
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::_getConditioningCounts;

      /// returns the set of target + conditioning nodes
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::_getAllNodes;

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      using BasicScore<RowFilter,IdSetAlloc,CountAlloc>::_getConditioningNodes;

      
      /// 1 / log(2)
      const float _1log2 { M_LOG2E };

     
      // ##########################################################################
      // ##########################################################################
      
      /// prevent copy constructor
      IndependenceTest ( const IndependenceTest& ) = delete;

      /// prevent copy operator
      IndependenceTest& operator= ( const IndependenceTest& ) = delete;

    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the template implementation
#include <agrum/learning/independenceTest.tcc>


#endif /* GUM_LEARNING_INDEPENDENCE_TEST_H */

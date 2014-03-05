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
 * @brief the base class for all the scores used for learning (BIC, BDeu, etc) as
 * well as all the independence tests
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_LEARNING_BASIC_SCORE_H
#define GUM_LEARNING_BASIC_SCORE_H


#include <vector>
#include <agrum/config.h>
#include <agrum/learning/recordCounter.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ===                         BASIC SCORE CLASS                         === */
    /* ========================================================================= */
    /** @class BasicScore The base class for all the scores used for learning
     * (BIC, BDeu, etc)
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
    class BasicScore {
    public:

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      BasicScore ( const RowFilter& filter,
                   const std::vector<unsigned int>& var_modalities );

      /// destructor
      virtual ~BasicScore ();

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new single variable to be counted
      /** @param var represents the index of the variable in the filtered rows
       * produced by the database cell filters whose observations shall be counted
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to method
       * countTarget to get the corresponding counting vector. */
      unsigned int addNodeSet ( unsigned int var );

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
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
      addNodeSet ( unsigned int var,
                   const std::vector<unsigned int>& conditioning_ids );

      /// add a new target (var1) conditioned by var2 to be counted
      /** @param var1 represents the index of the first target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the conditioning variable in the
       * filtered rows produced by the database cell filters
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to method
       * countTarget to get the corresponding counting vector. */
      unsigned int addNodeSet ( unsigned int var1,
                                unsigned int var2 );

      /// add a target (vars.first) conditioned by vars.second to be counted
      /** @param vars represents the index of the target variables in the
       * filtered rows produced by the database cell filters
       * @return the index of the produced counting vector: the user should use
       * class BasicScore to compute in one pass several scores. These and their
       * corresponding countings in the database are stored into a vector and the
       * value returned by method addNodeSet is the index of the counts in
       * this vector. The user shall pass this index as argument to method
       * countTarget to get the corresponding counting vector. */
      unsigned int addNodeSet ( const std::pair<unsigned int,unsigned int>& vars );

      /// add a target conditioned by variables to be counted
      /** @param var1 represents the index of the first target variable in the
       * filtered rows produced by the database cell filters
       * @param var2 represents the index of the second target variable in the
       * filtered rows produced by the database cell filters 
       * @param conditioning_ids the indices of the variables of the conditioning
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
      addNodeSet ( unsigned int var1,
                   unsigned int var2,
                   const std::vector<unsigned int>& conditioning_ids );

      /// add a new pair of target conditioned variables to be counted
      /** @param vars represents the index of the target variables in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
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
                   const std::vector<unsigned int>& conditioning_ids );

      /// add new set of "unconditioned" single targets
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
      unsigned int
      addNodeSets ( const std::vector<unsigned int>& single_vars );

      /// add new set of "conditioned" single targets
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
      unsigned int
      addNodeSets ( const std::vector<unsigned int>& single_vars,
                    const std::vector<unsigned int>& conditioning_ids );

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
      void clear ();

      /// returns the score corresponding to a given nodeset
      virtual float score ( unsigned int nodeset_index ) = 0;

      /// returns the modalities of the variables
      const std::vector<unsigned int>& modalities () const noexcept;

      /// @}

      
    protected:
      /// the modalities of the variables
      const std::vector<unsigned int>& _modalities;

      /// returns the counting vector for a given (conditioned) target set
      /** This method returns the observtion countings for the set of variables
       * whose index was returned by method addNodeSet or addNodeSets. If the
       * set was conditioned, the countings correspond to the target variables
       * @b and the conditioning variables. If you wish to get only the countings
       * for the conditioning variables, prefer using method countConditioning.
       * @warning the dimensions of the vector are as follows: first come the
       * nodes of the conditioning set (in the order in which they were specified
       * when callind addNodeset, and then the target nodes.
       * @warning whenever you call this function, if the counts have not been
       * computed yet, they are computed before the function returns. */
      const std::vector<float,CountAlloc>&
      _getAllCounts ( unsigned int index );

      /// returns the counting vector for a conditioning set
      /** @warning whenever you call this function, if the counts have not been
       * computed yet, they are computed before the function returns. */
      const std::vector<float,CountAlloc>&
      _getConditioningCounts ( unsigned int index );

      /// returns the set of target + conditioning nodes
      /** conditioning nodes are always the first ones in the vector and targets
       * are the last ones */
      const std::vector<unsigned int,IdSetAlloc>&
      _getAllNodes ( unsigned int index ) const noexcept;

      /// returns the conditioning nodes (nullptr if there are no such nodes)
      const std::vector<unsigned int,IdSetAlloc>*
      _getConditioningNodes ( unsigned int index ) const noexcept;

      
      /// 1 / log(2)
      const float _1log2 { M_LOG2E };

      

    private:
      /// the recordCounter that will parse the database
      RecordCounter<RowFilter,IdSetAlloc,CountAlloc> __record_counter;
      
      /// the target id sets to count and their indices in the record counter
      std::vector< std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* >
      __target_nodesets;

      /// the conditioning id sets to count and their indices in the record counter
      std::vector< std::pair<std::vector<unsigned int,IdSetAlloc>,unsigned int>* >
      __conditioning_nodesets;

      /// indicates whether we have already computed the countings of the nodesets
      bool __counts_computed { false };
      

      /// perform the computation of the countings
      void __count ();

      
      // ##########################################################################
      // ##########################################################################

      /// prevent copy constructor
      BasicScore ( const BasicScore<RowFilter,IdSetAlloc,CountAlloc>& ) = delete;

      /// prevent copy operator
      BasicScore<RowFilter,IdSetAlloc,CountAlloc>&
      operator= ( const BasicScore<RowFilter,IdSetAlloc,CountAlloc>& ) = delete;

    };


  } /* namespace learning */
  
  
} /* namespace gum */


/// include the template implementation
#include <agrum/learning/basicScore.tcc>


#endif /* GUM_LEARNING_BASIC_SCORE_H */

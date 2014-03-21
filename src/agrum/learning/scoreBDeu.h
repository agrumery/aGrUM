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
 * @brief the class for computing BDeu scores (actually their log2 value)
 *
 * @warning This class does not actually compute a BDeu score but rather the
 * log in base 2 of the BDeu score
 *
 * The class should be used as follows: first, to speed-up computations, you
 * should consider computing all the scores you need in one pass. To do so, use
 * the appropriate addNodeSets methods. These will compute everything you need.
 * Use methods score to retrieve the scores computed. See the Score class for
 * details.
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */


#ifndef GUM_LEARNING_SCORE_K2_H
#define GUM_LEARNING_SCORE_K2_H


#include <agrum/core/math/gammaLog2.h>
#include <agrum/learning/score.h>


namespace gum {

  
  namespace learning {

    
    /* ========================================================================= */
    /* ===                         SCORE BDeu CLASS                          === */
    /* ========================================================================= */
    /** @class ScoreBDeu
     * @ingroup learning_group
     * @brief The class for computing BDeu scores (actually their log2 value)
     *
     * @warning This class does not actually compute a BDeu score but rather the
     * log in base 2 of the BDeu score
     *
     * The class should be used as follows: first, to speed-up computations, you
     * should consider computing all the scores you need in one pass. To do so, use
     * the appropriate addNodeSets methods. These will compute everything you need.
     * Use methods score to retrieve the scores computed. See the Score class for
     * details.
     */
    template <typename RowFilter,
              typename IdSetAlloc = std::allocator<unsigned int>,
              typename CountAlloc = std::allocator<float> >
    class ScoreBDeu : protected Score<RowFilter,IdSetAlloc,CountAlloc> {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database */
      ScoreBDeu ( const RowFilter& filter,
                  const std::vector<unsigned int>& var_modalities );

      /// destructor
      virtual ~ScoreBDeu ();
      
      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// add a new single variable to be counted
      /** @param var represents the index of the variable in the filtered rows
       * produced by the database cell filters whose observations shall be counted
       * @return the index of the produced counting vector: the user should use
       * class Score to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are stored
       * into a vector and the value returned by method addNodeSet is the index of
       * the observed countings of "var" in this vector. The user shall pass this
       * index as argument to methods _getAllCounts to get the corresponding
       * counting vector. */
      unsigned int addNodeSet ( unsigned int var,
                                float equivalent_sample_size = 1 );

      /// add new set of "unconditioned" single targets
      /** This method is a shortcut for the application of addNodeSet on each
       * variable in vector vars.
       * @param vars represents the indices of the target variables in the
       * filtered rows produced by the database cell filters
       * @return the index of the first produced counting vector: the user should
       * use class Score to compute in one pass several scores. These and
       * their corresponding countings in the database are stored into a vector
       * and the value returned by method addNodeSets is the index of the counts
       * for the first variable of vars in this vector. The user shall pass this
       * index as argument to method _getAllCounts to get the corresponding
       * counting vector. The other counting vectors follow the first one in the
       * vector of counting vectors (i.e., their indices follow that of the
       * first var). */
      unsigned int
      addNodeSets ( const std::vector<unsigned int>& single_vars,
                    float equivalent_sample_size = 1 );

      /// add a new target variable plus some conditioning vars
      /** @param var represents the index of the target variable in the filtered
       * rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the produced counting vector: the user should use
       * class Score to compute in one pass several scores or independence
       * tests. These and their corresponding countings in the database are
       * stored into a vector and the value returned by method addNodeSet is the
       * index of the countings of (var | conditioning_ids) in this vector. The
       * user shall pass this index as argument to methods _getAllCounts and
       * _getConditioningCounts to get the counting vectors of
       * (conditioning_ids,vars) [in this order] and conditioning_ids
       * respectively. */
      unsigned int
      addNodeSet ( unsigned int var,
                   const std::vector<unsigned int>& conditioning_ids,
                   float equivalent_sample_size = 1 );

      /// add new set of target variables conditioned by the same variables
      /** This method is a shortcut for the application of addNodeSet on each
       * variable in vector vars conditioned by conditioning_ids.
       * @param vars represents the indices of the target variables in the
       * filtered rows produced by the database cell filters
       * @param conditioning_ids the indices of the variables of the conditioning
       * set in the filtered rows
       * @return the index of the first produced counting vector: the user should
       * use class Score to compute in one pass several scores. These and
       * their corresponding countings in the database are stored into a vector
       * and the value returned by method addNodeSets is the index of the counts
       * for the first variable of vars in this vector. The user shall pass this
       * index as argument to methods _getAllCounts and _getConditioningCounts
       * to get the countings of ( conditioning_ids,vars ) [in this order] and
       * conditioning_ids respectively, where var corresponds to the target
       * variable. */
      unsigned int
      addNodeSets ( const std::vector<unsigned int>& vars,
                    const std::vector<unsigned int>& conditioning_ids,
                    float equivalent_sample_size = 1 );

      /// clears all the data structures from memory
      void clear ();

      /// returns the modalities of the variables
      using Score<RowFilter,IdSetAlloc,CountAlloc>::modalities;
      
      /// returns the log2(BDeu score) corresponding to a given nodeset
      float score ( unsigned int nodeset_index );

      /// @}

      
    private:
      /// the log(gamma (n)) function: generalizes log((n-1)!)
      GammaLog2 __gammalog2;

      /// the equivalent sample sizes used for the scores of each nodeset
      std::vector<float> __ess;


      /// save an equivalent sample size
      void __insertESS ( float equivalent_sample_size,
                         unsigned int index );
      
    };
    

  } /* namespace learning */
  
  
} /* namespace gum */


/// always include the template implementation
#include <agrum/learning/scoreBDeu.tcc>


#endif /* GUM_LEARNING_SCORE_BDeu_H */

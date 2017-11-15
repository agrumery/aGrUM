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
 #ifndef GUM_LEARNING_PARTIALENTROPY_H
 #define GUM_LEARNING_PARTIALENTROPY_H

 #include <vector>
#include <agrum/config.h>
#include <agrum/core/math/math.h>

#include <agrum/learning/scores_and_tests/cache4PartEntropy.h>
 #include <agrum/learning/scores_and_tests/counter.h>

 namespace gum {

   namespace learning {

     /* =========================================================================
      */
     /* ===                      PARTIAL ENTROPY CLASS                        ===
      */
     /* =========================================================================
      */
     template <typename IdSetAlloc = std::allocator<Idx>,
               typename CountAlloc = std::allocator<double>>
     class PartialEntropy : public Counter<IdSetAlloc, CountAlloc> {
       public:
       // ##########################################################################
       // ##########################################################################


       template <typename RowFilter>
       PartialEntropy( const RowFilter&         filter,
                      const std::vector<Size>& var_modalities,
			             Size min_range = 0,
			             Size max_range = std::numeric_limits<Size>::max() );

       ~PartialEntropy();


       // ##########################################################################
       // ##########################################################################

       Idx addNodeSet( const std::vector<Idx>& var_set );

       Idx addNodeSet( Idx var );

       Idx addNodeSet( Idx var1, Idx var2 );

       /// clears all the data structures from memory
       void clear();

       /// clears the current cache (clear nodesets as well)
       void clearCache();

       /// turn on/off the use of a cache of the previously computed score
       void useCache( bool on_off ) noexcept;
       

       /// returns the score corresponding to a given nodeset
       double score( Idx nodeset_index );

       /// sets the range of records taken into account by the counter
       /** @param min_range he number of the first record to be taken into
        * account during learning
        * @param max_range the number of the record after the last one taken
        * into account*/
       void setRange( Size min_range, Size max_range );

       ///return the size of the database
       const Size _N();

       protected:
       /// 1 / log(2)
       const double _1log2{M_LOG2E};
       
       /// returns the counting vector for a given (conditioned) target set
       /** This method returns the observation countings for the set of variables
        * whose index was returned by method addNodeSet or addNodeSet. If the
        * set was conditioned, the countings correspond to the target variables
        * @b and the conditioning variables. If you wish to get only the
        * countings
        * for the conditioning variables, prefer using method
        * _getConditioningCounts.
        * @warning the dimensions of the vector are as follows: first come the
        * nodes of the conditioning set (in the order in which they were
        * specified
        * when callind addNodeset, and then the target nodes. */
       using Counter<IdSetAlloc, CountAlloc>::_getAllCounts;

       /// returns the counting vector for a conditioning set
       /** see method _getAllCounts for details */
       using Counter<IdSetAlloc, CountAlloc>::_getConditioningCounts;

       /// returns the set of target + conditioning nodes
       /** conditioning nodes are always the first ones in the vector and targets
        * are the last ones */
       using Counter<IdSetAlloc, CountAlloc>::_getAllNodes;

       /// returns the conditioning nodes (nullptr if there are no such nodes)
       using Counter<IdSetAlloc, CountAlloc>::_getConditioningNodes;

       /// indicates whether a score belongs to the cache
       bool _isInCache( Idx nodeset_index ) const noexcept;

       /// inserts a new score into the cache
       void _insertIntoCache( Idx nodeset_index, double score );

       /// returns a cached score
       double _cachedScore( Idx nodeset_index ) const noexcept;

       /// indicates whether we use the cache or not
       bool _isUsingCache() const noexcept;
       
       private:

       ///Size of the database
       Size __N = 0;

       /// a cache for the previously computed scores
       Cache4PartEntropy __cache;

       /// a Boolean indicating whether we wish to use the cache
       bool __use_cache{true};

       /// indicates whether the ith nodeset's score is in the cache or not
       std::vector<bool> __is_cached_score;

       /// the vector of scores for the current nodesets
       std::vector<double> __cached_score;

       /// an empty conditioning set
       const std::vector<Idx, IdSetAlloc> __empty_conditioning_set;
     };

   } /* namespace learning */

 } /* namespace gum */


 extern template class gum::learning::PartialEntropy<>;


 // always include the template implementation
 #include <agrum/learning/scores_and_tests/partialEntropy_tpl.h>

 #endif /* GUM_LEARNING_PARTIALENTROPY_H */

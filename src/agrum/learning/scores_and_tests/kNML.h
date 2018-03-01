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
/**
 * @file
 * @brief The class for the NML corrections used in 3off2
 *
 * To accomodate for finite datasets, corrections for the mutual information are
 * introduced by the authors. The NML correction is one of them, and they claim it
 * to be the most effective
 *
 * @author Quentin FALCAND
 */

#ifndef GUM_LEARNING_K_NML_H
#define GUM_LEARNING_K_NML_H

#include <vector>

#include <agrum/core/math/math.h>
#include <agrum/learning/scores_and_tests/cache4PartEntropy.h>
#include <agrum/learning/scores_and_tests/independenceTest.h>

namespace gum {

  namespace learning {

    /* =========================================================================
     */
    /* ===                      INDEP TEST CHI2 CLASS                        ===
     */
    /* =========================================================================
     */
    /** @class KNML
     * @brief the class for computing Chi2 independence test scores
     * @ingroup learning_group
     *
     */
    template < typename IdSetAlloc = std::allocator< Idx >,
               typename CountAlloc = std::allocator< double > >
    class KNML : public IndependenceTest< IdSetAlloc, CountAlloc > {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param filter the row filter that will be used to read the database
       * @param var_modalities the domain sizes of the variables in the database
       */
      template < typename RowFilter >
      KNML(const RowFilter& filter, const std::vector< Size >& var_modalities);

      /// destructor
      ~KNML();

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// returns the score corresponding to a given nodeset
      double score(Idx nodeset_index);

      /// @}

      protected:
      /// Computing the universal normalization constant, intermediary computation
      double _C(Size r, Size n);

      /// inserts a new score into the cache for C
      /// @todo remove exception driven programation
      void _insertIntoCCache(Size r, Size n, double c);

      private:
      /// an empty vector of ids
      const std::vector< Idx, IdSetAlloc > __empty_set;

      /// pre-computed values of C for r=2 and 0<n<=1000
      static const std::vector< double > __Cvec;

      /// cache for the values of C
      Cache4PartEntropy __cache_C;

      /// a Boolean indicating whether we wish to use the cache for C
      bool __use_cache_C{true};
    };

  } /* namespace learning */

} /* namespace gum */


extern template class gum::learning::KNML<>;


// always include the template implementation
#include <agrum/learning/scores_and_tests/kNML_tpl.h>

#endif /* GUM_LEARNING_K_NML_H */

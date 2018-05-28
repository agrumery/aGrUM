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
 * @brief class CorrectedMutualInformation, used in the 3off2 algorithm
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN.
 */
#ifndef GUM_LEARNING_CORRECTEDMUTUALINFORMATION_H
#define GUM_LEARNING_CORRECTEDMUTUALINFORMATION_H

#include <agrum/config.h>
#include <agrum/core/math/math.h>
#include <vector>

#include <agrum/learning/scores_and_tests/cache43ptInfo.h>
#include <agrum/learning/scores_and_tests/cache4IndepTest.h>
#include <agrum/learning/scores_and_tests/kNML.h>
#include <agrum/learning/scores_and_tests/partialEntropy.h>

namespace gum {

  namespace learning {

    /* =========================================================================
     */
    /* ===          2/3-POINTS CORRECTED MUTUAL INFORMATION CLASS            ===
     */
    /* =========================================================================
     */
    /**
     * @class CorrectedMutualInformation
     * @brief class CorrectedMutualInformation, used in the 3off2 algorithm
     *
     * This class handles the computations and storage of the mutual information
     * values used in 3off2 and potential corrections.
     *
     * @ingroup learning_group
     */
    template < typename IdSetAlloc = std::allocator< Idx >,
               typename CountAlloc = std::allocator< double > >
    class CorrectedMutualInformation {
      public:
      // ##########################################################################
      /// @name Constructors and destructors
      // ##########################################################################

      /// @{
      template < typename RowFilter >
      CorrectedMutualInformation(const RowFilter&           filter,
                                 const std::vector< Size >& var_modalities);

      virtual ~CorrectedMutualInformation();
      /// @}


      // ##########################################################################
      // Public member functions
      // ##########################################################################

      /// @name functions for cache
      /// @{
      /// clears all the data structures from memory
      virtual void clear();

      /// clears the current cache (clear nodesets as well)
      virtual void clearCache();

      /// turn on/off the use of a cache of the previously computed K score
      void useKCache(bool on_off) noexcept;

      /// turn on/off the use of a cache of the previously computed score
      void useCache(bool on_off) noexcept;
      /// @}

      /// @name score function, used to declare the variables
      /// @{
      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(Idx var1, Idx var2);

      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids);

      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(const std::pair< Idx, Idx >& vars);

      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(const std::pair< Idx, Idx >& vars,
                   const std::vector< Idx >&    conditioning_ids);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(Idx var1, Idx var2, Idx var3);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(const std::tuple< Idx, Idx, Idx >& vars);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(Idx                       var1,
                   Idx                       var2,
                   Idx                       var3,
                   const std::vector< Idx >& conditioning_ids);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(const std::tuple< Idx, Idx, Idx >& vars,
                   const std::vector< Idx >&          conditioning_ids);
      /// @}

      /// Size of the database
      const Size N();

      /// @name functions to choose which correction to use
      /// @{
      void useMDL();
      void useNML();
      void useNoCorr();
      /// @}


      /// the description type for the complexity correction
      enum class KModeTypes { MDL, NML, NoCorr };

      protected:
      /// returns the 2-point mutual information corresponding to a given nodeset
      virtual double
        _I_score(Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids);

      /// returns the 3-point mutual information corresponding to a given nodeset
      virtual double _I_score(Idx                       var1,
                              Idx                       var2,
                              Idx                       var3,
                              const std::vector< Idx >& conditioning_ids);

      /// returns the 2-point corrected mutual information corresponding to a given
      /// nodeset
      virtual double
        _score(Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids);

      /// returns the 3-point corrected mutual information corresponding to a given
      /// nodeset
      virtual double _score(Idx                       var1,
                            Idx                       var2,
                            Idx                       var3,
                            const std::vector< Idx >& conditioning_ids);

      // the cache for partial entropy computations
      PartialEntropy< IdSetAlloc, CountAlloc > _H;

      /// an empty conditioning set
      const std::vector< Idx > __empty_conditioning_set;

      /// inserts a new score into the cache
      void _insertIntoCache(Idx                       var1,
                            Idx                       var2,
                            const std::vector< Idx >& conditioning_ids,
                            double                    score);

      /// inserts a new score into the cache
      void _insertIntoCache(Idx                       var1,
                            Idx                       var2,
                            Idx                       var3,
                            const std::vector< Idx >& conditioning_ids,
                            double                    score);

      /// indicates whether we use the cache or not
      bool _isUsingCache() const noexcept;

      /// computes the complexity correction for the mutual information
      double _K(Idx var1, Idx var2, const std::vector< Idx >& conditioning_ids);
      double _K(Idx                       var1,
                Idx                       var2,
                Idx                       var3,
                const std::vector< Idx >& conditioning_ids);


      /// inserts a new score into the cache
      void _insertIntoKCache(Idx                       var1,
                             Idx                       var2,
                             const std::vector< Idx >& conditioning_ids,
                             double                    score);

      /// inserts a new score into the cache
      void _insertIntoKCache(Idx                       var1,
                             Idx                       var2,
                             Idx                       var3,
                             const std::vector< Idx >& conditioning_ids,
                             double                    score);

      /// a Boolean indicating whether we wish to use the cache
      bool __use_cache{true};

      /// a Boolean indicating whether we wish to use the Kcache
      bool __use_Kcache{true};

      private:
      /// a cache for the previously computed scores
      Cache4IndepTest __cache_2pt;
      Cache43ptInfo   __cache_3pt;

      /// a cache for the previously computed K scores
      Cache4IndepTest __cache_K2pt;
      Cache43ptInfo   __cache_K3pt;

      /// the object computing the NML k score
      KNML< IdSetAlloc, CountAlloc > __k_NML;


      /// the mode used for the correction
      KModeTypes __kmode{KModeTypes::MDL};

      /// variables modalities
      std::vector< Size > __modalities;

      const double __threshold{1e-10};
    };

  } /* namespace learning */

} /* namespace gum */


extern template class gum::learning::CorrectedMutualInformation<>;


// always include the template implementation
#include <agrum/learning/scores_and_tests/correctedMutualInformation_tpl.h>

#endif /* GUM_LEARNING_CORRECTEDMUTUALINFORMATION_H */

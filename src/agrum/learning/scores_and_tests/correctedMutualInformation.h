
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief The class computing n times the corrected mutual information,
 * as used in the 3off2 algorithm
 *
 * @author Quentin FALCAND, Christophe GONZALES and Pierre-Henri WUILLEMIN.
 */
#ifndef GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H
#define GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H

#include <agrum/config.h>
#include <agrum/core/math/math.h>
#include <vector>

#include <agrum/learning/scores_and_tests/kNML.h>
#include <agrum/learning/scores_and_tests/scoreLog2Likelihood.h>
#include <agrum/learning/scores_and_tests/scoreMDL.h>

namespace gum {

  namespace learning {

    /**
     * @class CorrectedMutualInformation
     * @brief The class computing n times the corrected mutual information,
     * as used in the 3off2 algorithm
     *
     * This class handles the computations and storage of the mutual information
     * values used in 3off2 and potential corrections.
     *
     * @ingroup learning_scores
     */
    template < template < typename > class ALLOC = std::allocator >
    class CorrectedMutualInformation {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< NodeId >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to countings in the contingency tables
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database.
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      CorrectedMutualInformation(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              apriori,
         const std::vector< std::pair< std::size_t, std::size_t >,
                            ALLOC< std::pair< std::size_t, std::size_t > > >&
            ranges,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param apriori An apriori that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to countings in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @param alloc the allocator used to allocate the structures within the
       * Score.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      CorrectedMutualInformation(
         const DBRowGeneratorParser< ALLOC >& parser,
         const Apriori< ALLOC >&              apriori,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      CorrectedMutualInformation(const CorrectedMutualInformation< ALLOC >& from);

      /// copy constructor with a given allocator
      CorrectedMutualInformation(const CorrectedMutualInformation< ALLOC >& from,
                                 const allocator_type&                      alloc);

      /// move constructor
      CorrectedMutualInformation(CorrectedMutualInformation< ALLOC >&& from);

      /// move constructor with a given allocator
      CorrectedMutualInformation(CorrectedMutualInformation< ALLOC >&& from,
                                 const allocator_type&                 alloc);

      /// virtual copy constructor
      virtual CorrectedMutualInformation< ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual CorrectedMutualInformation< ALLOC >*
         clone(const allocator_type& alloc) const;

      /// destructor
      virtual ~CorrectedMutualInformation();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CorrectedMutualInformation< ALLOC >&
         operator=(const CorrectedMutualInformation< ALLOC >& from);

      /// move operator
      CorrectedMutualInformation< ALLOC >&
         operator=(CorrectedMutualInformation< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name caching functions
      // ##########################################################################
      /// @{

      /// clears all the data structures from memory
      virtual void clear();

      /// clears all the current caches
      /** There are 4 caches in the CorrectedMutualInformation class:
       * # The I cache is intended to cache the computations of the mutual
       * informations used by 3off2
       * # the H cache is intended to store the results of the computations
       * of the entropies used in the mutual information formula
       * # the K cache is intended to store the penalties computed so far
       * # the Cnr cache is intended to store the results of the computations
       * of the Cnr formula used by the kNML penalty */
      virtual void clearCache();

      /// turn on/off the use of all the caches
      /** There are 4 caches in the CorrectedMutualInformation class:
       * # The I cache is intended to cache the computations of the mutual
       * informations used by 3off2
       * # the H cache is intended to store the results of the computations
       * of the entropies used in the mutual information formula
       * # the K cache is intended to store the penalties computed so far
       * # the Cnr cache is intended to store the results of the computations
       * of the Cnr formula used by the kNML penalty */
      virtual void useCache(bool on_off);

      /// turn on/off the use of the ICache (the mutual information cache)
      void useICache(bool on_off);

      /// clears the ICache (the mutual information  cache)
      void clearICache();

      /// turn on/off the use of the HCache (the cache for the entropies)
      void useHCache(bool on_off);

      /// clears the HCache (the cache for the entropies)
      void clearHCache();

      /// turn on/off the use of the KCache (the cache for the penalties)
      void useKCache(bool on_off);

      /// clears the KCache (the cache for the penalties)
      void clearKCache();

      /// turn on/off the use of the CnrCache (the cache for the Cnr formula)
      void useCnrCache(bool on_off);

      /// clears the CnrCache (the cache for the Cnr formula)
      void clearCnrCache();

      /// @}


      // ##########################################################################
      /// @name score functions
      // ##########################################################################
      /// @{

      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(NodeId var1, NodeId var2);

      /// returns the 2-point mutual information corresponding to a given nodeset
      double score(NodeId                                        var1,
                   NodeId                                        var2,
                   const std::vector< NodeId, ALLOC< NodeId > >& conditioning_ids);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(NodeId var1, NodeId var2, NodeId var3);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(NodeId                                        var1,
                   NodeId                                        var2,
                   NodeId                                        var3,
                   const std::vector< NodeId, ALLOC< NodeId > >& conditioning_ids);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// use the MDL penalty function
      void useMDL();

      /// use the kNML penalty function
      void useNML();

      /// use no correction/penalty function
      void useNoCorr();

      /// changes the max number of threads used to parse the database
      virtual void setMaxNbThreads(std::size_t nb) const;

      /// returns the number of threads used to parse the database
      virtual std::size_t nbThreads() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When computing score, several threads are used by record counters to
       * perform countings on the rows of the database, the MinNbRowsPerThread
       * method indicates how many rows each thread should at least process.
       * This is used to compute the number of threads actually run. This number
       * is equal to the min between the max number of threads allowed and the
       * number of records in the database divided by nb. */
      virtual void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      virtual std::size_t minNbRowsPerThread() const;

      /// sets new ranges to perform the countings used by the mutual information
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The countings are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database. */
      template < template < typename > class XALLOC >
      void setRanges(
         const std::vector< std::pair< std::size_t, std::size_t >,
                            XALLOC< std::pair< std::size_t, std::size_t > > >&
            new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t >,
                         ALLOC< std::pair< std::size_t, std::size_t > > >&
         ranges() const;


      /// returns the allocator used by the score
      allocator_type getAllocator() const;

      /// @}


      /// the description type for the complexity correction
      enum class KModeTypes { MDL, NML, NoCorr };


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// The object to compute N times Entropy H used by mutual information I
      /* Note that the log2-likelihood is equal to N times the entropy H */
      ScoreLog2Likelihood< ALLOC > __NH;

      /// the object computing the NML k score
      KNML< ALLOC > __k_NML;

      /** @brief a score MDL used to compute the size N of the database,
       * including the a priori */
      ScoreMDL< ALLOC > __score_MDL;

      /// the mode used for the correction
      KModeTypes __kmode{KModeTypes::MDL};


      /// a Boolean indicating whether we wish to use the I cache
      /** The I cache is the cache used to store N times the values of
       * mutual informations */
      bool __use_ICache{true};

      /// a Boolean indicating whether we wish to use the H cache
      /** The H cache is the cache for storing N times the entropy. Mutual
       * information is computed as a summation/subtraction of entropies. The
       * latter are cached directly within the __NH instance. */
      bool __use_HCache{true};

      /// a Boolean indicating whether we wish to use the K cache
      /** The K cache is used to cache K-scores, which corresponds to
       * summations/subtractions of kNML individual values. The cache for the
       * latter is called the Cnr cache because it uses Cnr values */
      bool __use_KCache{true};

      /// a Boolean indicating whether we wish to use the Cnr cache
      /** When using the kNML class, the computation of the K-scores
       * consists of summations/subtractions of kNML scores. The latter
       * essentially amount to computing Cnr values. Those can be
       * cached directly within the __k_NML instance */
      bool __use_CnrCache{true};


      /// the ICache
      ScoringCache< ALLOC > __ICache;

      /// the KCache
      ScoringCache< ALLOC > __KCache;


      /// an empty conditioning set
      const std::vector< NodeId, ALLOC< NodeId > > __empty_conditioning_set;

      /// a constant used to prevent numerical instabilities
      const double __threshold{1e-10};


      /// returns the 2-point mutual information corresponding to a given nodeset
      double __NI_score(NodeId                                        var_x,
                        NodeId                                        var_y,
                        const std::vector< NodeId, ALLOC< NodeId > >& vars_z);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double __NI_score(NodeId                                        var_x,
                        NodeId                                        var_y,
                        NodeId                                        var_z,
                        const std::vector< NodeId, ALLOC< NodeId > >& vars_ui);

      /// computes the complexity correction for the mutual information
      double __K_score(NodeId                                        var_x,
                       NodeId                                        var_y,
                       const std::vector< NodeId, ALLOC< NodeId > >& vars_z);

      /// computes the complexity correction for the mutual information
      double __K_score(NodeId                                        var_x,
                       NodeId                                        var_y,
                       NodeId                                        var_z,
                       const std::vector< NodeId, ALLOC< NodeId > >& vars_ui);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
extern template class gum::learning::CorrectedMutualInformation<>;
#endif


// always include the template implementation
#include <agrum/learning/scores_and_tests/correctedMutualInformation_tpl.h>

#endif /* GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H */

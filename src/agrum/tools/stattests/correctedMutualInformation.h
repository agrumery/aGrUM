/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @author Quentin FALCAND, Christophe GONZALES(_at_AMU) and Pierre-Henri
 * WUILLEMIN(_at_LIP6).
 */
#ifndef GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H
#define GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H

#include <agrum/config.h>
#include <vector>

#include <agrum/tools/stattests/kNML.h>
#include <agrum/BN/learning/scores_and_tests/scoreLog2Likelihood.h>
#include <agrum/BN/learning/scores_and_tests/scoreMDL.h>

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
    class CorrectedMutualInformation: public IThreadNumberManager {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
       * @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
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
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      CorrectedMutualInformation(const DBRowGeneratorParser&                                 parser,
                                 const Prior&                                                prior,
                                 const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                                 const Bijection< NodeId, std::size_t >& nodeId2columns
                                 = Bijection< NodeId, std::size_t >());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param prior An prior that we add to the computation of
       * the score (this should come from expert knowledge): this consists in
       * adding numbers to counts in the contingency tables
       * @param nodeId2Columns a mapping from the ids of the nodes in the
       * graphical model to the corresponding column in the DatabaseTable
       * parsed by the parser. This enables estimating from a database in
       * which variable A corresponds to the 2nd column the parameters of a BN
       * in which variable A has a NodeId of 5. An empty nodeId2Columns
       * bijection means that the mapping is an identity, i.e., the value of a
       * NodeId is equal to the index of the column in the DatabaseTable.
       * @warning If nodeId2columns is not empty, then only the scores over the
       * ids belonging to this bijection can be computed: applying method
       * score() over other ids will raise exception NotFound. */
      CorrectedMutualInformation(const DBRowGeneratorParser&             parser,
                                 const Prior&                            prior,
                                 const Bijection< NodeId, std::size_t >& nodeId2columns
                                 = Bijection< NodeId, std::size_t >());

      /// copy constructor
      CorrectedMutualInformation(const CorrectedMutualInformation& from);

      /// move constructor
      CorrectedMutualInformation(CorrectedMutualInformation&& from);

      /// virtual copy constructor
      virtual CorrectedMutualInformation* clone() const;

      /// destructor
      virtual ~CorrectedMutualInformation();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CorrectedMutualInformation& operator=(const CorrectedMutualInformation& from);

      /// move operator
      CorrectedMutualInformation& operator=(CorrectedMutualInformation&& from);

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
      double score(NodeId var1, NodeId var2, const std::vector< NodeId >& conditioning_ids);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(NodeId var1, NodeId var2, NodeId var3);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double score(NodeId                       var1,
                   NodeId                       var2,
                   NodeId                       var3,
                   const std::vector< NodeId >& conditioning_ids);

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
      virtual void setNumberOfThreads(Size nb);

      /// returns the number of threads used to parse the database
      virtual std::size_t getNumberOfThreads() const;

      /// indicates whether the user set herself the number of threads
      virtual bool isGumNumberOfThreadsOverriden() const;

      /** @brief changes the number min of rows a thread should process in a
       * multithreading context
       *
       * When computing score, several threads are used by record counters to
       * perform counts on the rows of the database, the MinNbRowsPerThread
       * method indicates how many rows each thread should at least process.
       * This is used to compute the number of threads actually run. This number
       * is equal to the min between the max number of threads allowed and the
       * number of records in the database divided by nb. */
      virtual void setMinNbRowsPerThread(const std::size_t nb) const;

      /// returns the minimum of rows that each thread should process
      virtual std::size_t minNbRowsPerThread() const;

      /// sets new ranges to perform the counts used by the mutual information
      /** @param ranges a set of pairs {(X1,Y1),...,(Xn,Yn)} of database's rows
       * indices. The counts are then performed only on the union of the
       * rows [Xi,Yi), i in {1,...,n}. This is useful, e.g, when performing
       * cross validation tasks, in which part of the database should be ignored.
       * An empty set of ranges is equivalent to an interval [X,Y) ranging over
       * the whole database. */
      void setRanges(const std::vector< std::pair< std::size_t, std::size_t > >& new_ranges);

      /// reset the ranges to the one range corresponding to the whole database
      void clearRanges();

      /// returns the current ranges
      const std::vector< std::pair< std::size_t, std::size_t > >& ranges() const;

      /// @}


      /// the description type for the complexity correction
      enum class KModeTypes {
        MDL,
        NML,
        NoCorr
      };


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// The object to compute N times Entropy H used by mutual information I
      /* Note that the log2-likelihood is equal to N times the entropy H */
      ScoreLog2Likelihood _NH_;

      /// the object computing the NML k score
      KNML _k_NML_;

      /** @brief a score MDL used to compute the size N of the database,
       * including the a prior*/
      ScoreMDL _score_MDL_;

      /// the mode used for the correction
      KModeTypes _kmode_{KModeTypes::MDL};


      /// a Boolean indicating whether we wish to use the I cache
      /** The I cache is the cache used to store N times the values of
       * mutual informations */
      bool _use_ICache_{true};

      /// a Boolean indicating whether we wish to use the H cache
      /** The H cache is the cache for storing N times the entropy. Mutual
       * information is computed as a summation/subtraction of entropies. The
       * latter are cached directly within the  _NH_ instance. */
      bool _use_HCache_{true};

      /// a Boolean indicating whether we wish to use the K cache
      /** The K cache is used to cache K-scores, which corresponds to
       * summations/subtractions of kNML individual values. The cache for the
       * latter is called the Cnr cache because it uses Cnr values */
      bool _use_KCache_{true};

      /// a Boolean indicating whether we wish to use the Cnr cache
      /** When using the kNML class, the computation of the K-scores
       * consists of summations/subtractions of kNML scores. The latter
       * essentially amount to computing Cnr values. Those can be
       * cached directly within the  _k_NML_ instance */
      bool _use_CnrCache_{true};


      /// the ICache
      ScoringCache _ICache_;

      /// the KCache
      ScoringCache _KCache_;


      /// an empty conditioning set
      const std::vector< NodeId > _empty_conditioning_set_;

      /// a constant used to prevent numerical instabilities
      const double _threshold_{1e-10};


      /// returns the 2-point mutual information corresponding to a given nodeset
      double _NI_score_(NodeId var_x, NodeId var_y, const std::vector< NodeId >& vars_z);

      /// returns the 3-point mutual information corresponding to a given nodeset
      double
         _NI_score_(NodeId var_x, NodeId var_y, NodeId var_z, const std::vector< NodeId >& vars_ui);

      /// computes the complexity correction for the mutual information
      double _K_score_(NodeId var_x, NodeId var_y, const std::vector< NodeId >& vars_z);

      /// computes the complexity correction for the mutual information
      double
         _K_score_(NodeId var_x, NodeId var_y, NodeId var_z, const std::vector< NodeId >& vars_ui);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/stattests/correctedMutualInformation_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CORRECTED_MUTUAL_INFORMATION_H */

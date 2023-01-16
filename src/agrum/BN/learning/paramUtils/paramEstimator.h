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


/** @file
 * @brief the base class for estimating parameters of CPTs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_PARAM_ESTIMATOR_H
#define GUM_LEARNING_PARAM_ESTIMATOR_H

#include <type_traits>

#include <agrum/agrum.h>
#include <agrum/BN/learning/priors/prior.h>
#include <agrum/tools/stattests/recordCounter.h>

namespace gum {

  namespace learning {


    /** @class ParamEstimator
     * @brief The base class for estimating parameters of CPTs
     * @headerfile paramEstimator.h <agrum/BN/learning/paramUtils/paramEstimator.h>
     * @ingroup learning_param_utils
     */
    class ParamEstimator: public IThreadNumberManager {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation
       * of the score
       * @param score_internal_prior The prior within the score used
       * to learn the data structure (might be a NoPrior)
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
      ParamEstimator(const DBRowGeneratorParser& parser,
                     const Prior&                external_prior,
                     const Prior&                _score_internal_prior,
                     const std::vector< std::pair< std::size_t, std::size_t > >& ranges,
                     const Bijection< NodeId, std::size_t >&                     nodeId2columns
                     = Bijection< NodeId, std::size_t >());

      /// default constructor
      /** @param parser the parser used to parse the database
       * @param external_prior An prior that we add to the computation
       * of the score
       * @param score_internal_prior The prior within the score used
       * to learn the data structure (might be a NoPrior)
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
      ParamEstimator(const DBRowGeneratorParser&             parser,
                     const Prior&                            external_prior,
                     const Prior&                            _score_internal_prior,
                     const Bijection< NodeId, std::size_t >& nodeId2columns
                     = Bijection< NodeId, std::size_t >());

      /// copy constructor
      ParamEstimator(const ParamEstimator& from);

      /// move constructor
      ParamEstimator(ParamEstimator&& from);

      /// virtual copy constructor
      virtual ParamEstimator* clone() const = 0;

      /// destructor
      virtual ~ParamEstimator();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// clears all the data structures from memory
      virtual void clear();

      /// sets the number max of threads that can be used
      /**
       * @param nb the number max of threads to be used. If this number is set to 0, then
       * it is defaulted to aGrUM's max number of threads
       */
      virtual void setNumberOfThreads(Size nb);

      /// returns the current max number of threads of the scheduler
      virtual Size getNumberOfThreads() const;

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

      /// sets new ranges to perform the counts used by the parameter estimator
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

      /// returns the CPT's parameters corresponding to a given target node
      std::vector< double > parameters(const NodeId target_node);

      /// returns the CPT's parameters corresponding to a given nodeset
      /** The vector contains the parameters of an n-dimensional CPT. The
       * distribution of the dimensions of the CPT within the vector is as
       * follows:
       * first, there is the target node, then the conditioning nodes (in the
       * order in which they were specified). */
      virtual std::vector< double > parameters(const NodeId                 target_node,
                                               const std::vector< NodeId >& conditioning_nodes)
         = 0;

      /// sets the CPT's parameters corresponding to a given Potential
      /** The potential is assumed to be a conditional probability, the first
       * variable of its variablesSequence() being the target variable, the
       * other ones being on the right side of the conditioning bar. */
      template < typename GUM_SCALAR >
      void setParameters(const NodeId                 target_node,
                         const std::vector< NodeId >& conditioning_nodes,
                         Potential< GUM_SCALAR >&     pot);

      /// returns the mapping from ids to column positions in the database
      /** @warning An empty nodeId2Columns bijection means that the mapping is
       * an identity, i.e., the value of a NodeId is equal to the index of the
       * column in the DatabaseTable. */
      const Bijection< NodeId, std::size_t >& nodeId2Columns() const;

      /// returns the database on which we perform the counts
      const DatabaseTable& database() const;

      /// assign a new Bayes net to all the counter's generators depending on a BN
      /** Typically, generators based on EM or K-means depend on a model to
       * compute correctly their outputs. Method setBayesNet enables to
       * update their BN model. */
      template < typename GUM_SCALAR >
      void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn);

      /// @}

      protected:
      /// an external a priori
      Prior* external_prior_{nullptr};

      /** @brief if a score was used for learning the structure of the PGM, this
       * is the priori internal to the score */
      Prior* score_internal_prior_{nullptr};

      /// the record counter used to parse the database
      RecordCounter counter_;

      /// an empty vector of nodes, used for empty conditioning
      const std::vector< NodeId > empty_nodevect_;


      /// copy operator
      ParamEstimator& operator=(const ParamEstimator& from);

      /// move operator
      ParamEstimator& operator=(ParamEstimator&& from);

      private:
#ifndef DOXYGEN_SHOULD_SKIP_THIS

      /** @brief check the coherency between the parameters passed to
       * the setParameters functions */
      template < typename GUM_SCALAR >
      void _checkParameters_(const NodeId                 target_node,
                             const std::vector< NodeId >& conditioning_nodes,
                             Potential< GUM_SCALAR >&     pot);

      // sets the CPT's parameters corresponding to a given Potential
      // when the potential belongs to a BayesNet<GUM_SCALAR> when
      // GUM_SCALAR is different from a double
      template < typename GUM_SCALAR >
      typename std::enable_if< !std::is_same< GUM_SCALAR, double >::value, void >::type
         _setParameters_(const NodeId                 target_node,
                         const std::vector< NodeId >& conditioning_nodes,
                         Potential< GUM_SCALAR >&     pot);

      // sets the CPT's parameters corresponding to a given Potential
      // when the potential belongs to a BayesNet<GUM_SCALAR> when
      // GUM_SCALAR is equal to double (the code is optimized for doubles)
      template < typename GUM_SCALAR >
      typename std::enable_if< std::is_same< GUM_SCALAR, double >::value, void >::type
         _setParameters_(const NodeId                 target_node,
                         const std::vector< NodeId >& conditioning_nodes,
                         Potential< GUM_SCALAR >&     pot);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include templated methods
#include <agrum/BN/learning/paramUtils/paramEstimator_tpl.h>

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/BN/learning/paramUtils/paramEstimator_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_PARAM_ESTIMATOR_H */

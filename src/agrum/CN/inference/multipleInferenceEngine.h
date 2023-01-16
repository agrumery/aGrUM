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


#ifndef __MULTIPLE_INFERENCE_ENGINES__H__
#define __MULTIPLE_INFERENCE_ENGINES__H__

/**
 * @file
 * @brief Abstract class representing CredalNet inference engines
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6) and Christophe GONZALES(_at_AMU)
 */

/// @todo virtual for all functions that MAY be one day redefined in any derived
/// class

#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CN/inference/inferenceEngine.h>

namespace gum {
  namespace credal {

    /**
     * @class MultipleInferenceEngine
     * @headerfile <agrum/CN/multipleInferenceEngine.h>
     * @brief Class template representing a CredalNet inference engine using
     * one or more IBayesNet inference engines such as LazyPropagation. Extends
     * InferenceEngine< GUM_SCALAR >. Used for outer multi-threading such as
     * CNMonteCarloSampling.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @tparam BNInferenceEngine A IBayesNet inference engine such as
     * LazyPropagation.
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
     */
    template < typename GUM_SCALAR, class BNInferenceEngine >
    class MultipleInferenceEngine: public InferenceEngine< GUM_SCALAR > {
      private:
      /** To easily access InferenceEngine< GUM_SCALAR > methods. */
      using _infE_ = InferenceEngine< GUM_SCALAR >;

      using _cluster_   = NodeProperty< std::vector< NodeId > >;
      using _credalSet_ = NodeProperty< std::vector< std::vector< GUM_SCALAR > > >;
      using _margi_     = NodeProperty< std::vector< GUM_SCALAR > >;
      using _expe_      = NodeProperty< GUM_SCALAR >;

      using _bnet_       = IBayesNet< GUM_SCALAR >;
      using _margis_     = std::vector< _margi_ >;
      using _expes_      = std::vector< _expe_ >;
      using _credalSets_ = std::vector< _credalSet_ >;
      using _clusters_   = std::vector< std::vector< _cluster_ > >;

      using _modals_ = std::vector< HashTable< std::string, std::vector< GUM_SCALAR > > >;

      /**
       * @brief Ask for redundancy elimination of a node credal set of a calling
       *thread.
       *
       * Called by updateThread_ if vertices are stored.
       *
       * @param this_thread the id of the thread executing this method
       * @param id A constant reference to the node id whose credal set is to be
       *checked for redundancy.
       * @param vertex The vertex to add to the credal set.
       * @param elimRedund \c true if redundancy elimination is to be performed,
       *\c
       *false otherwise and by default.
       */
      inline void _updateThreadCredalSets_(Size                             this_thread,
                                           const NodeId&                    id,
                                           const std::vector< GUM_SCALAR >& vertex,
                                           const bool&                      elimRedund);

      protected:
      /** Threads lower marginals, one per thread. */
      _margis_ l_marginalMin_;
      /** Threads upper marginals, one per thread. */
      _margis_ l_marginalMax_;
      /** Threads lower expectations, one per thread. */
      _expes_ l_expectationMin_;
      /** Threads upper expectations, one per thread. */
      _expes_ l_expectationMax_;
      /** Threads modalities. */
      _modals_ l_modal_;
      /** Threads vertices. */
      _credalSets_ l_marginalSets_;
      /** Threads evidence. */
      _margis_ l_evidence_;
      /** Threads clusters. */
      _clusters_ l_clusters_;


      /** Threads IBayesNet. */
      typename std::vector< _bnet_* > workingSet_;
      /** Threads evidence. */
      typename std::vector< List< const Potential< GUM_SCALAR >* >* > workingSetE_;

      /** Threads BNInferenceEngine. */
      typename std::vector< BNInferenceEngine* > l_inferenceEngine_;
      /** Threads optimal IBayesNet. */
      std::vector< VarMod2BNsMap< GUM_SCALAR >* > l_optimalNet_;

      /// the generators used for computing random values
      std::vector< std::mt19937 > generators_;

      /** Fusion of threads optimal IBayesNet. */
      // OptBN< GUM_SCALAR > threadFusion_; // we should use this OptBN if omp
      // is
      // disabled (avoid creating 2 objects when only one is necessary)
      // it should also avoid calling thread fusion operations

      /// @name Protected initialization methods
      /// @{

      /**
       * @brief Initialize threads data.
       *
       * @param num_threads The number of threads.
       * @param  _storeVertices_ \c True if vertices should be stored, \c False
       *otherwise.
       * @param  _storeBNOpt_ \c True if optimal IBayesNet should be stored, \c
       *false
       *otherwise.
       */
      void initThreadsData_(const Size& num_threads,
                            const bool  _storeVertices_,
                            const bool  _storeBNOpt_);

      /// @}

      /// @name Protected algorithms methods
      /// @{

      /**
       * @brief Update thread information (marginals, expectations, IBayesNet,
       *vertices) for a given node id.
       *
       * @param this_thread the id of the thread executing this method
       * @param id The id of the node to be updated.
       * @param vertex The vertex.
       * @param elimRedund \c true if redundancy elimination is to be performed,
       *\c
       *false otherwise and by default.
       * @return \c True if the IBayesNet is kept (for now), \c False otherwise.
       */
      inline bool updateThread_(Size                             this_thread,
                                const NodeId&                    id,
                                const std::vector< GUM_SCALAR >& vertex,
                                const bool&                      elimRedund = false);

      /**
       * @brief Fusion of threads marginals.
       */
      inline void updateMarginals_();

      /**
       * @brief Compute epsilon and update old marginals.
       *
       * @return Epsilon.
       */
      inline const GUM_SCALAR computeEpsilon_();

      /**
       * Update old marginals (from current marginals). Call this once to
       * initialize
       * old marginals (after burn-in for example) and then use computeEpsilon_
       * which
       * does the same job but compute epsilon too.
       */
      void updateOldMarginals_();

      /// @}

      /// @name Proptected post-inference methods
      /// @{

      /** Fusion of threads optimal IBayesNet. */
      void optFusion_();
      /** Fusion of threads expectations. */
      void expFusion_();
      /** @deprecated Fusion of threads vertices. */
      void verticesFusion_();   // called ?? not done yet

      /// @}

      public:
      /// @name Constructors / Destructors
      /// @{

      /**
       * Constructor.
       * @param credalNet The CredalNet to be used.
       */
      explicit MultipleInferenceEngine(const CredalNet< GUM_SCALAR >& credalNet);

      /** Destructor. */
      virtual ~MultipleInferenceEngine();

      /// @}

      /// @name Post-inference methods
      /// @{
      /**
       * Erase all inference related data to perform another one. You need to
       * insert
       * evidence again if needed but modalities are kept. You can insert new
       * ones by
       * using the appropriate method which will delete the old ones.
       */
      virtual void eraseAllEvidence();
      /// @}

      /// @name Pure virtual methods
      /// @{
      /** To be redefined by each credal net algorithm. Starts the inference. */
      virtual void makeInference() = 0;

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class MultipleInferenceEngine< double, gum::LazyPropagation< double > >;

    extern template class MultipleInferenceEngine< float, gum::LazyPropagation< float > >;
#endif

  }   // namespace credal
}   // namespace gum

#include <agrum/CN/inference/multipleInferenceEngine_tpl.h>

#endif

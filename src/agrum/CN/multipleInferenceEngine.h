/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
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

#ifndef __MULTIPLE_INFERENCE_ENGINES__H__
#define __MULTIPLE_INFERENCE_ENGINES__H__

/**
 * @file
 * @brief Abstract class representing CredalNet inference engines
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

/// @todo virtual for all functions that MAY be one day redefined in any derived
/// class

#include <agrum/CN/inferenceEngine.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {
  namespace credal {

    /**
     * @class MultipleInferenceEngine MultipleInferenceEngine.h
     * <agrum/CN/MultipleInferenceEngine.h>
     * @brief Class template representing a CredalNet inference engine using one or
     * more IBayesNet inference engines such as LazyPropagation. Extends
     * InferenceEngine< GUM_SCALAR >. Used for outer multi-threading such as
     * CNMonteCarloSampling.
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @tparam BNInferenceEngine A IBayesNet inference engine such as
     * LazyPropagation.
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
     */
    template <typename GUM_SCALAR, class BNInferenceEngine>
    class MultipleInferenceEngine : public InferenceEngine<GUM_SCALAR> {
      private:
      /** To easily access InferenceEngine< GUM_SCALAR > methods. */
      typedef InferenceEngine<GUM_SCALAR> __infE;

      typedef NodeProperty<std::vector<NodeId>> __cluster;
      typedef NodeProperty<std::vector<std::vector<GUM_SCALAR>>> __credalSet;
      typedef NodeProperty<std::vector<GUM_SCALAR>> __margi;
      typedef NodeProperty<GUM_SCALAR> __expe;

      typedef IBayesNet<GUM_SCALAR> __bnet;
      typedef std::vector<__margi> __margis;
      typedef std::vector<__expe> __expes;
      typedef std::vector<__credalSet> __credalSets;
      typedef std::vector<std::vector<__cluster>> __clusters;

      typedef typename std::vector<HashTable<std::string, std::vector<GUM_SCALAR>>>
          __modals;

      /**
       * @brief Ask for redundancy elimination of a node credal set of a calling
       *thread.
       *
       * Called by _updateThread if vertices are stored.
       *
       * @param id A constant reference to the node id whose credal set is to be
       *checked for redundancy.
       * @param vertex The vertex to add to the credal set.
       * @param elimRedund \c true if redundancy elimination is to be performed, \c
       *false otherwise and by default.
       */
      inline void __updateThreadCredalSets(const NodeId &id,
                                           const std::vector<GUM_SCALAR> &vertex,
                                           const bool &elimRedund);

      protected:
      /** Threads lower marginals, one per thread. */
      __margis _l_marginalMin;
      /** Threads upper marginals, one per thread. */
      __margis _l_marginalMax;
      /** Threads lower expectations, one per thread. */
      __expes _l_expectationMin;
      /** Threads upper expectations, one per thread. */
      __expes _l_expectationMax;
      /** Threads modalities. */
      __modals _l_modal;
      /** Threads vertices. */
      __credalSets _l_marginalSets;
      /** Threads evidence. */
      __margis _l_evidence;
      /** Threads clusters. */
      __clusters _l_clusters;

      /** Threads IBayesNet. */
      typename std::vector<__bnet *> _workingSet;
      /** Threads evidence. */
      typename std::vector<List<const Potential<GUM_SCALAR> *> *> _workingSetE;

      /** Threads BNInferenceEngine. */
      typename std::vector<BNInferenceEngine *> _l_inferenceEngine;
      /** Threads optimal IBayesNet. */
      std::vector<VarMod2BNsMap<GUM_SCALAR> *> _l_optimalNet;
      /** Fusion of threads optimal IBayesNet. */
      // OptBN< GUM_SCALAR > _threadFusion; // we should use this OptBN if omp is
      // disabled (avoid creating 2 objects when only one is necessary)
      // it should also avoid calling thread fusion operations

      /// @name Protected initialization methods
      /// @{

      /**
       * @brief Initialize threads data.
       *
       * @param num_threads The number of threads.
       * @param __storeVertices \c True if vertices should be stored, \c False
       *otherwise.
       * @param __storeBNOpt \c True if optimal IBayesNet should be stored, \c false
       *otherwise.
       */
      void _initThreadsData(const unsigned int &num_threads,
                            const bool __storeVertices, const bool __storeBNOpt);

      /// @}

      /// @name Protected algorithms methods
      /// @{

      /**
       * @brief Update thread information (marginals, expectations, IBayesNet,
       *vertices) for a given node id.
       *
       * @param id The id of the node to be updated.
       * @param vertex The vertex.
       * @param elimRedund \c true if redundancy elimination is to be performed, \c
       *false otherwise and by default.
       * @return \c True if the IBayesNet is kept (for now), \c False otherwise.
       */
      inline bool _updateThread(const NodeId &id,
                                const std::vector<GUM_SCALAR> &vertex,
                                const bool &elimRedund = false);

      /**
       * @brief Fusion of threads marginals.
       */
      inline void _updateMarginals();

      /**
       * @brief Compute epsilon and update old marginals.
       *
       * @return Epsilon.
       */
      inline const GUM_SCALAR _computeEpsilon();

      /**
       * Update old marginals (from current marginals). Call this once to initialize
       * old marginals (after burn-in for example) and then use _computeEpsilon which
       * does the same job but compute epsilon too.
       */
      void _updateOldMarginals();

      /// @}

      /// @name Proptected post-inference methods
      /// @{

      /** Fusion of threads optimal IBayesNet. */
      void _optFusion();
      /** Fusion of threads expectations. */
      void _expFusion();
      /** @deprecated Fusion of threads vertices. */
      void _verticesFusion(); // called ?? not done yet

      /// @}

      public:
      /// @name Constructors / Destructors
      /// @{

      /**
       * Constructor.
       * @param credalNet The CredalNet to be used.
       */
      MultipleInferenceEngine(const CredalNet<GUM_SCALAR> &credalNet);

      /** Destructor. */
      virtual ~MultipleInferenceEngine();

      /// @}

      /// @name Post-inference methods
      /// @{
      /**
       * Erase all inference related data to perform another one. You need to insert
       * evidence again if needed but modalities are kept. You can insert new ones by
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

    extern template class MultipleInferenceEngine<double,
                                                  gum::LazyPropagation<double>>;
    extern template class MultipleInferenceEngine<float,
                                                  gum::LazyPropagation<float>>;
  } // end of namespace
}

#include <agrum/CN/multipleInferenceEngine.tcc>

#endif

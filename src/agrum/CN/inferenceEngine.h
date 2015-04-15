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

#ifndef __INFERENCE_ENGINE__H__
#define __INFERENCE_ENGINE__H__

/**
 * @file
 * @brief Abstract class representing CredalNet inference engines
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 */

/// @todo virtual for all functions that MAY be one day redefined in any derived
/// class

#include <agrum/core/algorithms/approximationScheme/approximationScheme.h>
#include <agrum/CN/varMod2BNsMap.h>
#include <agrum/core/exceptions.h>
#include <map>

#include <agrum/core/math/rational.h>
#include <agrum/core/math/pow.h>

#include <agrum/CN/LrsWrapper.h>

namespace gum {
  namespace credal {

    /**
     * @class InferenceEngine InferenceEngine.h <agrum/CN/InferenceEngine.h>
     * @brief Abstract class template representing a CredalNet inference engine. Used
     * by credal network inference algorithms such as CNLoopyPropagation (inner
     * multi-threading) or CNMonteCarloSampling (outer multi-threading).
     * @ingroup cn_group
     * @tparam GUM_SCALAR A floating type ( float, double, long double ... ).
     * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
     */
    template <typename GUM_SCALAR>
    class InferenceEngine : public ApproximationScheme {
      private:
      typedef NodeProperty<std::vector<std::vector<GUM_SCALAR>>> credalSet;
      typedef NodeProperty<std::vector<GUM_SCALAR>> margi;
      typedef NodeProperty<GUM_SCALAR> expe;

      typedef typename gum::HashTable<std::string, std::vector<GUM_SCALAR>> dynExpe;

      typedef NodeProperty<std::vector<bool>> query;
      typedef NodeProperty<std::vector<NodeId>> cluster;

      protected:
      /** @brief A pointer to the Credal Net used. */
      const CredalNet<GUM_SCALAR> *_credalNet;

      /** @brief Old lower marginals used to compute epsilon. */
      margi _oldMarginalMin;
      /** @brief Old upper marginals used to compute epsilon. */
      margi _oldMarginalMax;

      /** @brief Lower marginals. */
      margi _marginalMin;
      /** @brief Upper marginals. */
      margi _marginalMax;

      /** @brief Credal sets vertices, if enabled */
      credalSet _marginalSets;

      /** @brief Lower expectations, if some variables modalities were inserted. */
      expe _expectationMin;
      /** @brief Upper expectations, if some variables modalities were inserted. */
      expe _expectationMax;

      /** @brief Lower dynamic expectations. If the network is not dynamic it's
       * content is the same as _expectationMin. */
      dynExpe _dynamicExpMin;
      /** @brief Upper dynamic expectations. If the network if not dynamic it's
       * content is the same as _expectationMax. */
      dynExpe _dynamicExpMax;

      /** @brief Variables modalities used to compute expectations. */
      dynExpe _modal;

      /** @brief Holds observed variables states. */
      margi _evidence;
      /** @brief Holds the query nodes states. */
      query _query;

      /** @brief Clusters of nodes used with dynamic networks. Any node key in _t0 is
       * present at \f$ t=0 \f$ and any node belonging to the node set of this key
       * share the same CPT than the key. Used for sampling with repetitive
       * independence. */
      cluster _t0;
      /** @brief Clusters of nodes used with dynamic networks. Any node key in _t1 is
       * present at \f$ t=1 \f$ and any node belonging to the node set of this key
       * share the same CPT than the key. Used for sampling with repetitive
       * independence. */
      cluster _t1;

      /** @brief \c True if credal sets vertices are stored, \c False otherwise. \c
       * False by default. */
      bool _storeVertices;
      /** @brief \c True if using repetitive independence ( dynamic network only ),
       * \c False otherwise. \c False by default. */
      bool _repetitiveInd;
      /** @brief Iterations limit stopping rule used by some algorithms such as
       * CNMonteCarloSampling. The algorithms stops if no changes occured within 1000
       * iterations by default. */
      /// int _iterStop;
      /** @brief \c True is optimal bayes net are stored, for each variable and each
       * modality, \c False otherwise. Not all algorithms offers this option. \c
       * False by default. */
      bool _storeBNOpt;
      /** @brief Object used to efficiently store optimal bayes net during inference,
       * for some algorithms. */
      VarMod2BNsMap<GUM_SCALAR> _dbnOpt;

      /**
       * @deprecated
       * @brief The number of time steps of this network (only usefull for dynamic
       * networks). */
      int _timeSteps;

      /// @name Protected initialization methods
      /// @{
      /**
       * Initialize _t0 and _t1 clusters.
       */
      void _repetitiveInit();

      /**
       * Initialize lower and upper expectations before inference, with the lower
       * expectation being initialized on the highest modality and the upper
       * expectation being initialized on the lowest modality.
       */
      void _initExpectations();

      /**
       * Initialize lower and upper old marginals and marginals before inference,
       * with the lower marginal being 1 and the upper 0.
       */
      void _initMarginals();

      /**
       * Initialize credal set vertices with empty sets.
       */
      void _initMarginalSets();

      /// @}

      /// @name Protected algorithms methods
      /// @{
      /**
       * Compute approximation scheme epsilon using the old marginals and the new
       *ones. Highest delta on either lower or upper marginal is epsilon.
       *
       * Also updates oldMarginals to current marginals.
       *
       * @return Epsilon.
       */
      inline const GUM_SCALAR _computeEpsilon();

      /**
       * Given a node id and one of it's possible vertex obtained during inference,
       *update this node lower and upper expectations.
       *
       * @param id The id of the node to be updated
       * @param vertex A (potential) vertex of the node credal set
       */
      inline void _updateExpectations(const NodeId &id,
                                      const std::vector<GUM_SCALAR> &vertex);

      /**
       * @deprecated Use _updateCredalSets ( LrsWrapper without intput/output files
       *needed ).
       *
       * Given a node id and one of it's possible vertex, update it's credal set.
       * To maximise efficiency, don't pass a vertex we know is inside the polytope
       *(i.e. not at an extreme value for any modality)
       *
       * @param id The id of the node to be updated
       * @param vertex A (potential) vertex of the node credal set
       * @param elimRedund remove redundant vertex (inside a facet)
       */
      GUM_DEPRECATED(inline void _updateCredalSetsWithFiles(
          const NodeId &id, const std::vector<GUM_SCALAR> &vertex,
          const bool &elimRedund = false));

      /**
       * Given a node id and one of it's possible vertex, update it's credal set.
       * To maximise efficiency, don't pass a vertex we know is inside the polytope
       *(i.e. not at an extreme value for any modality)
       *
       * @param id The id of the node to be updated
       * @param vertex A (potential) vertex of the node credal set
       * @param elimRedund remove redundant vertex (inside a facet)
       */
      inline void _updateCredalSets(const NodeId &id,
                                    const std::vector<GUM_SCALAR> &vertex,
                                    const bool &elimRedund = false);

      /// @}

      /// @name Proptected post-inference methods
      /// @{
      /**
       * Rearrange lower and upper expectations to suit dynamic networks.
       */
      void _dynamicExpectations();
      /// @}

      public:
      /// @name Constructors / Destructors
      /// @{

      // InferenceEngine ();
      /**
       * Construtor.
       *
       * @param credalNet The credal net to be used with this inference engine.
       */
      InferenceEngine(const CredalNet<GUM_SCALAR> &credalNet);
      /**
       * Destructor.
       */
      virtual ~InferenceEngine();

      /// @}

      /// @name Pure virtual methods
      /// @{
      /** To be redefined by each credal net algorithm. Starts the inference. */
      virtual void makeInference() = 0;
      /// @}

      /// @name Getters and setters
      /// @{
      /**
       * Get optimum IBayesNet.
       * @return A pointer to the optimal net object.
       */
      VarMod2BNsMap<GUM_SCALAR> *getVarMod2BNsMap();

      /**
       * Get this creadal network.
       * @return A constant reference to this CredalNet.
       */
      const CredalNet<GUM_SCALAR> &credalNet();

      /**
       * Get the _t0 cluster.
       * @return A constant reference to the _t0 cluster.
       */
      const NodeProperty<std::vector<NodeId>> &getT0Cluster() const;

      /**
       * Get the _t1 cluster.
       * @return A constant reference to the _t1 cluster.
       */
      const NodeProperty<std::vector<NodeId>> &getT1Cluster() const;

      /**
       * @param repetitive \c True if repetitive independence is to be used, false
       * otherwise. Only usefull with dynamic networks.
       */
      void setRepetitiveInd(const bool repetitive);

      /**
       * @param value \c True if vertices are to be stored, false otherwise.
       */
      void storeVertices(const bool value);
      /**
       * @param value \c True if optimal bayesian networks are to be stored for each
       * variable and each modality.
       */
      void storeBNOpt(const bool value);

      /**
       * Get the current independence status.
       * @return \c True if repetitive, \c False otherwise.
       */
      bool repetitiveInd() const;
      /**
       * Get the number of iterations without changes used to stop some algorithms.
       * @return the number of iterations.
       */
      /// int iterStop () const;
      /**
       * @return \c True if vertice are stored, \c False otherwise.
       */
      bool storeVertices() const;
      /**
       * @return \c True if optimal bayes net are stored for each variable and each
       * modality, \c False otherwise.
       */
      bool storeBNOpt() const;
      /// @}

      /// @name Pre-inference initialization methods
      /// @{
      /**
       * Insert variables modalities from file to compute expectations.
       * @param path The path to the modalities file.
       */
      void insertModalsFile(const std::string &path);

      /**
       * Insert variables modalities from map to compute expectations.
       * @param modals The map variable name - modalities.
       */
      void
      insertModals(const std::map<std::string, std::vector<GUM_SCALAR>> &modals);

      /**
       * Insert evidence from file.
       * @param path The path to the evidence file.
       */
      virtual void insertEvidenceFile(const std::string &path);

      /**
       * Insert evidence from map.
       * @param eviMap The map variable name - likelihood.
       */
      void
      insertEvidence(const std::map<std::string, std::vector<GUM_SCALAR>> &eviMap);

      /**
       * Insert evidence from Property.
       * @param evidence The on nodes Property containing likelihoods.
       */
      void insertEvidence(const NodeProperty<std::vector<GUM_SCALAR>> &evidence);

      /**
       * Insert query variables states from file.
       * @param path The path to the query file.
       */
      void insertQueryFile(const std::string &path);

      /**
       * Insert query variables and states from Property.
       * @param query The on nodes Property containing queried variables states.
       */
      void insertQuery(const NodeProperty<std::vector<bool>> &query);

      /// @}

      /// @name Post-inference methods
      /// @{
      /**
       * Erase all inference related data to perform another one. You need to insert
       * evidence again if needed but modalities are kept. You can insert new ones by
       * using the appropriate method which will delete the old ones.
       */
      virtual void eraseAllEvidence();

      /**
       * Get the lower marginals of a given node id.
       * @param id The node id which lower marginals we want.
       * @return A constant reference to this node lower marginals.
       */
      const std::vector<GUM_SCALAR> &marginalMin(const NodeId id) const;

      /**
       * Get the upper marginals of a given node id.
       * @param id The node id which upper marginals we want.
       * @return A constant reference to this node upper marginals.
       */
      const std::vector<GUM_SCALAR> &marginalMax(const NodeId id) const;

      /**
       * Get the lower marginals of a given variable name.
       * @param varName The variable name which lower marginals we want.
       * @return A constant reference to this variable lower marginals.
       */
      const std::vector<GUM_SCALAR> &marginalMin(const std::string &varName) const;

      /**
       * Get the upper marginals of a given variable name.
       * @param varName The variable name which upper marginals we want.
       * @return A constant reference to this variable upper marginals.
       */
      const std::vector<GUM_SCALAR> &marginalMax(const std::string &varName) const;

      /**
       * Get the lower expectation of a given node id.
       * @param id The node id which lower expectation we want.
       * @return A constant reference to this node lower expectation.
       */
      const GUM_SCALAR &expectationMin(const NodeId id) const;

      /**
       * Get the upper expectation of a given node id.
       * @param id The node id which upper expectation we want.
       * @return A constant reference to this node upper expectation.
       */
      const GUM_SCALAR &expectationMax(const NodeId id) const;

      /**
       * Get the lower expectation of a given variable name.
       * @param varName The variable name which lower expectation we want.
       * @return A constant reference to this variable lower expectation.
       */
      const GUM_SCALAR &expectationMin(const std::string &varName) const;

      /**
       * Get the upper expectation of a given variable name.
       * @param varName The variable name which upper expectation we want.
       * @return A constant reference to this variable upper expectation.
       */
      const GUM_SCALAR &expectationMax(const std::string &varName) const;

      /**
       * Get the lower dynamic expectation of a given variable prefix (without the
       * time step included, i.e. call with "temp" to get "temp_0", ..., "temp_T").
       * @param varName The variable name prefix which lower expectation we want.
       * @return A constant reference to the variable lower expectation over all time
       * steps.
       */
      const std::vector<GUM_SCALAR> &dynamicExpMin(const std::string &varName) const;

      /**
       * Get the upper dynamic expectation of a given variable prefix (without the
       * time step included, i.e. call with "temp" to get "temp_0", ..., "temp_T").
       * @param varName The variable name prefix which upper expectation we want.
       * @return A constant reference to the variable upper expectation over all time
       * steps.
       */
      const std::vector<GUM_SCALAR> &dynamicExpMax(const std::string &varName) const;

      /**
       * Get the vertice of a given node id.
       * @param id The node id which vertice we want.
       * @return A constant reference to this node vertice.
       */
      const std::vector<std::vector<GUM_SCALAR>> &vertices(const NodeId id) const;

      /**
       * Saves marginals to file.
       * @param path The path to the file to be used.
       */
      void saveMarginals(const std::string &path) const;

      /**
       * Saves expectations to file.
       *  @param path The path to the file to be used.
       */
      void saveExpectations(const std::string &path) const;

      /**
       * Saves vertices to file.
       * @param path The path to the file to be used.
       */
      void saveVertices(const std::string &path) const;

      /**
       * Compute dynamic expectations.
       * @see _dynamicExpectations
       * Only call this if an algorithm does not call it by itself.
       */
      void dynamicExpectations(); // if someone forgets the protected call at the end
                                  // of its own algorithm, the user can call it

      /**
       * Print all nodes marginals to standart output.
       */
      std::string toString() const;

      /**
       * Get approximation scheme state.
       * @return A constant string about approximation scheme state.
       */
      const std::string getApproximationSchemeMsg() {
        return this->messageApproximationScheme();
      }

      /// @}
    };

    extern template class InferenceEngine<float>;
    extern template class InferenceEngine<double>;
  }
} // namespace gum

#include <agrum/CN/inferenceEngine.tcc>

#endif

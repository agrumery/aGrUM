#include <agrum/CN/inferenceEngine.h>
#include <agrum/CN/multipleInferenceEngine.h>

namespace gum {
  namespace credal {

    template <typename GUM_SCALAR, class BNInferenceEngine>
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::MultipleInferenceEngine(
        const CredalNet<GUM_SCALAR> &credalNet)
        : InferenceEngine<GUM_SCALAR>::InferenceEngine(credalNet) {
      GUM_CONSTRUCTOR(MultipleInferenceEngine);
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    MultipleInferenceEngine<GUM_SCALAR,
                            BNInferenceEngine>::~MultipleInferenceEngine() {
      GUM_DESTRUCTOR(MultipleInferenceEngine);
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    inline void
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_initThreadsData(
        const unsigned int &num_threads, const bool __storeVertices,
        const bool __storeBNOpt) {
      _workingSet.clear();
      _workingSet.resize(num_threads, nullptr);
      _workingSetE.clear();
      _workingSetE.resize(num_threads, nullptr);

      _l_marginalMin.clear();
      _l_marginalMin.resize(num_threads);
      _l_marginalMax.clear();
      _l_marginalMax.resize(num_threads);
      _l_expectationMin.clear();
      _l_expectationMin.resize(num_threads);
      _l_expectationMax.clear();
      _l_expectationMax.resize(num_threads);

      _l_clusters.clear();
      _l_clusters.resize(num_threads);

      if (__storeVertices) {
        _l_marginalSets.clear();
        _l_marginalSets.resize(num_threads);
      }

      if (__storeBNOpt) {
        for (Size ptr = 0; ptr < this->_l_optimalNet.size(); ptr++)
          if (this->_l_optimalNet[ptr] != nullptr)
            delete _l_optimalNet[ptr];

        _l_optimalNet.clear();
        _l_optimalNet.resize(num_threads);
      }

      _l_modal.clear();
      _l_modal.resize(num_threads);

      __infE::_oldMarginalMin.clear();
      this->_oldMarginalMin = this->_marginalMin;
      this->_oldMarginalMax.clear();
      this->_oldMarginalMax = this->_marginalMax;
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    inline bool
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_updateThread(
        const NodeId &id, const std::vector<GUM_SCALAR> &vertex,
        const bool &elimRedund) {
      int tId = getThreadNumber();

      // save E(X) if we don't save vertices
      if (!__infE::_storeVertices && !_l_modal[tId].empty()) {
        std::string var_name = _workingSet[tId]->variable(id).name();
        auto delim = var_name.find_first_of("_");
        var_name = var_name.substr(0, delim);

        if (_l_modal[tId].exists(var_name)) {
          GUM_SCALAR exp = 0;
          Size vsize = vertex.size();

          for (Size mod = 0; mod < vsize; mod++)
            exp += vertex[mod] * _l_modal[tId][var_name][mod];

          if (exp > _l_expectationMax[tId][id])
            _l_expectationMax[tId][id] = exp;

          if (exp < _l_expectationMin[tId][id])
            _l_expectationMin[tId][id] = exp;
        }
      } // end of : if modal (map) not empty

      bool newOne = false;
      bool added = false;
      bool result = false;
      // for burn in, we need to keep checking on local marginals and not global ones
      // (faster inference)
      // we also don't want to store dbn for observed variables since there will be a
      // huge number of them (probably all of them).
      Size vsize = vertex.size();

      for (Size mod = 0; mod < vsize; mod++) {
        if (vertex[mod] < _l_marginalMin[tId][id][mod]) {
          _l_marginalMin[tId][id][mod] = vertex[mod];
          newOne = true;

          if (__infE::_storeBNOpt && !__infE::_evidence.exists(id)) {
            std::vector<unsigned int> key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 0;

            if (_l_optimalNet[tId]->insert(key, true))
              result = true;
          }
        }

        if (vertex[mod] > _l_marginalMax[tId][id][mod]) {
          _l_marginalMax[tId][id][mod] = vertex[mod];
          newOne = true;

          if (__infE::_storeBNOpt && !__infE::_evidence.exists(id)) {
            std::vector<unsigned int> key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 1;

            if (_l_optimalNet[tId]->insert(key, true))
              result = true;
          }
        } else if (vertex[mod] == _l_marginalMin[tId][id][mod] ||
                   vertex[mod] == _l_marginalMax[tId][id][mod]) {
          newOne = true;

          if (__infE::_storeBNOpt && vertex[mod] == _l_marginalMin[tId][id][mod] &&
              !__infE::_evidence.exists(id)) {
            std::vector<unsigned int> key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 0;

            if (_l_optimalNet[tId]->insert(key, false))
              result = true;
          }

          if (__infE::_storeBNOpt && vertex[mod] == _l_marginalMax[tId][id][mod] &&
              !__infE::_evidence.exists(id)) {
            std::vector<unsigned int> key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 1;

            if (_l_optimalNet[tId]->insert(key, false))
              result = true;
          }
        }

        // store point to compute credal set vertices.
        // check for redundancy at each step or at the end ?
        if (__infE::_storeVertices && !added && newOne) {
          __updateThreadCredalSets(id, vertex, elimRedund);
          added = true;
        }
      }

      // if all variables didn't get better marginals, we will delete
      if (__infE::_storeBNOpt && result)
        return true;

      return false;
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    inline void
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::__updateThreadCredalSets(
        const NodeId &id, const std::vector<GUM_SCALAR> &vertex,
        const bool &elimRedund) {
      int tId = getThreadNumber();
      auto &nodeCredalSet = _l_marginalSets[tId][id];
      Size dsize = vertex.size();

      bool eq = true;

      for (auto it = nodeCredalSet.cbegin(), itEnd = nodeCredalSet.cend();
           it != itEnd; ++it) {
        eq = true;

        for (Size i = 0; i < dsize; i++) {
          if (std::fabs(vertex[i] - (*it)[i]) > 1e-6) {
            eq = false;
            break;
          }
        }

        if (eq)
          break;
      }

      if (!eq || nodeCredalSet.size() == 0) {
        nodeCredalSet.push_back(vertex);
        return;
      } else
        return;

      /// we need this because of the next lambda return contidion fabs ( *minIt -
      /// *maxIt ) > 1e-6 which never happens if there is only one vertice
      if (nodeCredalSet.size() == 1)
        return;

      // check that the point and all previously added ones are not inside the actual
      // polytope
      auto itEnd = std::remove_if(nodeCredalSet.begin(), nodeCredalSet.end(),
                                  [&](const std::vector<GUM_SCALAR> &v) -> bool {
        for (auto jt = v.cbegin(), jtEnd = v.cend(),
                  minIt = _l_marginalMin[tId][id].cbegin(),
                  minItEnd = _l_marginalMin[tId][id].cend(),
                  maxIt = _l_marginalMax[tId][id].cbegin(),
                  maxItEnd = _l_marginalMax[tId][id].cend();
             jt != jtEnd && minIt != minItEnd && maxIt != maxItEnd;
             ++jt, ++minIt, ++maxIt) {
          if ((std::fabs(*jt - *minIt) < 1e-6 || std::fabs(*jt - *maxIt) < 1e-6) &&
              std::fabs(*minIt - *maxIt) > 1e-6)
            return false;
        }
        return true;
      });

      nodeCredalSet.erase(itEnd, nodeCredalSet.end());

      // we need at least 2 points to make a convex combination
      if (!elimRedund || nodeCredalSet.size() <= 2)
        return;

      // there may be points not inside the polytope but on one of it's facet,
      // meaning it's still a convex combination of vertices of this facet. Here we
      // need lrs.
      auto setSize = nodeCredalSet.size();

      LRSWrapper<GUM_SCALAR> lrsWrapper;
      lrsWrapper.setUpV(dsize, setSize);

      for (const auto &vtx : nodeCredalSet)
        lrsWrapper.fillV(vtx);

      lrsWrapper.elimRedundVrep();

      _l_marginalSets[tId][id] = lrsWrapper.getOutput();
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    inline void
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_updateMarginals() {
#pragma omp parallel
      {
        int threadId = getThreadNumber();
        auto nsize = _workingSet[threadId]->size();

#pragma omp for

        for (Size i = 0; i < nsize; i++) {
          Size dSize = _l_marginalMin[threadId][i].size();

          for (Size j = 0; j < dSize; j++) {
            Size tsize = _l_marginalMin.size();

            // go through all threads
            for (Size tId = 0; tId < tsize; tId++) {
              if (_l_marginalMin[tId][i][j] < this->_marginalMin[i][j])
                this->_marginalMin[i][j] = _l_marginalMin[tId][i][j];

              if (_l_marginalMax[tId][i][j] > this->_marginalMax[i][j])
                this->_marginalMax[i][j] = _l_marginalMax[tId][i][j];
            } // end of : all threads
          }   // end of : all modalities
        }     // end of : all variables
      }       // end of : parallel region
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    inline const GUM_SCALAR
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_computeEpsilon() {
      GUM_SCALAR eps = 0;
#pragma omp parallel
      {
        GUM_SCALAR tEps = 0;
        GUM_SCALAR delta;

        int tId = getThreadNumber();
        Size nsize = _workingSet[tId]->size();

#pragma omp for

        for (Size i = 0; i < nsize; i++) {
          Size dSize = _l_marginalMin[tId][i].size();

          for (Size j = 0; j < dSize; j++) {
            // on min
            delta = this->_marginalMin[i][j] - this->_oldMarginalMin[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps = (tEps < delta) ? delta : tEps;

            // on max
            delta = this->_marginalMax[i][j] - this->_oldMarginalMax[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps = (tEps < delta) ? delta : tEps;

            this->_oldMarginalMin[i][j] = this->_marginalMin[i][j];
            this->_oldMarginalMax[i][j] = this->_marginalMax[i][j];
          }
        } // end of : all variables

#pragma omp critical(epsilon_max)
        {
#pragma omp flush(eps)
          eps = (eps < tEps) ? tEps : eps;
        }

      } // end of : parallel region
      return eps;
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    void
    MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_updateOldMarginals() {
#pragma omp parallel
      {
        int threadId = getThreadNumber();
        Size nsize = _workingSet[threadId]->size();

#pragma omp for

        for (Size i = 0; i < nsize; i++) {
          Size dSize = _l_marginalMin[threadId][i].size();

          for (Size j = 0; j < dSize; j++) {
            Size tsize = _l_marginalMin.size();

            // go through all threads
            for (Size tId = 0; tId < tsize; tId++) {
              if (_l_marginalMin[tId][i][j] < this->_oldMarginalMin[i][j])
                this->_oldMarginalMin[i][j] = _l_marginalMin[tId][i][j];

              if (_l_marginalMax[tId][i][j] > this->_oldMarginalMax[i][j])
                this->_oldMarginalMax[i][j] = _l_marginalMax[tId][i][j];
            } // end of : all threads
          }   // end of : all modalities
        }     // end of : all variables
      }       // end of : parallel region
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    void MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_verticesFusion() {
      // don't create threads if there are no vertices saved
      if (!__infE::_storeVertices)
        return;

#pragma omp parallel
      {
        int threadId = getThreadNumber();
        Size nsize = _workingSet[threadId]->size();

#pragma omp for

        for (Size i = 0; i < nsize; i++) {
          Size tsize = _l_marginalMin.size();
          Size dSize = _l_marginalMin[threadId][i].size();

          // go through all threads
          for (Size tId = 0; tId < tsize; tId++) {
            auto &nodeThreadCredalSet = _l_marginalSets[tId][i];

            // for each vertex, if we are at any opt marginal, add it to the set
            for (const auto &vtx : nodeThreadCredalSet) {
              // we run redundancy elimination at each step
              // because there could be 100000 threads and the set will be so huge
              // ...
              // BUT not if vertices are of dimension 2 ! opt check and equality
              // should be enough
              __infE::_updateCredalSets(i, vtx, (vtx.size() > 2) ? true : false);
            } // end of : nodeThreadCredalSet
          }   // end of : all threads
        }     // end of : all variables
      }       // end of : parallel region
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    void MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_expFusion() {
      // don't create threads if there are no modalities to compute expectations
      if (this->_modal.empty())
        return;

      // we can compute expectations from vertices of the final credal set
      if (__infE::_storeVertices) {
#pragma omp parallel
        {
          int threadId = getThreadNumber();

          if (!this->_l_modal[threadId].empty()) {
            Size nsize = _workingSet[threadId]->size();

#pragma omp for

            for (Size i = 0; i < nsize; i++) {
              std::string var_name = _workingSet[threadId]->variable(i).name();
              auto delim = var_name.find_first_of("_");
              std::string time_step = var_name.substr(delim + 1, var_name.size());
              var_name = var_name.substr(0, delim);

              if (!_l_modal[threadId].exists(var_name))
                continue;

              Size setsize = __infE::_marginalSets[i].size();

              for (const auto &vertex : __infE::_marginalSets[i]) {
                GUM_SCALAR exp = 0;
                Size vsize = vertex.size();

                for (Size mod = 0; mod < vsize; mod++)
                  exp += vertex[mod] * _l_modal[threadId][var_name][mod];

                if (exp > __infE::_expectationMax[i])
                  __infE::_expectationMax[i] = exp;

                if (exp < __infE::_expectationMin[i])
                  __infE::_expectationMin[i] = exp;
              }
            } // end of : each variable parallel for
          }   // end of : if this thread has modals
        }     // end of parallel region
        return;
      }

#pragma omp parallel
      {
        int threadId = getThreadNumber();

        if (!this->_l_modal[threadId].empty()) {
          Size nsize = _workingSet[threadId]->size();

#pragma omp for

          for (Size i = 0; i < nsize; i++) {
            std::string var_name = _workingSet[threadId]->variable(i).name();
            auto delim = var_name.find_first_of("_");
            std::string time_step = var_name.substr(delim + 1, var_name.size());
            var_name = var_name.substr(0, delim);

            if (!_l_modal[threadId].exists(var_name))
              continue;

            Size tsize = _l_expectationMax.size();

            for (Size tId = 0; tId < tsize; tId++) {
              if (_l_expectationMax[tId][i] > this->_expectationMax[i])
                this->_expectationMax[i] = _l_expectationMax[tId][i];

              if (_l_expectationMin[tId][i] < this->_expectationMin[i])
                this->_expectationMin[i] = _l_expectationMin[tId][i];
            } // end of : each thread
          }   // end of : each variable
        }     // end of : if modals not empty
      }       // end of : parallel region
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    void MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::_optFusion() {
      typedef std::vector<bool> dBN;

      Size nsize = _workingSet[0]->size();

      // no parallel insert in hash-tables (OptBN)
      for (Size i = 0; i < nsize; i++) {

        // we don't store anything for observed variables
        if (__infE::_evidence.exists(i))
          continue;

        Size dSize = _l_marginalMin[0][i].size();

        for (Size j = 0; j < dSize; j++) {
          // go through all threads
          std::vector<unsigned int> keymin(3);
          keymin[0] = i;
          keymin[1] = j;
          keymin[2] = 0;
          std::vector<unsigned int> keymax(keymin);
          keymax[2] = 1;

          Size tsize = _l_marginalMin.size();

          for (Size tId = 0; tId < tsize; tId++) {
            if (_l_marginalMin[tId][i][j] == this->_marginalMin[i][j]) {
              const std::vector<dBN *> &tOpts =
                  _l_optimalNet[tId]->getBNOptsFromKey(keymin);
              Size osize = tOpts.size();

              for (Size bn = 0; bn < osize; bn++) {
                __infE::_dbnOpt.insert(*tOpts[bn], keymin);
              }
            }

            if (_l_marginalMax[tId][i][j] == this->_marginalMax[i][j]) {
              const std::vector<dBN *> &tOpts =
                  _l_optimalNet[tId]->getBNOptsFromKey(keymax);
              Size osize = tOpts.size();

              for (Size bn = 0; bn < osize; bn++) {
                __infE::_dbnOpt.insert(*tOpts[bn], keymax);
              }
            }
          } // end of : all threads
        }   // end of : all modalities
      }     // end of : all variables
    }

    template <typename GUM_SCALAR, class BNInferenceEngine>
    void MultipleInferenceEngine<GUM_SCALAR, BNInferenceEngine>::eraseAllEvidence() {
      __infE::eraseAllEvidence();
      Size tsize = _workingSet.size();

      // delete pointers
      for (Size bn = 0; bn < tsize; bn++) {
        if (__infE::_storeVertices)
          _l_marginalSets[bn].clear();

        if (_workingSet[bn] != nullptr)
          delete _workingSet[bn];

        if (__infE::_storeBNOpt)
          if (_l_inferenceEngine[bn] != nullptr)
            delete _l_optimalNet[bn];

        if (this->_workingSetE[bn] != nullptr) {
          for (const auto ev : *_workingSetE[bn])
            delete ev;

          delete _workingSetE[bn];
        }

        if (_l_inferenceEngine[bn] != nullptr)
          delete _l_inferenceEngine[bn];
      }

      // this is important, those will be resized with the correct number of threads.

      _workingSet.clear();
      _workingSetE.clear();
      _l_inferenceEngine.clear();
      _l_optimalNet.clear();

      _l_marginalMin.clear();
      _l_marginalMax.clear();
      _l_expectationMin.clear();
      _l_expectationMax.clear();
      _l_modal.clear();
      _l_marginalSets.clear();
      _l_evidence.clear();
      _l_clusters.clear();
    }
  }
}

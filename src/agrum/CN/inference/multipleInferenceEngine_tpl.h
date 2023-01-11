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


#include <agrum/CN/inference/inferenceEngine.h>
#include <agrum/CN/inference/multipleInferenceEngine.h>

namespace gum {
  namespace credal {

    template < typename GUM_SCALAR, class BNInferenceEngine >
    MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::MultipleInferenceEngine(
       const CredalNet< GUM_SCALAR >& credalNet) :
        InferenceEngine< GUM_SCALAR >::InferenceEngine(credalNet) {
      GUM_CONSTRUCTOR(MultipleInferenceEngine);
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::~MultipleInferenceEngine() {
      GUM_DESTRUCTOR(MultipleInferenceEngine);
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    inline void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::initThreadsData_(
       const Size& num_threads,
       const bool  _storeVertices_,
       const bool  _storeBNOpt_) {
      workingSet_.clear();
      workingSet_.resize(num_threads, nullptr);
      workingSetE_.clear();
      workingSetE_.resize(num_threads, nullptr);

      l_marginalMin_.clear();
      l_marginalMin_.resize(num_threads);
      l_marginalMax_.clear();
      l_marginalMax_.resize(num_threads);
      l_expectationMin_.clear();
      l_expectationMin_.resize(num_threads);
      l_expectationMax_.clear();
      l_expectationMax_.resize(num_threads);

      l_clusters_.clear();
      l_clusters_.resize(num_threads);

      if (_storeVertices_) {
        l_marginalSets_.clear();
        l_marginalSets_.resize(num_threads);
      }

      if (_storeBNOpt_) {
        for (Size ptr = 0; ptr < this->l_optimalNet_.size(); ptr++)
          if (this->l_optimalNet_[ptr] != nullptr) delete l_optimalNet_[ptr];

        l_optimalNet_.clear();
        l_optimalNet_.resize(num_threads);
      }

      l_modal_.clear();
      l_modal_.resize(num_threads);

      _infE_::oldMarginalMin_.clear();
      this->oldMarginalMin_ = this->marginalMin_;
      this->oldMarginalMax_.clear();
      this->oldMarginalMax_ = this->marginalMax_;

      // init the random number generators
      generators_.clear();
      generators_.resize(num_threads);
      auto seed = currentRandomGeneratorValue();
      for (auto& generator: generators_) {
        generator.seed(seed);
        seed = generator();
      }
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    inline bool MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::updateThread_(
       Size                             tId,
       const NodeId&                    id,
       const std::vector< GUM_SCALAR >& vertex,
       const bool&                      elimRedund) {
      // save E(X) if we don't save vertices
      if (!_infE_::storeVertices_ && !l_modal_[tId].empty()) {
        std::string var_name = workingSet_[tId]->variable(id).name();
        auto        delim    = var_name.find_first_of("_");
        var_name             = var_name.substr(0, delim);

        if (l_modal_[tId].exists(var_name)) {
          GUM_SCALAR exp   = 0;
          Size       vsize = Size(vertex.size());

          for (Size mod = 0; mod < vsize; mod++)
            exp += vertex[mod] * l_modal_[tId][var_name][mod];

          if (exp > l_expectationMax_[tId][id]) l_expectationMax_[tId][id] = exp;

          if (exp < l_expectationMin_[tId][id]) l_expectationMin_[tId][id] = exp;
        }
      }   // end of : if modal (map) not empty

      bool newOne = false;
      bool added  = false;
      bool result = false;
      // for burn in, we need to keep checking on local marginals and not global
      // ones
      // (faster inference)
      // we also don't want to store dbn for observed variables since there will
      // be a
      // huge number of them (probably all of them).
      Size vsize = Size(vertex.size());

      for (Size mod = 0; mod < vsize; mod++) {
        if (vertex[mod] < l_marginalMin_[tId][id][mod]) {
          l_marginalMin_[tId][id][mod] = vertex[mod];
          newOne                       = true;

          if (_infE_::storeBNOpt_ && !_infE_::evidence_.exists(id)) {
            std::vector< Size > key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 0;

            if (l_optimalNet_[tId]->insert(key, true)) result = true;
          }
        }

        if (vertex[mod] > l_marginalMax_[tId][id][mod]) {
          l_marginalMax_[tId][id][mod] = vertex[mod];
          newOne                       = true;

          if (_infE_::storeBNOpt_ && !_infE_::evidence_.exists(id)) {
            std::vector< Size > key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 1;

            if (l_optimalNet_[tId]->insert(key, true)) result = true;
          }
        } else if (vertex[mod] == l_marginalMin_[tId][id][mod]
                   || vertex[mod] == l_marginalMax_[tId][id][mod]) {
          newOne = true;

          if (_infE_::storeBNOpt_ && vertex[mod] == l_marginalMin_[tId][id][mod]
              && !_infE_::evidence_.exists(id)) {
            std::vector< Size > key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 0;

            if (l_optimalNet_[tId]->insert(key, false)) result = true;
          }

          if (_infE_::storeBNOpt_ && vertex[mod] == l_marginalMax_[tId][id][mod]
              && !_infE_::evidence_.exists(id)) {
            std::vector< Size > key(3);
            key[0] = id;
            key[1] = mod;
            key[2] = 1;

            if (l_optimalNet_[tId]->insert(key, false)) result = true;
          }
        }

        // store point to compute credal set vertices.
        // check for redundancy at each step or at the end ?
        if (_infE_::storeVertices_ && !added && newOne) {
          _updateThreadCredalSets_(tId, id, vertex, elimRedund);
          added = true;
        }
      }

      // if all variables didn't get better marginals, we will delete
      if (_infE_::storeBNOpt_ && result) return true;

      return false;
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    inline void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::_updateThreadCredalSets_(
       Size                             tId,
       const NodeId&                    id,
       const std::vector< GUM_SCALAR >& vertex,
       const bool&                      elimRedund) {
      auto& nodeCredalSet = l_marginalSets_[tId][id];
      Size  dsize         = Size(vertex.size());

      bool eq = true;

      for (auto it = nodeCredalSet.cbegin(), itEnd = nodeCredalSet.cend(); it != itEnd; ++it) {
        eq = true;

        for (Size i = 0; i < dsize; i++) {
          if (std::fabs(vertex[i] - (*it)[i]) > 1e-6) {
            eq = false;
            break;
          }
        }

        if (eq) break;
      }

      if (!eq || nodeCredalSet.size() == 0) {
        nodeCredalSet.push_back(vertex);
        return;
      } else return;

      /// we need this because of the next lambda return contidion fabs ( *minIt
      /// - *maxIt ) > 1e-6 which never happens if there is only one vertice
      if (nodeCredalSet.size() == 1) return;

      // check that the point and all previously added ones are not inside the
      // actual
      // polytope
      auto itEnd
         = std::remove_if(nodeCredalSet.begin(),
                          nodeCredalSet.end(),
                          [&](const std::vector< GUM_SCALAR >& v) -> bool {
                            for (auto jt       = v.cbegin(),
                                      jtEnd    = v.cend(),
                                      minIt    = l_marginalMin_[tId][id].cbegin(),
                                      minItEnd = l_marginalMin_[tId][id].cend(),
                                      maxIt    = l_marginalMax_[tId][id].cbegin(),
                                      maxItEnd = l_marginalMax_[tId][id].cend();
                                 jt != jtEnd && minIt != minItEnd && maxIt != maxItEnd;
                                 ++jt, ++minIt, ++maxIt) {
                              if ((std::fabs(*jt - *minIt) < 1e-6 || std::fabs(*jt - *maxIt) < 1e-6)
                                  && std::fabs(*minIt - *maxIt) > 1e-6)
                                return false;
                            }
                            return true;
                          });

      nodeCredalSet.erase(itEnd, nodeCredalSet.end());

      // we need at least 2 points to make a convex combination
      if (!elimRedund || nodeCredalSet.size() <= 2) return;

      // there may be points not inside the polytope but on one of it's facet,
      // meaning it's still a convex combination of vertices of this facet. Here
      // we need lrs.
      Size setSize = Size(nodeCredalSet.size());

      LRSWrapper< GUM_SCALAR > lrsWrapper;
      lrsWrapper.setUpV(dsize, setSize);

      for (const auto& vtx: nodeCredalSet)
        lrsWrapper.fillV(vtx);

      lrsWrapper.elimRedundVrep();

      l_marginalSets_[tId][id] = lrsWrapper.getOutput();
    }


    template < typename GUM_SCALAR, class BNInferenceEngine >
    inline void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::updateMarginals_() {
      // compute the max number of threads to use (avoid nested threads)
      const Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                               ? this->threadRanges_.size() - 1
                               : 1;   // no nested multithreading

      // create the function to be executed by the threads
      auto threadedExec = [this](const std::size_t                           this_thread,
                                 const std::size_t                           nb_threads,
                                 const std::vector< std::pair< Idx, Idx > >& ranges) {
        auto       i                = this->threadRanges_[this_thread].first;
        auto       j                = this->threadRanges_[this_thread].second;
        auto       domain_size      = this->marginalMax_[i].size();
        const auto end_i            = this->threadRanges_[this_thread + 1].first;
        auto       end_j            = this->threadRanges_[this_thread + 1].second;
        const auto marginalMax_size = this->marginalMax_.size();
        const auto tsize            = Size(l_marginalMin_.size());

        while ((i < end_i) || (j < end_j)) {
          // go through all work indices
          for (Idx tId = 0; tId < tsize; tId++) {
            if (l_marginalMin_[tId][i][j] < this->marginalMin_[i][j])
              this->marginalMin_[i][j] = l_marginalMin_[tId][i][j];

            if (l_marginalMax_[tId][i][j] > this->marginalMax_[i][j])
              this->marginalMax_[i][j] = l_marginalMax_[tId][i][j];
          }

          if (++j == domain_size) {
            j = 0;
            ++i;
            if (i < marginalMax_size) domain_size = this->marginalMax_[i].size();
          }
        }
      };

      // launch the threads
      ThreadExecutor::execute(
         nb_threads,
         threadedExec,
         (nb_threads == 1)
            ? std::vector< std::pair< NodeId, Idx > >{{0, 0}, {this->marginalMin_.size(), 0}}
            : this->threadRanges_);
    }

    /*
      // old openMP code :
      #pragma omp parallel
      {
        int  threadId = threadsOMP::getThreadNumber();
        long nsize    = long(workingSet_[threadId]->size());

        #pragma omp for
        for (long i = 0; i < nsize; i++) {
          Size dSize = Size(l_marginalMin_[threadId][i].size());

          for (Size j = 0; j < dSize; j++) {
            Size tsize = Size(l_marginalMin_.size());

            // go through all threads
            for (Size tId = 0; tId < tsize; tId++) {
              if (l_marginalMin_[tId][i][j] < this->marginalMin_[i][j])
                this->marginalMin_[i][j] = l_marginalMin_[tId][i][j];

              if (l_marginalMax_[tId][i][j] > this->marginalMax_[i][j])
                this->marginalMax_[i][j] = l_marginalMax_[tId][i][j];
            }   // end of : all threads
          }     // end of : all modalities
        }       // end of : all variables
      }         // end of : parallel region
    }
    */


    template < typename GUM_SCALAR, class BNInferenceEngine >
    inline const GUM_SCALAR
       MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::computeEpsilon_() {
      // compute the number of threads (avoid nested threads)
      const Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                               ? this->threadRanges_.size() - 1
                               : 1;   // no nested multithreading

      std::vector< GUM_SCALAR > tEps(nb_threads, 0);

      // create the function to be executed by the threads
      auto threadedExec = [this, &tEps](const std::size_t                           this_thread,
                                        const std::size_t                           nb_threads,
                                        const std::vector< std::pair< Idx, Idx > >& ranges) {
        auto&      this_tEps = tEps[this_thread];
        GUM_SCALAR delta     = 0;

        auto       i                = this->threadRanges_[this_thread].first;
        auto       j                = this->threadRanges_[this_thread].second;
        auto       domain_size      = this->marginalMax_[i].size();
        const auto end_i            = this->threadRanges_[this_thread + 1].first;
        auto       end_j            = this->threadRanges_[this_thread + 1].second;
        const auto marginalMax_size = this->marginalMax_.size();

        while ((i < end_i) || (j < end_j)) {
          // on min
          delta = this->marginalMin_[i][j] - this->oldMarginalMin_[i][j];
          delta = (delta < 0) ? (-delta) : delta;
          if (this_tEps < delta) this_tEps = delta;

          // on max
          delta = this->marginalMax_[i][j] - this->oldMarginalMax_[i][j];
          delta = (delta < 0) ? (-delta) : delta;
          if (this_tEps < delta) this_tEps = delta;

          this->oldMarginalMin_[i][j] = this->marginalMin_[i][j];
          this->oldMarginalMax_[i][j] = this->marginalMax_[i][j];

          if (++j == domain_size) {
            j = 0;
            ++i;
            if (i < marginalMax_size) domain_size = this->marginalMax_[i].size();
          }
        }
      };

      // launch the threads
      ThreadExecutor::execute(
         nb_threads,
         threadedExec,
         (nb_threads == 1)
            ? std::vector< std::pair< NodeId, Idx > >{{0, 0}, {this->marginalMin_.size(), 0}}
            : this->threadRanges_);

      // aggregate all the results
      GUM_SCALAR eps = tEps[0];
      for (const auto nb: tEps)
        if (eps < nb) eps = nb;

      return eps;
    }

    /*
     // old openMP code :
      GUM_SCALAR eps = 0;
      #pragma omp parallel
      {
        GUM_SCALAR tEps = 0;
        GUM_SCALAR delta;

        int  tId   = threadsOMP::getThreadNumber();
        long nsize = long(workingSet_[tId]->size());

        #pragma omp for
        for (long i = 0; i < nsize; i++) {
          Size dSize = Size(l_marginalMin_[tId][i].size());
          std::cout << omp_get_num_threads() << std::endl;

          for (Size j = 0; j < dSize; j++) {
            // on min
            delta = this->marginalMin_[i][j] - this->oldMarginalMin_[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps  = (tEps < delta) ? delta : tEps;

            // on max
            delta = this->marginalMax_[i][j] - this->oldMarginalMax_[i][j];
            delta = (delta < 0) ? (-delta) : delta;
            tEps  = (tEps < delta) ? delta : tEps;

            this->oldMarginalMin_[i][j] = this->marginalMin_[i][j];
            this->oldMarginalMax_[i][j] = this->marginalMax_[i][j];
          }
        }   // end of : all variables

        #pragma omp critical(epsilon_max)
        {
        #pragma omp flush(eps)
          eps = (eps < tEps) ? tEps : eps;
        }

      }   // end of : parallel region
      return eps;
    }
    */

    template < typename GUM_SCALAR, class BNInferenceEngine >
    void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::updateOldMarginals_() {
#pragma omp parallel
      {
        int  threadId = threadsOMP::getThreadNumber();
        long nsize    = long(workingSet_[threadId]->size());

#pragma omp for

        for (long i = 0; i < nsize; i++) {
          Size dSize = Size(l_marginalMin_[threadId][i].size());

          for (Size j = 0; j < dSize; j++) {
            Size tsize = Size(l_marginalMin_.size());

            // go through all threads
            for (Size tId = 0; tId < tsize; tId++) {
              if (l_marginalMin_[tId][i][j] < this->oldMarginalMin_[i][j])
                this->oldMarginalMin_[i][j] = l_marginalMin_[tId][i][j];

              if (l_marginalMax_[tId][i][j] > this->oldMarginalMax_[i][j])
                this->oldMarginalMax_[i][j] = l_marginalMax_[tId][i][j];
            }   // end of : all threads
          }     // end of : all modalities
        }       // end of : all variables
      }         // end of : parallel region
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::verticesFusion_() {
      // don't create threads if there are no vertices saved
      if (!_infE_::storeVertices_) return;

      // compute the max number of threads to use (avoid nested threads)
      const Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                               ? ThreadNumberManager::getNumberOfThreads()
                               : 1;   // no nested multithreading

      // create the function to be executed by the threads
      Size tsize        = Size(l_marginalMin_.size());
      auto threadedExec = [this, tsize](const std::size_t                           this_thread,
                                        const std::size_t                           nb_threads,
                                        Idx                                         work_index,
                                        const std::vector< std::pair< Idx, Idx > >& ranges) {
        for (Idx i = ranges[this_thread].first, end = ranges[this_thread].second; i < end; i++) {
          // go through all threads
          for (Size tId = 0; tId < tsize; ++tId) {
            auto& nodeThreadCredalSet = l_marginalSets_[tId][i];

            // for each vertex, if we are at any opt marginal, add it to the set
            for (const auto& vtx: nodeThreadCredalSet) {
              // we run redundancy elimination at each step because there could
              // be 100000 threads and the set will be so huge...
              // BUT not if vertices are of dimension 2 ! opt check and equality
              // should be enough
              _infE_::updateCredalSets_(i, vtx, (vtx.size() > 2) ? true : false);
            }   // end of : nodeThreadCredalSet
          }     // end of : all threads
        }       // end of : all variables
      };

      const Size working_size = workingSet_.size();
      for (Idx work_index = 0; work_index < working_size; ++work_index) {
        // compute the ranges over which the threads will work
        const auto nsize           = workingSet_[work_index]->size();
        const auto real_nb_threads = std::min(nb_threads, nsize);
        const auto ranges = gum::dispatchRangeToThreads(0, nsize, (unsigned int)(real_nb_threads));
        ThreadExecutor::execute(real_nb_threads, threadedExec, work_index, ranges);
      }
    }

    /*
      // old openMP code :
      #pragma omp parallel
      {
        int  threadId = threadsOMP::getThreadNumber();
        Size nsize    = Size(workingSet_[threadId]->size());

        #pragma omp for
        for (long i = 0; i < long(nsize); i++) {
          Size tsize = Size(l_marginalMin_.size());

          // go through all threads
          for (long tId = 0; tId < long(tsize); tId++) {
            auto& nodeThreadCredalSet = l_marginalSets_[tId][i];

            // for each vertex, if we are at any opt marginal, add it to the set
            for (const auto& vtx: nodeThreadCredalSet) {
              // we run redundancy elimination at each step
              // because there could be 100000 threads and the set will be so
              // huge
              // ...
              // BUT not if vertices are of dimension 2 ! opt check and equality
              // should be enough
              _infE_::updateCredalSets_(i, vtx, (vtx.size() > 2) ? true : false);
            }   // end of : nodeThreadCredalSet
          }     // end of : all threads
        }       // end of : all variables
      }         // end of : parallel region
    }
    */


    template < typename GUM_SCALAR, class BNInferenceEngine >
    void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::expFusion_() {
      // don't create threads if there are no modalities to compute expectations
      if (this->modal_.empty()) return;

      // compute the max number of threads to use (avoid nested threads)
      const Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                               ? ThreadNumberManager::getNumberOfThreads()
                               : 1;   // no nested multithreading

      // we can compute expectations from vertices of the final credal set
      if (_infE_::storeVertices_) {
        // create the function to be executed by the threads
        auto threadedExec = [this](const std::size_t                           this_thread,
                                   const std::size_t                           nb_threads,
                                   Idx                                         work_index,
                                   const std::vector< std::pair< Idx, Idx > >& ranges) {
          for (Idx i = ranges[this_thread].first, end = ranges[this_thread].second; i < end; i++) {
            std::string var_name = workingSet_[work_index]->variable(i).name();
            auto        delim    = var_name.find_first_of("_");
            var_name             = var_name.substr(0, delim);

            if (!l_modal_[work_index].exists(var_name)) continue;

            for (const auto& vertex: _infE_::marginalSets_[i]) {
              GUM_SCALAR exp   = 0;
              Size       vsize = Size(vertex.size());

              for (Size mod = 0; mod < vsize; mod++)
                exp += vertex[mod] * l_modal_[work_index][var_name][mod];

              if (exp > _infE_::expectationMax_[i]) _infE_::expectationMax_[i] = exp;

              if (exp < _infE_::expectationMin_[i]) _infE_::expectationMin_[i] = exp;
            }
          }
        };

        const Size working_size = workingSet_.size();
        for (Idx work_index = 0; work_index < working_size; ++work_index) {
          if (!this->l_modal_[work_index].empty()) {
            // compute the ranges over which the threads will work
            const auto nsize           = workingSet_[work_index]->size();
            const auto real_nb_threads = std::min(nb_threads, nsize);
            const auto ranges
               = gum::dispatchRangeToThreads(0, nsize, (unsigned int)(real_nb_threads));
            ThreadExecutor::execute(real_nb_threads, threadedExec, work_index, ranges);
          }
        }

        return;
      }

      // create the function to be executed by the threads
      auto threadedExec = [this](const std::size_t                           this_thread,
                                 const std::size_t                           nb_threads,
                                 Idx                                         work_index,
                                 const std::vector< std::pair< Idx, Idx > >& ranges) {
        for (Idx i = ranges[this_thread].first, end = ranges[this_thread].second; i < end; i++) {
          std::string var_name = workingSet_[work_index]->variable(i).name();
          auto        delim    = var_name.find_first_of("_");
          var_name             = var_name.substr(0, delim);

          if (!l_modal_[work_index].exists(var_name)) continue;

          Size tsize = Size(l_expectationMax_.size());

          for (Idx tId = 0; tId < tsize; tId++) {
            if (l_expectationMax_[tId][i] > this->expectationMax_[i])
              this->expectationMax_[i] = l_expectationMax_[tId][i];

            if (l_expectationMin_[tId][i] < this->expectationMin_[i])
              this->expectationMin_[i] = l_expectationMin_[tId][i];
          }   // end of : each thread
        }     // end of : each variable
      };

      const Size working_size = workingSet_.size();
      for (Idx work_index = 0; work_index < working_size; ++work_index) {
        if (!this->l_modal_[work_index].empty()) {
          const auto nsize           = Size(workingSet_[work_index]->size());
          const auto real_nb_threads = std::min(nb_threads, nsize);
          const auto ranges
             = gum::dispatchRangeToThreads(0, nsize, (unsigned int)(real_nb_threads));
          ThreadExecutor::execute(real_nb_threads, threadedExec, work_index, ranges);
        }
      }
    }

    /*
      // old openMP code :
      // don't create threads if there are no modalities to compute expectations
      if (this->modal_.empty()) return;

      // we can compute expectations from vertices of the final credal set
      if (_infE_::storeVertices_) {
      #pragma omp parallel
        {
          int threadId = threadsOMP::getThreadNumber();

          if (!this->l_modal_[threadId].empty()) {
            Size nsize = Size(workingSet_[threadId]->size());

            #pragma omp for
            for (long i = 0; i < long(nsize); i++) {   // i needs to be signed (due to omp with
                                                       // visual c++ 15)
              std::string var_name = workingSet_[threadId]->variable(i).name();
              auto        delim    = var_name.find_first_of("_");
              var_name             = var_name.substr(0, delim);

              if (!l_modal_[threadId].exists(var_name)) continue;

              for (const auto& vertex: _infE_::marginalSets_[i]) {
                GUM_SCALAR exp   = 0;
                Size       vsize = Size(vertex.size());

                for (Size mod = 0; mod < vsize; mod++)
                  exp += vertex[mod] * l_modal_[threadId][var_name][mod];

                if (exp > _infE_::expectationMax_[i]) _infE_::expectationMax_[i] = exp;

                if (exp < _infE_::expectationMin_[i]) _infE_::expectationMin_[i] = exp;
              }
            }   // end of : each variable parallel for
          }     // end of : if this thread has modals
        }       // end of parallel region
        return;
      }

      #pragma omp parallel
      {
        int threadId = threadsOMP::getThreadNumber();

        if (!this->l_modal_[threadId].empty()) {
          Size nsize = Size(workingSet_[threadId]->size());
          #pragma omp for
          for (long i = 0; i < long(nsize);
               i++) {   // long instead of Idx due to omp for visual C++15
            std::string var_name = workingSet_[threadId]->variable(i).name();
            auto        delim    = var_name.find_first_of("_");
            var_name             = var_name.substr(0, delim);

            if (!l_modal_[threadId].exists(var_name)) continue;

            Size tsize = Size(l_expectationMax_.size());

            for (Idx tId = 0; tId < tsize; tId++) {
              if (l_expectationMax_[tId][i] > this->expectationMax_[i])
                this->expectationMax_[i] = l_expectationMax_[tId][i];

              if (l_expectationMin_[tId][i] < this->expectationMin_[i])
                this->expectationMin_[i] = l_expectationMin_[tId][i];
            }   // end of : each thread
          }     // end of : each variable
        }       // end of : if modals not empty
      }         // end of : parallel region
    }
    */

    template < typename GUM_SCALAR, class BNInferenceEngine >
    void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::optFusion_() {
      using dBN = std::vector< bool >;

      Size nsize = Size(workingSet_[0]->size());

      // no parallel insert in hash-tables (OptBN)
      for (Idx i = 0; i < nsize; i++) {
        // we don't store anything for observed variables
        if (_infE_::evidence_.exists(i)) continue;

        Size dSize = Size(l_marginalMin_[0][i].size());

        for (Size j = 0; j < dSize; j++) {
          // go through all threads
          std::vector< Size > keymin(3);
          keymin[0] = i;
          keymin[1] = j;
          keymin[2] = 0;
          std::vector< Size > keymax(keymin);
          keymax[2] = 1;

          Size tsize = Size(l_marginalMin_.size());

          for (Size tId = 0; tId < tsize; tId++) {
            if (l_marginalMin_[tId][i][j] == this->marginalMin_[i][j]) {
              const std::vector< dBN* >& tOpts = l_optimalNet_[tId]->getBNOptsFromKey(keymin);
              Size                       osize = Size(tOpts.size());

              for (Size bn = 0; bn < osize; bn++) {
                _infE_::dbnOpt_.insert(*tOpts[bn], keymin);
              }
            }

            if (l_marginalMax_[tId][i][j] == this->marginalMax_[i][j]) {
              const std::vector< dBN* >& tOpts = l_optimalNet_[tId]->getBNOptsFromKey(keymax);
              Size                       osize = Size(tOpts.size());

              for (Size bn = 0; bn < osize; bn++) {
                _infE_::dbnOpt_.insert(*tOpts[bn], keymax);
              }
            }
          }   // end of : all threads
        }     // end of : all modalities
      }       // end of : all variables
    }

    template < typename GUM_SCALAR, class BNInferenceEngine >
    void MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::eraseAllEvidence() {
      _infE_::eraseAllEvidence();
      Size tsize = Size(workingSet_.size());

      // delete pointers
      for (Size bn = 0; bn < tsize; bn++) {
        if (_infE_::storeVertices_) l_marginalSets_[bn].clear();

        if (workingSet_[bn] != nullptr) delete workingSet_[bn];

        if (_infE_::storeBNOpt_)
          if (l_inferenceEngine_[bn] != nullptr) delete l_optimalNet_[bn];

        if (this->workingSetE_[bn] != nullptr) {
          for (const auto ev: *workingSetE_[bn])
            delete ev;

          delete workingSetE_[bn];
        }

        if (l_inferenceEngine_[bn] != nullptr) delete l_inferenceEngine_[bn];
      }

      // this is important, those will be resized with the correct number of
      // threads.

      workingSet_.clear();
      workingSetE_.clear();
      l_inferenceEngine_.clear();
      l_optimalNet_.clear();

      l_marginalMin_.clear();
      l_marginalMax_.clear();
      l_expectationMin_.clear();
      l_expectationMax_.clear();
      l_modal_.clear();
      l_marginalSets_.clear();
      l_evidence_.clear();
      l_clusters_.clear();
    }

  }   // namespace credal
}   // namespace gum

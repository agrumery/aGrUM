/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

#include <agrum/CN/inference/CNMonteCarloSampling.h>
#include <agrum/tools/core/exceptions.h>

namespace gum::credal {

  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::CNMonteCarloSampling(
     const CredalNet< GUM_SCALAR >& credalNet) :
      MultipleInferenceEngine< GUM_SCALAR, BNInferenceEngine >::MultipleInferenceEngine(credalNet) {
    _infEs_::repetitiveInd_ = false;
    _infEs_::storeVertices_ = false;
    _infEs_::storeBNOpt_    = false;

    this->setMaxTime(60);
    this->enableMaxTime();

    this->setPeriodSize(1000);

    GUM_CONSTRUCTOR(CNMonteCarloSampling)
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::~CNMonteCarloSampling() {
    GUM_DESTRUCTOR(CNMonteCarloSampling)
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::makeInference() {
    if (_infEs_::repetitiveInd_) {
      try {
        this->repetitiveInit_();
      } catch (InvalidArgument const& err) {
        GUM_SHOWERROR(err)
        _infEs_::repetitiveInd_ = false;
      }
    }
    _mcInitApproximationScheme_();
    _mcThreadDataCopy_();

    // don't put it after burnIn, it could stop with timeout : we want at
    // least one burnIn and one periodSize
    GUM_SCALAR eps = 1.;   // to validate testSuite ?

    auto psize = this->periodSize();

    if (this->continueApproximationScheme(eps)) {
      // compute the number of threads to use
      const Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                               ? this->getNumberOfThreads()
                               : 1;   // no nested multithreading

      // dispatch {0,...,psize} among the threads
      const auto ranges = gum::dispatchRangeToThreads(0, psize, (unsigned int)(nb_threads));

      // create the function to be executed by the threads
      auto threadedExec
         = [this, ranges](const std::size_t this_thread, const std::size_t nb_threads) {
             const auto& this_range = ranges[this_thread];
             for (Idx j = this_range.first; j < this_range.second; ++j) {
               _threadInference_(this_thread);
               _threadUpdate_(this_thread);
             }
           };

      do {
        eps = 0;

        // launch the threads
        ThreadExecutor::execute(nb_threads, threadedExec);

        this->updateApproximationScheme(int(psize));

        this->updateMarginals_();   // fusion threads + update margi

        eps = this->computeEpsilon_();   // also updates oldMargi

      } while (this->continueApproximationScheme(eps));
    }

    if (!this->modal_.empty()) { this->expFusion_(); }

    if (_infEs_::storeBNOpt_) { this->optFusion_(); }

    if (_infEs_::storeVertices_) { this->verticesFusion_(); }

    if (!this->modal_.empty()) {
      this->dynamicExpectations_();   // work with any network
    }
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_threadUpdate_(Size tId) {
    if (this->l_inferenceEngine_[tId]->evidenceProbability() > 0) {
      const DAG& tDag = this->workingSet_[tId]->dag();

      for (auto node: tDag) {
        const Potential< GUM_SCALAR >& potential(this->l_inferenceEngine_[tId]->posterior(node));
        Instantiation                  ins(potential);
        std::vector< GUM_SCALAR >      vertex;

        for (ins.setFirst(); !ins.end(); ++ins) {
          vertex.push_back(potential[ins]);
        }

        // true for redundancy elimination of node it credal set but since global
        // marginals are only updated at the end of each period of
        // approximationScheme, it is "useless" ( and expensive ) to check now
        this->updateThread_(tId, node, vertex, false);
      }
    }
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_threadInference_(Size tId) {
    _verticesSampling_(tId);

    this->l_inferenceEngine_[tId]->eraseAllEvidence();
    _insertEvidence_(tId);
    this->l_inferenceEngine_[tId]->makeInference();
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_mcInitApproximationScheme_() {
    this->setEpsilon(0.);
    this->enableEpsilon();   // to be sure

    this->disableMinEpsilonRate();
    this->disableMaxIter();

    this->initApproximationScheme();
  }


  template < typename GUM_SCALAR, class BNInferenceEngine >
  void CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_mcThreadDataCopy_() {
    auto num_threads = this->getNumberOfThreads();
    this->initThreadsData_(num_threads, _infEs_::storeVertices_, _infEs_::storeBNOpt_);
    this->l_inferenceEngine_.resize(num_threads, nullptr);

    // create the BNs: do this in a single thread because Bayes Nets do not
    // support slaves in multi threading
    for (auto& thread_bn: this->workingSet_)
      thread_bn = new BayesNet< GUM_SCALAR >(this->credalNet_->current_bn());

    // create the function to be executed by the threads
    auto threadedExec = [this](const std::size_t this_thread, const std::size_t nb_threads) {
      this->l_marginalMin_[this_thread]    = this->marginalMin_;
      this->l_marginalMax_[this_thread]    = this->marginalMax_;
      this->l_expectationMin_[this_thread] = this->expectationMin_;
      this->l_expectationMax_[this_thread] = this->expectationMax_;
      this->l_modal_[this_thread]          = this->modal_;

      _infEs_::l_clusters_[this_thread].resize(2);
      _infEs_::l_clusters_[this_thread][0] = _infEs_::t0_;
      _infEs_::l_clusters_[this_thread][1] = _infEs_::t1_;

      if (_infEs_::storeVertices_) { this->l_marginalSets_[this_thread] = this->marginalSets_; }

      this->workingSetE_[this_thread] = new List< const Potential< GUM_SCALAR >* >();

      // #TODO: the next instruction works only for lazy propagation.
      //        => find a way to remove the second argument
      auto inference_engine = new BNInferenceEngine(this->workingSet_[this_thread],
                                                    RelevantPotentialsFinderType::FIND_ALL);
      inference_engine->setNumberOfThreads(1);

      this->l_inferenceEngine_[this_thread] = inference_engine;

      if (_infEs_::storeBNOpt_) {
        this->l_optimalNet_[this_thread] = new VarMod2BNsMap< GUM_SCALAR >(*this->credalNet_);
      }
    };

    // launch the threads
    ThreadExecutor::execute(num_threads, threadedExec);
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void
     CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_binaryRep_(std::vector< bool >& toFill,
                                                                        const Idx value) const {
    Idx  n      = value;
    auto tfsize = toFill.size();

    // get bits of choosen_vertex
    for (decltype(tfsize) i = 0; i < tfsize; i++) {
      toFill[i] = n & 1;
      n /= 2;
    }
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void
     CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_verticesSampling_(Size this_thread) {
    IBayesNet< GUM_SCALAR >* working_bn       = this->workingSet_[this_thread];
    auto&                    random_generator = this->generators_[this_thread];
    const auto               cpt              = &this->credalNet_->credalNet_currentCpt();

    using dBN = std::vector< std::vector< std::vector< bool > > >;

    dBN sample;

    if (_infEs_::storeBNOpt_) { sample = dBN(this->l_optimalNet_[this_thread]->getSampleDef()); }

    if (_infEs_::repetitiveInd_) {
      const auto& t0 = _infEs_::l_clusters_[this_thread][0];
      const auto& t1 = _infEs_::l_clusters_[this_thread][1];

      for (const auto& elt: t0) {
        auto                     dSize = working_bn->variable(elt.first).domainSize();
        Potential< GUM_SCALAR >* potential(
           const_cast< Potential< GUM_SCALAR >* >(&working_bn->cpt(elt.first)));
        std::vector< GUM_SCALAR > var_cpt(potential->domainSize());

        Size pconfs = Size((*cpt)[elt.first].size());

        for (Size pconf = 0; pconf < pconfs; pconf++) {
          Size choosen_vertex = randomValue((*cpt)[elt.first][pconf].size());

          if (_infEs_::storeBNOpt_) { _binaryRep_(sample[elt.first][pconf], choosen_vertex); }

          for (Size mod = 0; mod < dSize; mod++) {
            var_cpt[pconf * dSize + mod] = (*cpt)[elt.first][pconf][choosen_vertex][mod];
          }
        }   // end of : pconf

        potential->fillWith(var_cpt);

        Size t0esize = Size(elt.second.size());

        for (Size pos = 0; pos < t0esize; pos++) {
          if (_infEs_::storeBNOpt_) { sample[elt.second[pos]] = sample[elt.first]; }

          Potential< GUM_SCALAR >* potential2(
             const_cast< Potential< GUM_SCALAR >* >(&working_bn->cpt(elt.second[pos])));
          potential2->fillWith(var_cpt);
        }
      }

      for (const auto& elt: t1) {
        auto                     dSize = working_bn->variable(elt.first).domainSize();
        Potential< GUM_SCALAR >* potential(
           const_cast< Potential< GUM_SCALAR >* >(&working_bn->cpt(elt.first)));
        std::vector< GUM_SCALAR > var_cpt(potential->domainSize());

        for (Size pconf = 0; pconf < (*cpt)[elt.first].size(); pconf++) {
          Idx choosen_vertex = randomValue(random_generator, (*cpt)[elt.first][pconf].size());

          if (_infEs_::storeBNOpt_) { _binaryRep_(sample[elt.first][pconf], choosen_vertex); }

          for (decltype(dSize) mod = 0; mod < dSize; mod++) {
            var_cpt[pconf * dSize + mod] = (*cpt)[elt.first][pconf][choosen_vertex][mod];
          }
        }   // end of : pconf

        potential->fillWith(var_cpt);

        auto t1esize = elt.second.size();

        for (decltype(t1esize) pos = 0; pos < t1esize; pos++) {
          if (_infEs_::storeBNOpt_) { sample[elt.second[pos]] = sample[elt.first]; }

          Potential< GUM_SCALAR >* potential2(
             const_cast< Potential< GUM_SCALAR >* >(&working_bn->cpt(elt.second[pos])));
          potential2->fillWith(var_cpt);
        }
      }

      if (_infEs_::storeBNOpt_) { this->l_optimalNet_[this_thread]->setCurrentSample(sample); }
    } else {
      for (auto node: working_bn->nodes()) {
        auto                     dSize = working_bn->variable(node).domainSize();
        Potential< GUM_SCALAR >* potential(
           const_cast< Potential< GUM_SCALAR >* >(&working_bn->cpt(node)));
        std::vector< GUM_SCALAR > var_cpt(potential->domainSize());

        auto pConfs = (*cpt)[node].size();

        for (decltype(pConfs) pconf = 0; pconf < pConfs; pconf++) {
          Size nVertices      = Size((*cpt)[node][pconf].size());
          Idx  choosen_vertex = randomValue(random_generator, nVertices);

          if (_infEs_::storeBNOpt_) { _binaryRep_(sample[node][pconf], choosen_vertex); }

          for (decltype(dSize) mod = 0; mod < dSize; mod++) {
            var_cpt[pconf * dSize + mod] = (*cpt)[node][pconf][choosen_vertex][mod];
          }
        }   // end of : pconf

        potential->fillWith(var_cpt);
      }

      if (_infEs_::storeBNOpt_) { this->l_optimalNet_[this_thread]->setCurrentSample(sample); }
    }
  }

  template < typename GUM_SCALAR, class BNInferenceEngine >
  inline void
     CNMonteCarloSampling< GUM_SCALAR, BNInferenceEngine >::_insertEvidence_(Size this_thread) {
    if (this->evidence_.size() == 0) { return; }

    BNInferenceEngine* inference_engine = this->l_inferenceEngine_[this_thread];

    IBayesNet< GUM_SCALAR >* working_bn = this->workingSet_[this_thread];

    List< const Potential< GUM_SCALAR >* >* evi_list = this->workingSetE_[this_thread];

    if (evi_list->size() > 0) {
      for (const auto pot: *evi_list)
        inference_engine->addEvidence(*pot);
      return;
    }

    for (const auto& elt: this->evidence_) {
      auto p = new Potential< GUM_SCALAR >;
      (*p) << working_bn->variable(elt.first);

      try {
        p->fillWith(elt.second);
      } catch (Exception const& err) {
        GUM_SHOWERROR(err)
        throw;
      }

      evi_list->insert(p);
    }

    if (evi_list->size() > 0) {
      for (const auto pot: *evi_list)
        inference_engine->addEvidence(*pot);
    }
  }

}   // namespace gum::credal

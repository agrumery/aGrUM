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


#include <agrum/CN/inference/CNLoopyPropagation.h>

namespace gum::credal {

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::saveInference(const std::string& path) {
    std::string path_name = path.substr(0, path.size() - 4);
    path_name             = path_name + ".res";

    std::ofstream res(path_name.c_str(), std::ios::out | std::ios::trunc);

    if (!res.good()) {
      GUM_ERROR(NotFound,
                "CNLoopyPropagation<GUM_SCALAR>::saveInference(std::"
                "string & path) : could not open file : "
                   + path_name)
    }


    if (std::string ext = path.substr(path.size() - 3, path.size());
        std::strcmp(ext.c_str(), "evi") == 0) {
      std::ifstream evi(path.c_str(), std::ios::in);
      std::string   ligne;

      if (!evi.good()) {
        GUM_ERROR(NotFound,
                  "CNLoopyPropagation<GUM_SCALAR>::saveInference(std::"
                  "string & path) : could not open file : "
                     + ext)
      }

      while (evi.good()) {
        getline(evi, ligne);
        res << ligne << "\n";
      }

      evi.close();
    }

    res << "[RESULTATS]"
        << "\n";

    for (auto node: _bnet_->nodes()) {
      // calcul distri posteriori
      GUM_SCALAR msg_p_min = 1.0;
      GUM_SCALAR msg_p_max = 0.0;

      // cas evidence, calcul immediat
      if (_infE_::evidence_.exists(node)) {
        if (_infE_::evidence_[node][1] == 0.) {
          msg_p_min = 0.;
        } else if (_infE_::evidence_[node][1] == 1.) {
          msg_p_min = 1.;
        }

        msg_p_max = msg_p_min;
      }
      // sinon depuis node P et node L
      else {
        GUM_SCALAR min = NodesP_min_[node];
        GUM_SCALAR max;

        if (NodesP_max_.exists(node)) {
          max = NodesP_max_[node];
        } else {
          max = min;
        }

        GUM_SCALAR lmin = NodesL_min_[node];
        GUM_SCALAR lmax;

        if (NodesL_max_.exists(node)) {
          lmax = NodesL_max_[node];
        } else {
          lmax = lmin;
        }

        // cas limites sur min
        if (min == INF_ && lmin == 0.) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
        }

        if (lmin == INF_) {   // cas infini
          msg_p_min = GUM_SCALAR(1.);
        } else if (min == 0. || lmin == 0.) {
          msg_p_min = GUM_SCALAR(0.);
        } else {
          msg_p_min = GUM_SCALAR(1. / (1. + ((1. / min - 1.) * 1. / lmin)));
        }

        // cas limites sur max
        if (max == INF_ && lmax == 0.) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
        }

        if (lmax == INF_) {   // cas infini
          msg_p_max = GUM_SCALAR(1.);
        } else if (max == 0. || lmax == 0.) {
          msg_p_max = GUM_SCALAR(0.);
        } else {
          msg_p_max = GUM_SCALAR(1. / (1. + ((1. / max - 1.) * 1. / lmax)));
        }
      }

      if (msg_p_min != msg_p_min && msg_p_max == msg_p_max) { msg_p_min = msg_p_max; }

      if (msg_p_max != msg_p_max && msg_p_min == msg_p_min) { msg_p_max = msg_p_min; }

      if (msg_p_max != msg_p_max && msg_p_min != msg_p_min) {
        std::cout << std::endl;
        std::cout << "pas de proba calculable (verifier observations)" << std::endl;
      }

      res << "P(" << _bnet_->variable(node).name() << " | e) = ";

      if (_infE_::evidence_.exists(node)) {
        res << "(observe)" << std::endl;
      } else {
        res << std::endl;
      }

      res << "\t\t" << _bnet_->variable(node).label(0) << "  [ " << (GUM_SCALAR)1. - msg_p_max;

      if (msg_p_min != msg_p_max) {
        res << ", " << (GUM_SCALAR)1. - msg_p_min << " ] | ";
      } else {
        res << " ] | ";
      }

      res << _bnet_->variable(node).label(1) << "  [ " << msg_p_min;

      if (msg_p_min != msg_p_max) {
        res << ", " << msg_p_max << " ]" << std::endl;
      } else {
        res << " ]" << std::endl;
      }
    }   // end of : for each node

    res.close();
  }

  /**
   * pour les fonctions suivantes, les GUM_SCALAR min/max doivent etre
   * initialises
   * (min a 1 et max a 0) pour comparer avec les resultats intermediaires
   */

  /**
   * une fois les cpts marginalises sur X et Ui, on calcul le min/max,
   */
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::compute_ext_(GUM_SCALAR&                msg_l_min,
                                                      GUM_SCALAR&                msg_l_max,
                                                      std::vector< GUM_SCALAR >& lx,
                                                      GUM_SCALAR&                num_min,
                                                      GUM_SCALAR&                num_max,
                                                      GUM_SCALAR&                den_min,
                                                      GUM_SCALAR&                den_max) {
    GUM_SCALAR num_min_tmp = 1.;
    GUM_SCALAR den_min_tmp = 1.;
    GUM_SCALAR num_max_tmp = 1.;
    GUM_SCALAR den_max_tmp = 1.;

    GUM_SCALAR res_min = 1.0;
    GUM_SCALAR res_max = 0.0;

    auto lsize = lx.size();

    for (decltype(lsize) i = 0; i < lsize; i++) {
      bool non_defini_min = false;
      bool non_defini_max = false;

      if (lx[i] == INF_) {
        num_min_tmp = num_min;
        den_min_tmp = den_max;
        num_max_tmp = num_max;
        den_max_tmp = den_min;
      } else if (lx[i] == (GUM_SCALAR)1.) {
        num_min_tmp = GUM_SCALAR(1.);
        den_min_tmp = GUM_SCALAR(1.);
        num_max_tmp = GUM_SCALAR(1.);
        den_max_tmp = GUM_SCALAR(1.);
      } else if (lx[i] > (GUM_SCALAR)1.) {
        GUM_SCALAR li = GUM_SCALAR(1.) / (lx[i] - GUM_SCALAR(1.));
        num_min_tmp   = num_min + li;
        den_min_tmp   = den_max + li;
        num_max_tmp   = num_max + li;
        den_max_tmp   = den_min + li;
      } else if (lx[i] < (GUM_SCALAR)1.) {
        GUM_SCALAR li = GUM_SCALAR(1.) / (lx[i] - GUM_SCALAR(1.));
        num_min_tmp   = num_max + li;
        den_min_tmp   = den_min + li;
        num_max_tmp   = num_min + li;
        den_max_tmp   = den_max + li;
      }

      if (den_min_tmp == 0. && num_min_tmp == 0.) {
        non_defini_min = true;
      } else if (den_min_tmp == 0. && num_min_tmp != 0.) {
        res_min = INF_;
      } else if (den_min_tmp != INF_ || num_min_tmp != INF_) {
        res_min = num_min_tmp / den_min_tmp;
      }

      if (den_max_tmp == 0. && num_max_tmp == 0.) {
        non_defini_max = true;
      } else if (den_max_tmp == 0. && num_max_tmp != 0.) {
        res_max = INF_;
      } else if (den_max_tmp != INF_ || num_max_tmp != INF_) {
        res_max = num_max_tmp / den_max_tmp;
      }

      if (non_defini_max && non_defini_min) {
        std::cout << "undefined msg" << std::endl;
        continue;
      } else if (non_defini_min && !non_defini_max) {
        res_min = res_max;
      } else if (non_defini_max && !non_defini_min) {
        res_max = res_min;
      }

      if (res_min < 0.) { res_min = 0.; }

      if (res_max < 0.) { res_max = 0.; }

      if (msg_l_min == msg_l_max && msg_l_min == -2.) {
        msg_l_min = res_min;
        msg_l_max = res_max;
      }

      if (res_max > msg_l_max) { msg_l_max = res_max; }

      if (res_min < msg_l_min) { msg_l_min = res_min; }

    }   // end of : for each lx
  }

  /**
   * extremes pour une combinaison des parents, message vers parent
   */
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::compute_ext_(
     std::vector< std::vector< GUM_SCALAR > >& combi_msg_p,
     const NodeId&                             id,
     GUM_SCALAR&                               msg_l_min,
     GUM_SCALAR&                               msg_l_max,
     std::vector< GUM_SCALAR >&                lx,
     const Idx&                                pos) {
    GUM_SCALAR num_min = 0.;
    GUM_SCALAR num_max = 0.;
    GUM_SCALAR den_min = 0.;
    GUM_SCALAR den_max = 0.;

    auto taille = combi_msg_p.size();

    std::vector< typename std::vector< GUM_SCALAR >::iterator > it(taille);

    for (decltype(taille) i = 0; i < taille; i++) {
      it[i] = combi_msg_p[i].begin();
    }

    Size pp = pos;

    Size combi_den = 0;
    Size combi_num = pp;

    // marginalisation
    while (it[taille - 1] != combi_msg_p[taille - 1].end()) {
      GUM_SCALAR prod = 1.;

      for (decltype(taille) k = 0; k < taille; k++) {
        prod *= *it[k];
      }

      den_min += (_cn_->get_binaryCPT_min()[id][combi_den] * prod);
      den_max += (_cn_->get_binaryCPT_max()[id][combi_den] * prod);

      num_min += (_cn_->get_binaryCPT_min()[id][combi_num] * prod);
      num_max += (_cn_->get_binaryCPT_max()[id][combi_num] * prod);

      combi_den++;
      combi_num++;

      if (pp != 0) {
        if (combi_den % pp == 0) {
          combi_den += pp;
          combi_num += pp;
        }
      }

      // incrementation
      ++it[0];

      for (decltype(taille) i = 0; (i < taille - 1) && (it[i] == combi_msg_p[i].end()); ++i) {
        it[i] = combi_msg_p[i].begin();
        ++it[i + 1];
      }
    }   // end of : marginalisation

    compute_ext_(msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max);
  }

  /**
   * extremes pour une combinaison des parents, message vers enfant
   * marginalisation cpts
   */
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::compute_ext_(
     std::vector< std::vector< GUM_SCALAR > >& combi_msg_p,
     const NodeId&                             id,
     GUM_SCALAR&                               msg_p_min,
     GUM_SCALAR&                               msg_p_max) {
    GUM_SCALAR min = 0.;
    GUM_SCALAR max = 0.;

    auto taille = combi_msg_p.size();

    std::vector< typename std::vector< GUM_SCALAR >::iterator > it(taille);

    for (decltype(taille) i = 0; i < taille; i++) {
      it[i] = combi_msg_p[i].begin();
    }

    int  combi  = 0;
    auto theEnd = combi_msg_p[taille - 1].end();

    while (it[taille - 1] != theEnd) {
      GUM_SCALAR prod = 1.;

      for (decltype(taille) k = 0; k < taille; k++) {
        prod *= *it[k];
      }

      min += (_cn_->get_binaryCPT_min()[id][combi] * prod);
      max += (_cn_->get_binaryCPT_max()[id][combi] * prod);

      combi++;

      // incrementation
      ++it[0];

      for (decltype(taille) i = 0; (i < taille - 1) && (it[i] == combi_msg_p[i].end()); ++i) {
        it[i] = combi_msg_p[i].begin();
        ++it[i + 1];
      }
    }

    if (min < msg_p_min) { msg_p_min = min; }

    if (max > msg_p_max) { msg_p_max = max; }
  }

  /**
   * enumerate combinations messages parents, pour message vers enfant
   */
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::enum_combi_(
     std::vector< std::vector< std::vector< GUM_SCALAR > > >& msgs_p,
     const NodeId&                                            id,
     GUM_SCALAR&                                              msg_p_min,
     GUM_SCALAR&                                              msg_p_max) {
    auto taille = msgs_p.size();

    // source node
    if (taille == 0) {
      msg_p_min = _cn_->get_binaryCPT_min()[id][0];
      msg_p_max = _cn_->get_binaryCPT_max()[id][0];
      return;
    }

    Size msgPerm = 1;
    for (Size i = 0; i < taille; i++) {
      msgPerm *= msgs_p[i].size();
    }

    // dispatch the messages among the threads and prepare the data
    // they will process
    Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                       ? this->getNumberOfThreads()
                       : 1;   // no nested multithreading
    nb_threads      = std::min(msgPerm * taille / this->threadMinimalNbOps_, nb_threads);
    if (nb_threads < 1) nb_threads = 1;

    const auto ranges = gum::dispatchRangeToThreads(0, msgPerm, (unsigned int)(nb_threads));
    const auto real_nb_threads = ranges.size();
    std::vector< GUM_SCALAR > msg_pmin(real_nb_threads, msg_p_min);
    std::vector< GUM_SCALAR > msg_pmax(real_nb_threads, msg_p_max);

    // create the function to be executed by the threads
    auto threadedExec
       = [this, &msg_pmin, &msg_pmax, msgs_p, taille, ranges, id](const std::size_t this_thread,
                                                                  const std::size_t nb_threads) {
           std::vector< std::vector< GUM_SCALAR > > combi_msg_p(taille);

           const auto& [first, second] = ranges[this_thread];
           for (Idx j = first; j < second; ++j) {
             // get jth msg :
             auto jvalue = j;

             for (Idx i = 0; i < taille; i++) {
               if (msgs_p[i].size() == 2) {
                 combi_msg_p[i] = (jvalue & 1) ? msgs_p[i][1] : msgs_p[i][0];
                 jvalue /= 2;
               } else {
                 combi_msg_p[i] = msgs_p[i][0];
               }
             }

             compute_ext_(combi_msg_p, id, msg_pmin[this_thread], msg_pmax[this_thread]);
           }
         };

    // launch the threads
    ThreadExecutor::execute(real_nb_threads, threadedExec);

    for (Idx j = 0; j < real_nb_threads; ++j) {
      if (msg_p_min > msg_pmin[j]) { msg_p_min = msg_pmin[j]; }
      if (msg_p_max < msg_pmax[j]) { msg_p_max = msg_pmax[j]; }
    }
  }


  /**
   * comme precedemment mais pour message parent, vraisemblance prise en
   * compte
   */
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::enum_combi_(
     std::vector< std::vector< std::vector< GUM_SCALAR > > >& msgs_p,
     const NodeId&                                            id,
     GUM_SCALAR&                                              real_msg_l_min,
     GUM_SCALAR&                                              real_msg_l_max,
     std::vector< GUM_SCALAR >&                               lx,
     const Idx&                                               pos) {
    GUM_SCALAR msg_l_min = real_msg_l_min;
    GUM_SCALAR msg_l_max = real_msg_l_max;

    auto taille = msgs_p.size();

    // one parent node, the one receiving the message
    if (taille == 0) {
      GUM_SCALAR num_min = _cn_->get_binaryCPT_min()[id][1];
      GUM_SCALAR num_max = _cn_->get_binaryCPT_max()[id][1];
      GUM_SCALAR den_min = _cn_->get_binaryCPT_min()[id][0];
      GUM_SCALAR den_max = _cn_->get_binaryCPT_max()[id][0];

      compute_ext_(msg_l_min, msg_l_max, lx, num_min, num_max, den_min, den_max);

      real_msg_l_min = msg_l_min;
      real_msg_l_max = msg_l_max;
      return;
    }

    Size msgPerm = 1;
    for (Size i = 0; i < taille; i++) {
      msgPerm *= msgs_p[i].size();
    }

    // dispatch the messages among the threads and prepare the data
    // they will process
    Size nb_threads = ThreadExecutor::nbRunningThreadsExecutors() == 0
                       ? this->getNumberOfThreads()
                       : 1;   // no nested multithreading
    nb_threads      = std::min(msgPerm * taille / this->threadMinimalNbOps_, nb_threads);
    if (nb_threads < 1) nb_threads = 1;

    const auto ranges = gum::dispatchRangeToThreads(0, msgPerm, (unsigned int)(nb_threads));
    const auto real_nb_threads = ranges.size();
    std::vector< GUM_SCALAR > msg_lmin(real_nb_threads, msg_l_min);
    std::vector< GUM_SCALAR > msg_lmax(real_nb_threads, msg_l_max);

    // create the function to be executed by the threads
    auto threadedExec = [this, &msg_lmin, &msg_lmax, msgs_p, taille, ranges, id, &lx, pos](
                           const std::size_t this_thread,
                           const std::size_t nb_threads) {
      std::vector< std::vector< GUM_SCALAR > > combi_msg_p(taille);

      const auto& [first, second] = ranges[this_thread];
      for (Idx j = first; j < second; ++j) {
        // get jth msg :
        auto jvalue = j;

        for (Idx i = 0; i < taille; i++) {
          if (msgs_p[i].size() == 2) {
            combi_msg_p[i] = (jvalue & 1) ? msgs_p[i][1] : msgs_p[i][0];
            jvalue /= 2;
          } else {
            combi_msg_p[i] = msgs_p[i][0];
          }
        }
        compute_ext_(combi_msg_p, id, msg_lmin[this_thread], msg_lmax[this_thread], lx, pos);
      }
    };

    // launch the threads
    ThreadExecutor::execute(real_nb_threads, threadedExec);

    for (Idx j = 0; j < real_nb_threads; ++j) {
      if ((msg_l_min > msg_lmin[j] || msg_l_min == -2) && msg_lmin[j] > 0) {
        msg_l_min = msg_lmin[j];
      }
      if ((msg_l_max < msg_lmax[j] || msg_l_max == -2) && msg_lmax[j] > 0) {
        msg_l_max = msg_lmax[j];
      }
    }

    real_msg_l_min = msg_l_min;
    real_msg_l_max = msg_l_max;
  }


  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::makeInference() {
    if (InferenceUpToDate_) { return; }

    initialize_();

    _infE_::initApproximationScheme();

    switch (_inferenceType_) {
      case InferenceType::nodeToNeighbours: makeInferenceNodeToNeighbours_(); break;

      case InferenceType::ordered: makeInferenceByOrderedArcs_(); break;

      case InferenceType::randomOrder: makeInferenceByRandomOrder_(); break;
    }

    //_updateMarginals();
    updateIndicatrices_();   // will call updateMarginals_()

    computeExpectations_();

    InferenceUpToDate_ = true;
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::eraseAllEvidence() {
    _infE_::eraseAllEvidence();

    ArcsL_min_.clear();
    ArcsL_max_.clear();
    ArcsP_min_.clear();
    ArcsP_max_.clear();
    NodesL_min_.clear();
    NodesL_max_.clear();
    NodesP_min_.clear();
    NodesP_max_.clear();

    InferenceUpToDate_ = false;

    if (!msg_l_sent_.empty()) {
      for (auto node: _bnet_->nodes()) {
        delete msg_l_sent_[node];
      }
    }

    msg_l_sent_.clear();
    update_l_.clear();
    update_p_.clear();

    active_nodes_set.clear();
    next_active_nodes_set.clear();
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::initialize_() {
    const DAG& graphe = _bnet_->dag();

    // use const iterators with cbegin when available
    for (auto node: _bnet_->topologicalOrder()) {
      update_p_.set(node, false);
      update_l_.set(node, false);
      auto parents_ = new NodeSet();
      msg_l_sent_.set(node, parents_);

      // accelerer init pour evidences
      if (_infE_::evidence_.exists(node)) {
        if (_infE_::evidence_[node][1] != 0. && _infE_::evidence_[node][1] != 1.) {
          GUM_ERROR(OperationNotAllowed, "CNLoopyPropagation can only handle HARD evidences")
        }

        active_nodes_set.insert(node);
        update_l_.set(node, true);
        update_p_.set(node, true);

        if (_infE_::evidence_[node][1] == (GUM_SCALAR)1.) {
          NodesL_min_.set(node, INF_);
          NodesP_min_.set(node, (GUM_SCALAR)1.);
        } else if (_infE_::evidence_[node][1] == (GUM_SCALAR)0.) {
          NodesL_min_.set(node, (GUM_SCALAR)0.);
          NodesP_min_.set(node, (GUM_SCALAR)0.);
        }

        std::vector< GUM_SCALAR > marg(2);
        marg[1] = NodesP_min_[node];
        marg[0] = 1 - marg[1];

        _infE_::oldMarginalMin_.set(node, marg);
        _infE_::oldMarginalMax_.set(node, marg);

        continue;
      }

      NodeSet par_ = graphe.parents(node);
      NodeSet enf_ = graphe.children(node);

      if (par_.empty()) {
        active_nodes_set.insert(node);
        update_p_.set(node, true);
        update_l_.set(node, true);
      }

      if (enf_.empty()) {
        active_nodes_set.insert(node);
        update_p_.set(node, true);
        update_l_.set(node, true);
      }

      /**
       * messages and so parents need to be read in order of appearance
       * use potentials instead of dag
       */
      const auto parents = &_bnet_->cpt(node).variablesSequence();

      std::vector< std::vector< std::vector< GUM_SCALAR > > > msgs_p;
      std::vector< std::vector< GUM_SCALAR > >                msg_p;
      std::vector< GUM_SCALAR >                               distri(2);

      // +1 from start to avoid counting_ itself
      // use const iterators when available with cbegin
      for (auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt) {
        // compute probability distribution to avoid doing it multiple times
        // (at
        // each combination of messages)
        distri[1] = NodesP_min_[_bnet_->nodeId(**jt)];
        distri[0] = (GUM_SCALAR)1. - distri[1];
        msg_p.push_back(distri);

        if (NodesP_max_.exists(_bnet_->nodeId(**jt))) {
          distri[1] = NodesP_max_[_bnet_->nodeId(**jt)];
          distri[0] = (GUM_SCALAR)1. - distri[1];
          msg_p.push_back(distri);
        }

        msgs_p.push_back(msg_p);
        msg_p.clear();
      }

      GUM_SCALAR msg_p_min = 1.;
      GUM_SCALAR msg_p_max = 0.;

      if (_cn_->currentNodeType(node) != CredalNet< GUM_SCALAR >::NodeType::Indic) {
        enum_combi_(msgs_p, node, msg_p_min, msg_p_max);
      }

      if (msg_p_min <= (GUM_SCALAR)0.) { msg_p_min = (GUM_SCALAR)0.; }

      if (msg_p_max <= (GUM_SCALAR)0.) { msg_p_max = (GUM_SCALAR)0.; }

      NodesP_min_.set(node, msg_p_min);
      std::vector< GUM_SCALAR > marg(2);
      marg[1] = msg_p_min;
      marg[0] = 1 - msg_p_min;

      _infE_::oldMarginalMin_.set(node, marg);

      if (msg_p_min != msg_p_max) {
        marg[1] = msg_p_max;
        marg[0] = 1 - msg_p_max;
        NodesP_max_.insert(node, msg_p_max);
      }

      _infE_::oldMarginalMax_.set(node, marg);

      NodesL_min_.set(node, (GUM_SCALAR)1.);
    }

    for (auto arc: _bnet_->arcs()) {
      ArcsP_min_.set(arc, NodesP_min_[arc.tail()]);

      if (NodesP_max_.exists(arc.tail())) { ArcsP_max_.set(arc, NodesP_max_[arc.tail()]); }

      ArcsL_min_.set(arc, NodesL_min_[arc.tail()]);
    }
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::makeInferenceNodeToNeighbours_() {
    const DAG& graphe = _bnet_->dag();

    GUM_SCALAR eps;
    // to validate TestSuite
    _infE_::continueApproximationScheme(1.);

    do {
      for (auto node: active_nodes_set) {
        for (auto chil: graphe.children(node)) {
          if (_cn_->currentNodeType(chil) == CredalNet< GUM_SCALAR >::NodeType::Indic) { continue; }

          msgP_(node, chil);
        }

        for (auto par: graphe.parents(node)) {
          if (_cn_->currentNodeType(node) == CredalNet< GUM_SCALAR >::NodeType::Indic) { continue; }

          msgL_(node, par);
        }
      }

      eps = calculateEpsilon_();

      _infE_::updateApproximationScheme();

      active_nodes_set.clear();
      active_nodes_set = next_active_nodes_set;
      next_active_nodes_set.clear();

    } while (_infE_::continueApproximationScheme(eps) && active_nodes_set.size() > 0);

    _infE_::stopApproximationScheme();   // just to be sure of the
    // approximationScheme has been notified of
    // the end of looop
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::makeInferenceByRandomOrder_() {
    Size nbrArcs = _bnet_->dag().sizeArcs();

    std::vector< cArcP > seq;
    seq.reserve(nbrArcs);

    for (const auto& arc: _bnet_->arcs()) {
      seq.push_back(&arc);
    }

    GUM_SCALAR eps;
    // validate TestSuite
    _infE_::continueApproximationScheme(1.);

    do {
      for (Size j = 0, theEnd = nbrArcs / 2; j < theEnd; j++) {
        auto w1 = randomValue(nbrArcs);
        auto w2 = randomValue(nbrArcs);

        if (w1 == w2) { continue; }

        std::swap(seq[w1], seq[w2]);
      }

      for (const auto it: seq) {
        if (_cn_->currentNodeType(it->tail()) == CredalNet< GUM_SCALAR >::NodeType::Indic
            || _cn_->currentNodeType(it->head()) == CredalNet< GUM_SCALAR >::NodeType::Indic) {
          continue;
        }

        msgP_(it->tail(), it->head());
        msgL_(it->head(), it->tail());
      }

      eps = calculateEpsilon_();

      _infE_::updateApproximationScheme();

    } while (_infE_::continueApproximationScheme(eps));
  }

  // gives slightly worse results for some variable/modalities than other
  // inference
  // types (node D on 2U network loose 0.03 precision)
  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::makeInferenceByOrderedArcs_() {
    Size nbrArcs = _bnet_->dag().sizeArcs();

    std::vector< cArcP > seq;
    seq.reserve(nbrArcs);

    for (const auto& arc: _bnet_->arcs()) {
      seq.push_back(&arc);
    }

    GUM_SCALAR eps;
    // validate TestSuite
    _infE_::continueApproximationScheme(1.);

    do {
      for (const auto it: seq) {
        if (_cn_->currentNodeType(it->tail()) == CredalNet< GUM_SCALAR >::NodeType::Indic
            || _cn_->currentNodeType(it->head()) == CredalNet< GUM_SCALAR >::NodeType::Indic) {
          continue;
        }

        msgP_(it->tail(), it->head());
        msgL_(it->head(), it->tail());
      }

      eps = calculateEpsilon_();

      _infE_::updateApproximationScheme();

    } while (_infE_::continueApproximationScheme(eps));
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::msgL_(const NodeId Y, const NodeId X) {
    NodeSet const& children = _bnet_->children(Y);
    NodeSet const& parents_ = _bnet_->parents(Y);

    const auto parents = &_bnet_->cpt(Y).variablesSequence();

    if (((children.size() + parents->size() - 1) == 1) && (!_infE_::evidence_.exists(Y))) {
      return;
    }

    bool update_l = update_l_[Y];
    bool update_p = update_p_[Y];

    if (!update_p && !update_l) { return; }

    msg_l_sent_[Y]->insert(X);

    // for future refresh LM/PI
    if (msg_l_sent_[Y]->size() == parents_.size()) {
      msg_l_sent_[Y]->clear();
      update_l_[Y] = false;
    }

    // refresh LM_part
    if (update_l) {
      if (!children.empty() && !_infE_::evidence_.exists(Y)) {
        GUM_SCALAR lmin = 1.;
        GUM_SCALAR lmax = 1.;

        for (auto chil: children) {
          lmin *= ArcsL_min_[Arc(Y, chil)];

          if (ArcsL_max_.exists(Arc(Y, chil))) {
            lmax *= ArcsL_max_[Arc(Y, chil)];
          } else {
            lmax *= ArcsL_min_[Arc(Y, chil)];
          }
        }

        lmin = lmax;

        if (lmax != lmax && lmin == lmin) { lmax = lmin; }

        if (lmax != lmax && lmin != lmin) {
          std::cout << "no likelihood defined [lmin, lmax] (incompatibles "
                       "evidence ?)"
                    << std::endl;
        }

        if (lmin < 0.) { lmin = 0.; }

        if (lmax < 0.) { lmax = 0.; }

        // no need to update nodeL if evidence since nodeL will never be used

        NodesL_min_[Y] = lmin;

        if (lmin != lmax) {
          NodesL_max_.set(Y, lmax);
        } else if (NodesL_max_.exists(Y)) {
          NodesL_max_.erase(Y);
        }

      }   // end of : node has children & no evidence

    }   // end of : if update_l

    GUM_SCALAR lmin = NodesL_min_[Y];
    GUM_SCALAR lmax;

    if (NodesL_max_.exists(Y)) {
      lmax = NodesL_max_[Y];
    } else {
      lmax = lmin;
    }

    /**
     *  lmin == lmax == 1  => sends 1 as message to parents
     */

    if (lmin == lmax && lmin == 1.) {
      ArcsL_min_[Arc(X, Y)] = lmin;

      if (ArcsL_max_.exists(Arc(X, Y))) { ArcsL_max_.erase(Arc(X, Y)); }

      return;
    }

    // garder pour chaque noeud un table des parents maj, une fois tous maj,
    // stop
    // jusque notification msg L ou P

    if (update_p || update_l) {
      std::vector< std::vector< std::vector< GUM_SCALAR > > > msgs_p;
      std::vector< std::vector< GUM_SCALAR > >                msg_p;
      std::vector< GUM_SCALAR >                               distri(2);

      Idx pos;

      // +1 from start to avoid counting_ itself
      // use const iterators with cbegin when available
      for (auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt) {
        if (_bnet_->nodeId(**jt) == X) {
          // retirer la variable courante de la taille
          pos = parents->pos(*jt) - 1;
          continue;
        }

        // compute probability distribution to avoid doing it multiple times
        // (at each combination of messages)
        distri[1] = ArcsP_min_[Arc(_bnet_->nodeId(**jt), Y)];
        distri[0] = GUM_SCALAR(1.) - distri[1];
        msg_p.push_back(distri);

        if (ArcsP_max_.exists(Arc(_bnet_->nodeId(**jt), Y))) {
          distri[1] = ArcsP_max_[Arc(_bnet_->nodeId(**jt), Y)];
          distri[0] = GUM_SCALAR(1.) - distri[1];
          msg_p.push_back(distri);
        }

        msgs_p.push_back(msg_p);
        msg_p.clear();
      }

      GUM_SCALAR min = -2.;
      GUM_SCALAR max = -2.;

      std::vector< GUM_SCALAR > lx;
      lx.push_back(lmin);

      if (lmin != lmax) { lx.push_back(lmax); }

      enum_combi_(msgs_p, Y, min, max, lx, pos);

      if (min == -2. || max == -2.) {
        if (min != -2.) {
          max = min;
        } else if (max != -2.) {
          min = max;
        } else {
          std::cout << std::endl;
          std::cout << "!!!! pas de message L calculable !!!!" << std::endl;
          return;
        }
      }

      if (min < 0.) { min = 0.; }

      if (max < 0.) { max = 0.; }

      bool update = false;

      if (min != ArcsL_min_[Arc(X, Y)]) {
        ArcsL_min_[Arc(X, Y)] = min;
        update                = true;
      }

      if (ArcsL_max_.exists(Arc(X, Y))) {
        if (max != ArcsL_max_[Arc(X, Y)]) {
          if (max != min) {
            ArcsL_max_[Arc(X, Y)] = max;
          } else {   // if ( max == min )
            ArcsL_max_.erase(Arc(X, Y));
          }

          update = true;
        }
      } else {
        if (max != min) {
          ArcsL_max_.insert(Arc(X, Y), max);
          update = true;
        }
      }

      if (update) {
        update_l_.set(X, true);
        next_active_nodes_set.insert(X);
      }

    }   // end of update_p || update_l
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::msgP_(const NodeId X, const NodeId demanding_child) {
    NodeSet const& children = _bnet_->children(X);

    const auto parents = &_bnet_->cpt(X).variablesSequence();

    if (((children.size() + parents->size() - 1) == 1) && (!_infE_::evidence_.exists(X))) {
      return;
    }

    // LM_part ---- from all children but one --- the lonely one will get the
    // message

    if (_infE_::evidence_.exists(X)) {
      ArcsP_min_[Arc(X, demanding_child)] = _infE_::evidence_[X][1];

      if (ArcsP_max_.exists(Arc(X, demanding_child))) { ArcsP_max_.erase(Arc(X, demanding_child)); }

      return;
    }

    bool update_l = update_l_[X];
    bool update_p = update_p_[X];

    if (!update_p && !update_l) { return; }

    GUM_SCALAR lmin = 1.;
    GUM_SCALAR lmax = 1.;

    // use cbegin if available
    for (auto chil: children) {
      if (chil == demanding_child) { continue; }

      lmin *= ArcsL_min_[Arc(X, chil)];

      if (ArcsL_max_.exists(Arc(X, chil))) {
        lmax *= ArcsL_max_[Arc(X, chil)];
      } else {
        lmax *= ArcsL_min_[Arc(X, chil)];
      }
    }

    if (lmin != lmin && lmax == lmax) { lmin = lmax; }

    if (lmax != lmax && lmin == lmin) { lmax = lmin; }

    if (lmax != lmax && lmin != lmin) {
      std::cout << "pas de vraisemblance definie [lmin, lmax] (observations "
                   "incompatibles ?)"
                << std::endl;
      return;
    }

    if (lmin < 0.) { lmin = 0.; }

    if (lmax < 0.) { lmax = 0.; }

    // refresh PI_part
    GUM_SCALAR min = INF_;
    GUM_SCALAR max = 0.;

    if (update_p) {
      std::vector< std::vector< std::vector< GUM_SCALAR > > > msgs_p;
      std::vector< std::vector< GUM_SCALAR > >                msg_p;
      std::vector< GUM_SCALAR >                               distri(2);

      // +1 from start to avoid counting_ itself
      // use const_iterators if available
      for (auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt) {
        // compute probability distribution to avoid doing it multiple times
        // (at
        // each combination of messages)
        distri[1] = ArcsP_min_[Arc(_bnet_->nodeId(**jt), X)];
        distri[0] = GUM_SCALAR(1.) - distri[1];
        msg_p.push_back(distri);

        if (ArcsP_max_.exists(Arc(_bnet_->nodeId(**jt), X))) {
          distri[1] = ArcsP_max_[Arc(_bnet_->nodeId(**jt), X)];
          distri[0] = GUM_SCALAR(1.) - distri[1];
          msg_p.push_back(distri);
        }

        msgs_p.push_back(msg_p);
        msg_p.clear();
      }

      enum_combi_(msgs_p, X, min, max);

      if (min < 0.) { min = 0.; }

      if (max < 0.) { max = 0.; }

      if (min == INF_ || max == INF_) {
        std::cout << " ERREUR msg P min = max = INF " << std::endl;
        std::cout.flush();
        return;
      }

      NodesP_min_[X] = min;

      if (min != max) {
        NodesP_max_.set(X, max);
      } else if (NodesP_max_.exists(X)) {
        NodesP_max_.erase(X);
      }

      update_p_.set(X, false);

    }   // end of update_p
    else {
      min = NodesP_min_[X];

      if (NodesP_max_.exists(X)) {
        max = NodesP_max_[X];
      } else {
        max = min;
      }
    }

    if (update_p || update_l) {
      GUM_SCALAR msg_p_min;
      GUM_SCALAR msg_p_max;

      // cas limites sur min
      if (min == INF_ && lmin == 0.) {
        std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;
      }

      if (lmin == INF_) {   // cas infini
        msg_p_min = GUM_SCALAR(1.);
      } else if (min == 0. || lmin == 0.) {
        msg_p_min = 0;
      } else {
        msg_p_min = GUM_SCALAR(1. / (1. + ((1. / min - 1.) * 1. / lmin)));
      }

      // cas limites sur max
      if (max == INF_ && lmax == 0.) {
        std::cout << "MESSAGE P ERR (negatif) : pi = inf, l = 0" << std::endl;
      }

      if (lmax == INF_) {   // cas infini
        msg_p_max = GUM_SCALAR(1.);
      } else if (max == 0. || lmax == 0.) {
        msg_p_max = 0;
      } else {
        msg_p_max = GUM_SCALAR(1. / (1. + ((1. / max - 1.) * 1. / lmax)));
      }

      if (msg_p_min != msg_p_min && msg_p_max == msg_p_max) {
        msg_p_min = msg_p_max;
        std::cout << std::endl;
        std::cout << "msg_p_min is NaN" << std::endl;
      }

      if (msg_p_max != msg_p_max && msg_p_min == msg_p_min) {
        msg_p_max = msg_p_min;
        std::cout << std::endl;
        std::cout << "msg_p_max is NaN" << std::endl;
      }

      if (msg_p_max != msg_p_max && msg_p_min != msg_p_min) {
        std::cout << std::endl;
        std::cout << "pas de message P calculable (verifier observations)" << std::endl;
        return;
      }

      if (msg_p_min < 0.) { msg_p_min = 0.; }

      if (msg_p_max < 0.) { msg_p_max = 0.; }

      bool update = false;

      if (msg_p_min != ArcsP_min_[Arc(X, demanding_child)]) {
        ArcsP_min_[Arc(X, demanding_child)] = msg_p_min;
        update                              = true;
      }

      if (ArcsP_max_.exists(Arc(X, demanding_child))) {
        if (msg_p_max != ArcsP_max_[Arc(X, demanding_child)]) {
          if (msg_p_max != msg_p_min) {
            ArcsP_max_[Arc(X, demanding_child)] = msg_p_max;
          } else {   // if ( msg_p_max == msg_p_min )
            ArcsP_max_.erase(Arc(X, demanding_child));
          }

          update = true;
        }
      } else {
        if (msg_p_max != msg_p_min) {
          ArcsP_max_.insert(Arc(X, demanding_child), msg_p_max);
          update = true;
        }
      }

      if (update) {
        update_p_.set(demanding_child, true);
        next_active_nodes_set.insert(demanding_child);
      }

    }   // end of : update_l || update_p
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::refreshLMsPIs_(bool refreshIndic) {
    for (auto node: _bnet_->nodes()) {
      if ((!refreshIndic)
          && _cn_->currentNodeType(node) == CredalNet< GUM_SCALAR >::NodeType::Indic) {
        continue;
      }

      NodeSet const& children = _bnet_->children(node);

      auto parents = &_bnet_->cpt(node).variablesSequence();

      if (update_l_[node]) {
        GUM_SCALAR lmin = 1.;
        GUM_SCALAR lmax = 1.;

        if (!children.empty() && !_infE_::evidence_.exists(node)) {
          for (auto chil: children) {
            lmin *= ArcsL_min_[Arc(node, chil)];

            if (ArcsL_max_.exists(Arc(node, chil))) {
              lmax *= ArcsL_max_[Arc(node, chil)];
            } else {
              lmax *= ArcsL_min_[Arc(node, chil)];
            }
          }

          if (lmin != lmin && lmax == lmax) { lmin = lmax; }

          lmax = lmin;

          if (lmax != lmax && lmin != lmin) {
            std::cout << "pas de vraisemblance definie [lmin, lmax] (observations "
                         "incompatibles ?)"
                      << std::endl;
            return;
          }

          if (lmin < 0.) { lmin = 0.; }

          if (lmax < 0.) { lmax = 0.; }

          NodesL_min_[node] = lmin;

          if (lmin != lmax) {
            NodesL_max_.set(node, lmax);
          } else if (NodesL_max_.exists(node)) {
            NodesL_max_.erase(node);
          }
        }

      }   // end of : update_l

      if (update_p_[node]) {
        if ((parents->size() - 1) > 0 && !_infE_::evidence_.exists(node)) {
          std::vector< std::vector< std::vector< GUM_SCALAR > > > msgs_p;
          std::vector< std::vector< GUM_SCALAR > >                msg_p;
          std::vector< GUM_SCALAR >                               distri(2);

          // +1 from start to avoid counting_ itself
          // cbegin
          for (auto jt = ++parents->begin(), theEnd = parents->end(); jt != theEnd; ++jt) {
            // compute probability distribution to avoid doing it multiple
            // times
            // (at each combination of messages)
            distri[1] = ArcsP_min_[Arc(_bnet_->nodeId(**jt), node)];
            distri[0] = GUM_SCALAR(1.) - distri[1];
            msg_p.push_back(distri);

            if (ArcsP_max_.exists(Arc(_bnet_->nodeId(**jt), node))) {
              distri[1] = ArcsP_max_[Arc(_bnet_->nodeId(**jt), node)];
              distri[0] = GUM_SCALAR(1.) - distri[1];
              msg_p.push_back(distri);
            }

            msgs_p.push_back(msg_p);
            msg_p.clear();
          }

          GUM_SCALAR min = INF_;
          GUM_SCALAR max = 0.;

          enum_combi_(msgs_p, node, min, max);

          if (min < 0.) { min = 0.; }

          if (max < 0.) { max = 0.; }

          NodesP_min_[node] = min;

          if (min != max) {
            NodesP_max_.set(node, max);
          } else if (NodesP_max_.exists(node)) {
            NodesP_max_.erase(node);
          }

          update_p_[node] = false;
        }
      }   // end of update_p

    }   // end of : for each node
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::updateMarginals_() {
    for (auto node: _bnet_->nodes()) {
      GUM_SCALAR msg_p_min = 1.;
      GUM_SCALAR msg_p_max = 0.;

      if (_infE_::evidence_.exists(node)) {
        if (_infE_::evidence_[node][1] == 0.) {
          msg_p_min = (GUM_SCALAR)0.;
        } else if (_infE_::evidence_[node][1] == 1.) {
          msg_p_min = 1.;
        }

        msg_p_max = msg_p_min;
      } else {
        GUM_SCALAR min = NodesP_min_[node];
        GUM_SCALAR max;

        if (NodesP_max_.exists(node)) {
          max = NodesP_max_[node];
        } else {
          max = min;
        }

        GUM_SCALAR lmin = NodesL_min_[node];
        GUM_SCALAR lmax;
        if (NodesL_max_.exists(node)) {
          lmax = NodesL_max_[node];
        } else {
          lmax = lmin;
        }

        if (min == INF_ || max == INF_) {
          std::cout << " min ou max === INF_ !!!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
          return;
        }

        if (min == INF_ && lmin == 0.) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
          return;
        }

        if (lmin == INF_) {
          msg_p_min = GUM_SCALAR(1.);
        } else if (min == 0. || lmin == 0.) {
          msg_p_min = GUM_SCALAR(0.);
        } else {
          msg_p_min = GUM_SCALAR(1. / (1. + ((1. / min - 1.) * 1. / lmin)));
        }

        if (max == INF_ && lmax == 0.) {
          std::cout << "proba ERR (negatif) : pi = inf, l = 0" << std::endl;
          return;
        }

        if (lmax == INF_) {
          msg_p_max = GUM_SCALAR(1.);
        } else if (max == 0. || lmax == 0.) {
          msg_p_max = GUM_SCALAR(0.);
        } else {
          msg_p_max = GUM_SCALAR(1. / (1. + ((1. / max - 1.) * 1. / lmax)));
        }
      }

      if (msg_p_min != msg_p_min && msg_p_max == msg_p_max) {
        msg_p_min = msg_p_max;
        std::cout << std::endl;
        std::cout << "msg_p_min is NaN" << std::endl;
      }

      if (msg_p_max != msg_p_max && msg_p_min == msg_p_min) {
        msg_p_max = msg_p_min;
        std::cout << std::endl;
        std::cout << "msg_p_max is NaN" << std::endl;
      }

      if (msg_p_max != msg_p_max && msg_p_min != msg_p_min) {
        std::cout << std::endl;
        std::cout << "Please check the observations (no proba can be computed)" << std::endl;
        return;
      }

      if (msg_p_min < 0.) { msg_p_min = 0.; }

      if (msg_p_max < 0.) { msg_p_max = 0.; }

      _infE_::marginalMin_[node][0] = 1 - msg_p_max;
      _infE_::marginalMax_[node][0] = 1 - msg_p_min;
      _infE_::marginalMin_[node][1] = msg_p_min;
      _infE_::marginalMax_[node][1] = msg_p_max;
    }
  }

  template < typename GUM_SCALAR >
  GUM_SCALAR CNLoopyPropagation< GUM_SCALAR >::calculateEpsilon_() {
    refreshLMsPIs_();
    updateMarginals_();

    return _infE_::computeEpsilon_();
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::updateIndicatrices_() {
    for (auto node: _bnet_->nodes()) {
      if (_cn_->currentNodeType(node) != CredalNet< GUM_SCALAR >::NodeType::Indic) { continue; }

      for (auto pare: _bnet_->parents(node)) {
        msgP_(pare, node);
      }
    }

    refreshLMsPIs_(true);
    updateMarginals_();
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::computeExpectations_() {
    if (_infE_::modal_.empty()) { return; }

    std::vector< std::vector< GUM_SCALAR > > vertices(2, std::vector< GUM_SCALAR >(2));

    for (auto node: _bnet_->nodes()) {
      vertices[0][0] = _infE_::marginalMin_[node][0];
      vertices[0][1] = _infE_::marginalMax_[node][1];

      vertices[1][0] = _infE_::marginalMax_[node][0];
      vertices[1][1] = _infE_::marginalMin_[node][1];

      for (auto vertex = 0, vend = 2; vertex != vend; vertex++) {
        _infE_::updateExpectations_(node, vertices[vertex]);
        // test credal sets vertices elim
        // remove with L2U since variables are binary
        // but does the user know that ?
        _infE_::updateCredalSets_(node,
                                  vertices[vertex]);   // no redundancy elimination with 2 vertices
      }
    }
  }

  template < typename GUM_SCALAR >
  CNLoopyPropagation< GUM_SCALAR >::CNLoopyPropagation(const CredalNet< GUM_SCALAR >& cnet) :
      InferenceEngine< GUM_SCALAR >::InferenceEngine(cnet) {
    if (!cnet.isSeparatelySpecified()) {
      GUM_ERROR(OperationNotAllowed,
                "CNLoopyPropagation is only available "
                "with separately specified nets");
    }

    // test for binary cn
    for (auto node: cnet.current_bn().nodes())
      if (cnet.current_bn().variable(node).domainSize() != 2) {
        GUM_ERROR(OperationNotAllowed,
                  "CNLoopyPropagation is only available "
                  "with binary credal networks")
      }

    // test if compute CPTMinMax has been called
    if (!cnet.hasComputedBinaryCPTMinMax()) {
      GUM_ERROR(OperationNotAllowed,
                "CNLoopyPropagation only works when "
                "\"computeBinaryCPTMinMax()\" has been called for "
                "this credal net")
    }

    _cn_   = &cnet;
    _bnet_ = &cnet.current_bn();

    _inferenceType_    = InferenceType::nodeToNeighbours;
    InferenceUpToDate_ = false;

    GUM_CONSTRUCTOR(CNLoopyPropagation)
  }

  template < typename GUM_SCALAR >
  CNLoopyPropagation< GUM_SCALAR >::~CNLoopyPropagation() {
    InferenceUpToDate_ = false;

    if (!msg_l_sent_.empty()) {
      for (auto node: _bnet_->nodes()) {
        delete msg_l_sent_[node];
      }
    }

    GUM_DESTRUCTOR(CNLoopyPropagation)
  }

  template < typename GUM_SCALAR >
  void CNLoopyPropagation< GUM_SCALAR >::inferenceType(InferenceType inft) {
    _inferenceType_ = inft;
  }

  template < typename GUM_SCALAR >
  typename CNLoopyPropagation< GUM_SCALAR >::InferenceType
     CNLoopyPropagation< GUM_SCALAR >::inferenceType() {
    return _inferenceType_;
  }
}   // namespace gum::credal

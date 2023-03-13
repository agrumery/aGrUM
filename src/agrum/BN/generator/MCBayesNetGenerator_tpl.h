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
 * @brief Source implementation of MCBayesNetGenerator
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) and Ariele-Paolo MAESANO
 *
 */

#include <agrum/BN/generator/MCBayesNetGenerator.h>
#define IBNG IBayesNetGenerator< GUM_SCALAR, ICPTGenerator >

namespace gum {
  template < typename GUM_SCALAR >
  gum::Size getMaxModality(gum::BayesNet< GUM_SCALAR >& bayesNet) {
    gum::Size maxMod = 0;

    for (auto node: bayesNet.nodes())
      if (maxMod < bayesNet.variable(node).domainSize())
        maxMod = bayesNet.variable(node).domainSize();

    return maxMod;
  }

  // Default constructor.
  // Use the SimpleCPTGenerator for generating the BNs CPT.
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::MCBayesNetGenerator(
     Size nbrNodes,
     Size maxArcs,
     Idx  maxModality,
     Size iteration,
     Idx  p,
     Idx  q) :
      IBNG(nbrNodes, maxArcs, maxModality) {
    if (p + q > 100)
      GUM_ERROR(OperationNotAllowed,
                "the sum of the probabilities p and q must be at most equal to 100");

    iteration_ = iteration;
    p_         = p;
    q_         = q;

    GUM_CONSTRUCTOR(MCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::MCBayesNetGenerator(
     BayesNet< GUM_SCALAR > bayesNet,
     Size                   iteration,
     Idx                    p,
     Idx                    q) :
      MCBayesNetGenerator(bayesNet.size(),
                          (Size)(bayesNet.sizeArcs() * 1.1),
                          getMaxModality(bayesNet)) {
    iteration_ = iteration;
    p_         = p;
    q_         = q;
  }

  // Destructor.
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::~MCBayesNetGenerator() {
    GUM_DESTRUCTOR(MCBayesNetGenerator);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::generateBN(
     BayesNet< GUM_SCALAR >& bayesNet) {
    Idx   iteration = iteration_;
    Timer timer;
    _createTree_(this->nbrNodes_);
    _transformPoly_(this->nbrNodes_ / 2);
    _PMMx_poly_();
    this->fromDAG(bayesNet);

    this->fromDAG(bayesNet);
    this->fillCPT(bayesNet);
    iteration_ = iteration;
  }

  /*
    template < typename GUM_SCALAR,
               template < typename >
               class ICPTGenerator,
               template < typename >
               class ICPTDisturber >
    void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::disturbBN(
       BayesNet< GUM_SCALAR >& bayesNetinit,
       Size                    iteration) {   // insert option for the variation
      GUM_ERROR(NotImplementedYet,"Badly implemented method")

      disturbing_ = true;
      Size iter   = iteration_;

      if (iteration) iteration_ = iteration;

      this->bayesNet_ = bayesNetinit;

      if (_checkConditions_()) {
        LazyPropagation< GUM_SCALAR > inf(&bayesNetinit);
        inf.makeInference();

        for (auto node: bayesNetinit.nodes()) {
          auto pottemp = new Potential< GUM_SCALAR >();
          pottemp->copy(inf.posterior(node));
          hashMarginal_.insert(node, pottemp);
        }

        bayesNettemp_ = this->bayesNet_;

        if (_isPolytree_()) _PMMx_poly_();
        else _PMMx_multi_();

        bayesNetinit = (this->bayesNet_);

        while (hashMarginal_.size()) {
          delete (hashMarginal_.begin().val());
          hashMarginal_.erase(hashMarginal_.beginSafe());   // safe iterator needed here.
        }

      } else {
        std::cout << this->bayesNet_.toDot() << std::endl;
        GUM_ERROR(OperationNotAllowed, "BN is not valid cause it does not respect constraint ")
      }

      iteration_  = iter;
      disturbing_ = false;
    }
  */

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE bool MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_checkConditions_() {
    return this->maxArcs_ >= this->dag_.sizeArcs();
  }

  // main algorithme for moving between state of the IBayesNet according on the
  // nature of the topology polytree or multi-connected

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_PMMx_poly_() {
    while (true) {
      if (!iteration_--) return;
      DAG tmp_dag = this->dag_;
      Idx per     = randomValue(100);

      if (per < p_) {
        _AorR_();

        if (_checkConditions_()) {
          tmp_dag = this->dag_;
          _PMMx_multi_();
          break;
        } else {
          this->dag_ = tmp_dag;
        }
      } else {
        if (per < p_ + q_) {
          _AR_();

          if (!_checkConditions_()) {
            this->dag_ = tmp_dag;
          } else {
            tmp_dag = this->dag_;
          }
        } else {
          _jump_poly_();

          if (_checkConditions_()) {
            tmp_dag = this->dag_;
            _PMMx_multi_();
            break;
          } else {
            this->dag_ = tmp_dag;
          }
        }
      }
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_PMMx_multi_() {
    while (true) {
      if (!iteration_--) return;
      DAG tmp_dag = this->dag_;

      Idx per = randomValue(100);

      if (per < p_ + q_) {
        _AorR_();
        if (_checkConditions_()) {
          if (_isPolytree_()) {
            if (per < p_) {
              tmp_dag = this->dag_;
              _PMMx_poly_();
              break;
            } else {
              this->dag_ = tmp_dag;
            }
          } else {
            tmp_dag = this->dag_;
          }
        } else {
          this->dag_ = tmp_dag;
        }
      } else {
        _jump_multi_();
        if (_checkConditions_()) {
          tmp_dag = this->dag_;
          if (_isPolytree_()) {
            _PMMx_poly_();
            break;
          }
        } else {
          this->dag_ = tmp_dag;
        }
      }
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_AorR_() {
    NodeId i, j;
    _chooseNodes_(i, j);
    if (this->dag_.existsArc(i, j)) {
      _eraseArc_(i, j);

      return;
    } else _insertArc_(i, j);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_AR_() {
    NodeId i, j, head, tail;
    _chooseNodes_(i, j);

    if (this->dag_.existsArc(i, j) || this->dag_.existsArc(j, i)) {
      return;
    } else {
      Idx per = randomValue(100);

      if (per < 50) {
        head = i;
        tail = j;
      } else {
        head = j;
        tail = i;
      }

      for (auto node: this->dag_.parents(j)) {
        NodeSet excluded;
        excluded.insert(j);

        if (_is_connected_(node, i, excluded)) {
          this->dag_.eraseArc(Arc(node, j));
          this->dag_.addArc(head, tail);
          return;
        }
      }

      for (auto node: this->dag_.children(j)) {
        NodeSet excluded;
        excluded.insert(j);

        if (_is_connected_(node, i, excluded)) {
          this->dag_.eraseArc(Arc{j, node});
          this->dag_.addArc(head, tail);
          return;
        }
      }
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_jump_poly_() {
    NodeId i, j;
    _chooseNodes_(i, j);

    if (!this->dag_.existsArc(i, j)) _insertArc_(i, j);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_jump_multi_() {
    NodeId i, j;
    _chooseNodes_(i, j);

    if (this->dag_.existsArc(i, j)) { _eraseArc_(i, j); }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_insertArc_(NodeId i,
                                                                                  NodeId j) {
    if (_directedPath_(j, i)) return;

    /*if (disturbing_) {
      auto potj = this->bayesNet_.cpt(j);
      this->bayesNet_.addArc(i, j);

      this->disturbAugmCPT(j, this->bayesNet_, potj, (GUM_SCALAR)0.5);
    } else */
    this->dag_.addArc(i, j);
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_eraseArc_(
     NodeId i,
     NodeId j,
     bool   mustbeconnex) {
    /*if (disturbing_) {
      const BayesNet< GUM_SCALAR > bayesNet(this->bayesNet_);
      Potential< GUM_SCALAR >      potj;
      potj.copy(this->bayesNet_.cpt(j));
      this->bayesNet_.eraseArc(i, j);

      if (_connect_(i, j) || !mustbeconnex) {
        auto marg = *hashMarginal_[i];

        this->disturbReducCPT(j, this->bayesNet_, potj, marg);
      } else this->bayesNet_.addArc(i, j);
    } else */
    {
      this->dag_.eraseArc(Arc(i, j));

      if (!_connect_(i, j) && mustbeconnex) { this->dag_.addArc(i, j); }
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_chooseNodes_(NodeId& i,
                                                                                    NodeId& j) {
    if (this->dag_.size() < 3) {
      GUM_ERROR(ArgumentError, "This dag has only " << this->dag_.size() << " nodes.")
    }
    i = randomValue(this->dag_.size());
    j = randomValue(this->dag_.size());

    while (i == j)
      j = randomValue(this->dag_.size());
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_chooseCloseNodes_(
     NodeId& i,
     NodeId& j) {
    NodeId temp = randomValue(this->dag_.size());
    Size   co   = 0;

    if (this->dag_.parents(temp).size()) {
      j       = temp;
      auto it = this->dag_.parents(j).begin();
      co      = randomValue(this->dag_.parents(j).size());

      while (co--) {
        ++it;
      }

      i = *it;
    } else if (this->dag_.children(temp).size()) {
      i       = temp;
      auto it = this->dag_.children(i).begin();
      co      = randomValue(this->dag_.children(i).size());

      while (co--) {
        ++it;
      }

      j = *it;
    } else {
      GUM_ERROR(FatalError, "Sorry Misconstructed BN because of isolated node.")
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_createTree_(Size BNSize) {
    Idx    n        = 0;
    NodeId root     = this->dag_.addNode();
    Size   maxNodes = BNSize - 1;
    Size   SubG     = 0;

    while (maxNodes) {
      SubG         = randomValue(maxNodes) + 1;
      maxNodes     = maxNodes - SubG;
      NodeId rootS = _createPartTree_(SubG, n);
      this->dag_.addArc(root, rootS);
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  NodeId
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_createPartTree_(Size BNSize,
                                                                                       Idx& n) {
    /*
    Size              nb_mod = 2 + randomValue(this->maxModality_ - 1);
    std::stringstream strBuff;
    strBuff << "n_" << n++;
    NodeId root     = this->bayesNet_.add(LabelizedVariable(strBuff.str(), "", nb_mod));
     */
    NodeId root     = this->dag_.addNode();
    Size   maxNodes = BNSize - 1;
    Size   SubG     = 0;

    while (maxNodes) {
      SubG         = randomValue(maxNodes) + 1;
      maxNodes     = maxNodes - SubG;
      NodeId rootS = _createPartTree_(SubG, n);
      this->dag_.addArc(root, rootS);
    }

    return root;
  }

  // Allow to invert maximum nbiter arc to use from polytree only
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  void
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_transformPoly_(Idx nbiter) {
    while (nbiter--) {
      NodeId i, j;
      _chooseCloseNodes_(i, j);
      auto dag_tmp = this->dag_;
      _eraseArc_(i, j, false);
      this->dag_.addArc(j, i);

      if (!_checkConditions_()) this->dag_ = dag_tmp;
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE bool MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_isPolytree_() {
    return this->dag_.size() - 1 == this->dag_.sizeArcs();
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  bool MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_connect_(const NodeId i,
                                                                                  const NodeId j) {
    if (this->dag_.existsArc(i, j) || this->dag_.existsArc(j, i)) return true;
    else {
      NodeSet excluded;
      excluded.insert(i);

      for (auto par: this->dag_.parents(i)) {
        if (!excluded.exists(par) && _is_connected_(par, j, excluded)) return true;
      }

      for (auto chi: this->dag_.children(i)) {
        if (!excluded.exists(chi) && _is_connected_(chi, j, excluded)) return true;
      }

      return false;
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  bool MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_is_connected_(
     const NodeId i,
     const NodeId j,
     NodeSet&     excluded) {
    if (this->dag_.existsArc(i, j) || this->dag_.existsArc(j, i)) return true;
    else {
      excluded.insert(i);

      for (auto par: this->dag_.parents(i)) {
        if (!excluded.exists(par) && _is_connected_(par, j, excluded)) return true;
      }

      for (auto chi: this->dag_.children(i)) {
        if (!excluded.exists(chi) && _is_connected_(chi, j, excluded)) return true;
      }

      return false;
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  bool
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_directedPath_(NodeId tail,
                                                                                     NodeId head) {
    if (this->dag_.existsArc(tail, head)) return true;
    else {
      NodeSet excluded;
      excluded.insert(tail);

      for (auto node: this->dag_.children(tail)) {
        if (_directedPath_(node, head, excluded)) return true;
      }

      return false;
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  bool MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::_directedPath_(
     NodeId   tail,
     NodeId   head,
     NodeSet& excluded) {
    if (this->dag_.existsArc(tail, head)) return true;
    else {
      excluded.insert(tail);

      for (auto node: this->dag_.children(tail)) {
        if (!excluded.exists(node) && _directedPath_(node, head, excluded)) return true;
      }

      return false;
    }
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE Size MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::iteration() const {
    return iteration_;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE Idx MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::p() const {
    return p_;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE Idx MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::q() const {
    return q_;
  }

  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void
     MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::setIteration(Size iteration) {
    iteration_ = iteration;
  }
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::setP(Idx p) {
    p_ = p;

    if (p + q_ > 100)
      GUM_ERROR(OperationNotAllowed,
                "the sum of the probabilities p and q must be at most equal to 100");
  }
  template < typename GUM_SCALAR,
             template < typename >
             class ICPTGenerator,
             template < typename >
             class ICPTDisturber >
  INLINE void MCBayesNetGenerator< GUM_SCALAR, ICPTGenerator, ICPTDisturber >::setQ(Idx q) {
    q_ = q;

    if (p_ + q > 100)
      GUM_ERROR(OperationNotAllowed,
                "the sum of the probabilities p and q must be at most equal to 100");
  }

} /* namespace gum */

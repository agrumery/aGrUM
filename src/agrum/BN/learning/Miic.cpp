
/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe
 * GONZALES(@AMU) info_at_agrum_dot_org
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
 * @brief Implementation of gum::learning::ThreeOffTwo and MIIC
 *
 * @author Quentin FALCAND, Marvin LASSERRE and Pierre-Henri WUILLEMIN(@LIP6)
 */

#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/core/hashTable.h>
#include <agrum/tools/core/heap.h>
#include <agrum/tools/core/timer.h>
#include <agrum/tools/graphs/mixedGraph.h>
#include <agrum/BN/learning/Miic.h>
#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>
#include <agrum/tools/stattests/correctedMutualInformation.h>


namespace gum {

  namespace learning {

    /// default constructor
    Miic::Miic(){GUM_CONSTRUCTOR(Miic)}

    /// default constructor with maxLog
    Miic::Miic(int maxLog) :
        _maxLog_(maxLog),
        _size_(0) {
      GUM_CONSTRUCTOR(Miic);
    }

    /// copy constructor
    Miic::Miic(const Miic& from) : ApproximationScheme(from), _size_(from._size_) {
      GUM_CONS_CPY(Miic);
    }

    /// move constructor
    Miic::Miic(Miic&& from) : ApproximationScheme(std::move(from)), _size_(from._size_) {
      GUM_CONS_MOV(Miic);
    }

    /// destructor
    Miic::~Miic() { GUM_DESTRUCTOR(Miic); }

    /// copy operator
    Miic& Miic::operator=(const Miic& from) {
      ApproximationScheme::operator=(from);
      return *this;
    }

    /// move operator
    Miic& Miic::operator=(Miic&& from) {
      ApproximationScheme::operator=(std::move(from));
      return *this;
    }


    bool GreaterPairOn2nd::operator()(const CondRanking& e1, const CondRanking& e2) const {
      return e1.second > e2.second;
    }

    bool GreaterAbsPairOn2nd::operator()(const Ranking& e1, const Ranking& e2) const {
      return std::abs(e1.second) > std::abs(e2.second);
    }

    bool GreaterTupleOnLast::operator()(const ProbabilisticRanking& e1,
                                        const ProbabilisticRanking& e2) const {
      double p1xz = std::get< 2 >(e1);
      double p1yz = std::get< 3 >(e1);
      double p2xz = std::get< 2 >(e2);
      double p2yz = std::get< 3 >(e2);
      double I1   = std::get< 1 >(e1);
      double I2   = std::get< 1 >(e2);
      // First, we look at the sign of information.
      // Then, the probability values
      // and finally the abs value of information.
      if ((I1 < 0 && I2 < 0) || (I1 >= 0 && I2 >= 0)) {
        if (std::max(p1xz, p1yz) == std::max(p2xz, p2yz)) {
          return std::abs(I1) > std::abs(I2);
        } else {
          return std::max(p1xz, p1yz) > std::max(p2xz, p2yz);
        }
      } else {
        return I1 < I2;
      }
    }

    /// learns the structure of a MixedGraph
    MixedGraph Miic::learnMixedStructure(CorrectedMutualInformation<>& mutualInformation,
                                         MixedGraph                    graph) {
      timer_.reset();
      current_step_ = 0;

      // clear the vector of latent arcs to be sure
      _latentCouples_.clear();

      /// the heap of ranks, with the score, and the NodeIds of x, y and z.
      Heap< CondRanking, GreaterPairOn2nd > rank;

      /// the variables separation sets
      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > > sep_set;

      initiation_(mutualInformation, graph, sep_set, rank);

      iteration_(mutualInformation, graph, sep_set, rank);

      // std::cout << "Le graphe contient: " << graph.sizeEdges() << " edges." <<
      // std::endl; std::cout << "En voici la liste: " << graph.edges() <<
      // std::endl;

      if (_useMiic_) {
        orientationMiic_(mutualInformation, graph, sep_set);
      } else {
        orientation3off2_(mutualInformation, graph, sep_set);
      }

      return graph;
    }

    /*
     * PHASE 1 : INITIATION
     *
     * We go over all edges and test if the variables are independent. If they
     * are,
     * the edge is deleted. If not, the best contributor is found.
     */
    void Miic::initiation_(CorrectedMutualInformation<>& mutualInformation,
                           MixedGraph&                   graph,
                           HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                           Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      NodeId  x, y;
      EdgeSet edges      = graph.edges();
      Size    steps_init = edges.size();

      for (const Edge& edge: edges) {
        x          = edge.first();
        y          = edge.second();
        double Ixy = mutualInformation.score(x, y);

        if (Ixy <= 0) {   //< K
          graph.eraseEdge(edge);
          sepSet.insert(std::make_pair(x, y), _emptySet_);
        } else {
          findBestContributor_(x, y, _emptySet_, graph, mutualInformation, rank);
        }

        ++current_step_;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress, (current_step_ * 33) / steps_init, 0., timer_.step());
        }
      }
    }

    /*
     * PHASE 2 : ITERATION
     *
     * As long as we find important nodes for edges, we go over them to see if
     * we can assess the independence of the variables.
     */
    void Miic::iteration_(CorrectedMutualInformation<>& mutualInformation,
                          MixedGraph&                   graph,
                          HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                          Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      // if no triples to further examine pass
      CondRanking best;

      Size steps_init = current_step_;
      Size steps_iter = rank.size();

      try {
        while (rank.top().second > 0.5) {
          best = rank.pop();

          const NodeId          x  = std::get< 0 >(*(best.first));
          const NodeId          y  = std::get< 1 >(*(best.first));
          const NodeId          z  = std::get< 2 >(*(best.first));
          std::vector< NodeId > ui = std::move(std::get< 3 >(*(best.first)));

          ui.push_back(z);
          const double i_xy_ui = mutualInformation.score(x, y, ui);
          if (i_xy_ui < 0) {
            graph.eraseEdge(Edge(x, y));
            sepSet.insert(std::make_pair(x, y), std::move(ui));
          } else {
            findBestContributor_(x, y, ui, graph, mutualInformation, rank);
          }

          delete best.first;

          ++current_step_;
          if (onProgress.hasListener()) {
            GUM_EMIT3(onProgress,
                      (current_step_ * 66) / (steps_init + steps_iter),
                      0.,
                      timer_.step());
          }
        }
      } catch (...) {}   // here, rank is empty
      current_step_ = steps_init + steps_iter;
      if (onProgress.hasListener()) { GUM_EMIT3(onProgress, 66, 0., timer_.step()); }
      current_step_ = steps_init + steps_iter;
    }

    /*
     * PHASE 3 : ORIENTATION
     *
     * Try to assess v-structures and propagate them.
     */
    void Miic::orientation3off2_(
       CorrectedMutualInformation<>&                                          mutualInformation,
       MixedGraph&                                                            graph,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      std::vector< Ranking > triples      = unshieldedTriples_(graph, mutualInformation, sepSet);
      Size                   steps_orient = triples.size();
      Size                   past_steps   = current_step_;

      // marks always correspond to the head of the arc/edge. - is for a forbidden
      // arc, > for a mandatory arc
      // we start by adding the mandatory arcs
      for (auto iter = _initialMarks_.begin(); iter != _initialMarks_.end(); ++iter) {
        if (graph.existsEdge(iter.key().first, iter.key().second) && iter.val() == '>') {
          graph.eraseEdge(Edge(iter.key().first, iter.key().second));
          graph.addArc(iter.key().first, iter.key().second);
        }
      }

      NodeId i = 0;
      // list of elements that we shouldnt read again, ie elements that are
      // eligible to
      // rule 0 after the first time they are tested, and elements on which rule 1
      // has been applied
      while (i < triples.size()) {
        // if i not in do_not_reread
        Ranking triple = triples[i];
        NodeId  x, y, z;
        x = std::get< 0 >(*triple.first);
        y = std::get< 1 >(*triple.first);
        z = std::get< 2 >(*triple.first);

        std::vector< NodeId >       ui;
        std::pair< NodeId, NodeId > key     = {x, y};
        std::pair< NodeId, NodeId > rev_key = {y, x};
        if (sepSet.exists(key)) {
          ui = sepSet[key];
        } else if (sepSet.exists(rev_key)) {
          ui = sepSet[rev_key];
        }
        double Ixyz_ui = triple.second;
        bool   reset{false};
        // try Rule 0
        if (Ixyz_ui < 0) {
          // if ( z not in Sep[x,y])
          if (std::find(ui.begin(), ui.end(), z) == ui.end()) {
            if (!graph.existsArc(x, z) && !graph.existsArc(z, x)) {
              // when we try to add an arc to the graph, we always verify if
              // we are allowed to do so, ie it is not a forbidden arc an it
              // does not create a cycle
              if (!_existsDirectedPath_(graph, z, x)
                  && !(_initialMarks_.exists({x, z}) && _initialMarks_[{x, z}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(x, z));
                graph.addArc(x, z);
              } else if (_existsDirectedPath_(graph, z, x)
                         && !(_initialMarks_.exists({z, x}) && _initialMarks_[{z, x}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(x, z));
                // if we find a cycle, we force the competing edge
                graph.addArc(z, x);
                if (std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(z, x))
                    == _latentCouples_.end()) {
                  _latentCouples_.emplace_back(z, x);
                }
              }
            } else if (!graph.existsArc(y, z) && !graph.existsArc(z, y)) {
              if (!_existsDirectedPath_(graph, z, y)
                  && !(_initialMarks_.exists({x, z}) && _initialMarks_[{x, z}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(y, z));
                graph.addArc(y, z);
              } else if (_existsDirectedPath_(graph, z, y)
                         && !(_initialMarks_.exists({z, y}) && _initialMarks_[{z, y}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(y, z));
                // if we find a cycle, we force the competing edge
                graph.addArc(z, y);
                if (std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(z, y))
                    == _latentCouples_.end()) {
                  _latentCouples_.emplace_back(z, y);
                }
              }
            } else {
              // checking if the anti-directed arc already exists, to register a
              // latent variable
              if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
                _latentCouples_.emplace_back(z, x);
              }
              if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
                _latentCouples_.emplace_back(z, y);
              }
            }
          }
        } else {   // try Rule 1
          if (graph.existsArc(x, z) && !graph.existsArc(z, y) && !graph.existsArc(y, z)) {
            if (!_existsDirectedPath_(graph, y, z)
                && !(_initialMarks_.exists({z, y}) && _initialMarks_[{z, y}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, y));
              graph.addArc(z, y);
            } else if (_existsDirectedPath_(graph, y, z)
                       && !(_initialMarks_.exists({y, z}) && _initialMarks_[{y, z}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, y));
              // if we find a cycle, we force the competing edge
              graph.addArc(y, z);
              if (std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(y, z))
                  == _latentCouples_.end()) {
                _latentCouples_.emplace_back(y, z);
              }
            }
          }
          if (graph.existsArc(y, z) && !graph.existsArc(z, x) && !graph.existsArc(x, z)) {
            if (!_existsDirectedPath_(graph, x, z)
                && !(_initialMarks_.exists({z, x}) && _initialMarks_[{z, x}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, x));
              graph.addArc(z, x);
            } else if (_existsDirectedPath_(graph, x, z)
                       && !(_initialMarks_.exists({x, z}) && _initialMarks_[{x, z}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, x));
              // if we find a cycle, we force the competing edge
              graph.addArc(x, z);
              if (std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(x, z))
                  == _latentCouples_.end()) {
                _latentCouples_.emplace_back(x, z);
              }
            }
          }
        }   // if rule 0 or rule 1

        // if what we want to add already exists : pass to the next triplet
        if (reset) {
          i = 0;
        } else {
          ++i;
        }
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    ((current_step_ + i) * 100) / (past_steps + steps_orient),
                    0.,
                    timer_.step());
        }
      }   // while

      // erasing the the double headed arcs
      for (const Arc& arc: _latentCouples_) {
        graph.eraseArc(Arc(arc.head(), arc.tail()));
      }
    }

    /// varient trying to propagate both orientations in a bidirected arc
    void Miic::orientationLatents_(
       CorrectedMutualInformation<>&                                          mutualInformation,
       MixedGraph&                                                            graph,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      std::vector< Ranking > triples      = unshieldedTriples_(graph, mutualInformation, sepSet);
      Size                   steps_orient = triples.size();
      Size                   past_steps   = current_step_;

      NodeId i = 0;
      // list of elements that we shouldnt read again, ie elements that are
      // eligible to
      // rule 0 after the first time they are tested, and elements on which rule 1
      // has been applied
      while (i < triples.size()) {
        // if i not in do_not_reread
        Ranking triple = triples[i];
        NodeId  x, y, z;
        x = std::get< 0 >(*triple.first);
        y = std::get< 1 >(*triple.first);
        z = std::get< 2 >(*triple.first);

        std::vector< NodeId >       ui;
        std::pair< NodeId, NodeId > key     = {x, y};
        std::pair< NodeId, NodeId > rev_key = {y, x};
        if (sepSet.exists(key)) {
          ui = sepSet[key];
        } else if (sepSet.exists(rev_key)) {
          ui = sepSet[rev_key];
        }
        double Ixyz_ui = triple.second;
        // try Rule 0
        if (Ixyz_ui < 0) {
          // if ( z not in Sep[x,y])
          if (std::find(ui.begin(), ui.end(), z) == ui.end()) {
            // if what we want to add already exists : pass
            if ((graph.existsArc(x, z) || graph.existsArc(z, x))
                && (graph.existsArc(y, z) || graph.existsArc(z, y))) {
              ++i;
            } else {
              i = 0;
              graph.eraseEdge(Edge(x, z));
              graph.eraseEdge(Edge(y, z));
              // checking for cycles
              if (graph.existsArc(z, x)) {
                graph.eraseArc(Arc(z, x));
                try {
                  std::vector< NodeId > path = graph.directedPath(z, x);
                  // if we find a cycle, we force the competing edge
                  _latentCouples_.emplace_back(z, x);
                } catch (gum::NotFound) { graph.addArc(x, z); }
                graph.addArc(z, x);
              } else {
                try {
                  std::vector< NodeId > path = graph.directedPath(z, x);
                  // if we find a cycle, we force the competing edge
                  graph.addArc(z, x);
                  _latentCouples_.emplace_back(z, x);
                } catch (gum::NotFound) { graph.addArc(x, z); }
              }
              if (graph.existsArc(z, y)) {
                graph.eraseArc(Arc(z, y));
                try {
                  std::vector< NodeId > path = graph.directedPath(z, y);
                  // if we find a cycle, we force the competing edge
                  _latentCouples_.emplace_back(z, y);
                } catch (gum::NotFound) { graph.addArc(y, z); }
                graph.addArc(z, y);
              } else {
                try {
                  std::vector< NodeId > path = graph.directedPath(z, y);
                  // if we find a cycle, we force the competing edge
                  graph.addArc(z, y);
                  _latentCouples_.emplace_back(z, y);

                } catch (gum::NotFound) { graph.addArc(y, z); }
              }
              if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
                _latentCouples_.emplace_back(z, x);
              }
              if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
                _latentCouples_.emplace_back(z, y);
              }
            }
          } else {
            ++i;
          }
        } else {   // try Rule 1
          bool reset{false};
          if (graph.existsArc(x, z) && !graph.existsArc(z, y) && !graph.existsArc(y, z)) {
            reset = true;
            graph.eraseEdge(Edge(z, y));
            try {
              std::vector< NodeId > path = graph.directedPath(y, z);
              // if we find a cycle, we force the competing edge
              graph.addArc(y, z);
              _latentCouples_.emplace_back(y, z);
            } catch (gum::NotFound) { graph.addArc(z, y); }
          }
          if (graph.existsArc(y, z) && !graph.existsArc(z, x) && !graph.existsArc(x, z)) {
            reset = true;
            graph.eraseEdge(Edge(z, x));
            try {
              std::vector< NodeId > path = graph.directedPath(x, z);
              // if we find a cycle, we force the competing edge
              graph.addArc(x, z);
              _latentCouples_.emplace_back(x, z);
            } catch (gum::NotFound) { graph.addArc(z, x); }
          }

          if (reset) {
            i = 0;
          } else {
            ++i;
          }
        }   // if rule 0 or rule 1
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    ((current_step_ + i) * 100) / (past_steps + steps_orient),
                    0.,
                    timer_.step());
        }
      }   // while

      // erasing the the double headed arcs
      for (const Arc& arc: _latentCouples_) {
        graph.eraseArc(Arc(arc.head(), arc.tail()));
      }
    }

    /// varient using the orientation protocol of MIIC
    void Miic::orientationMiic_(
       CorrectedMutualInformation<>&                                          mutualInformation,
       MixedGraph&                                                            graph,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      // structure to store the orientations marks -, o, or >,
      // Considers the head of the arc/edge first node -* second node
      HashTable< std::pair< NodeId, NodeId >, char > marks = _initialMarks_;

      // marks always correspond to the head of the arc/edge. - is for a forbidden
      // arc, > for a mandatory arc
      // we start by adding the mandatory arcs
      for (auto iter = marks.begin(); iter != marks.end(); ++iter) {
        if (graph.existsEdge(iter.key().first, iter.key().second) && iter.val() == '>') {
          graph.eraseEdge(Edge(iter.key().first, iter.key().second));
          graph.addArc(iter.key().first, iter.key().second);
        }
      }

      std::vector< ProbabilisticRanking > proba_triples
         = unshieldedTriplesMiic_(graph, mutualInformation, sepSet, marks);

      const Size steps_orient = proba_triples.size();
      Size       past_steps   = current_step_;

      ProbabilisticRanking best;
      if (steps_orient > 0) { best = proba_triples[0]; }

      while (!proba_triples.empty() && std::max(std::get< 2 >(best), std::get< 3 >(best)) > 0.5) {
        const NodeId x = std::get< 0 >(*std::get< 0 >(best));
        const NodeId y = std::get< 1 >(*std::get< 0 >(best));
        const NodeId z = std::get< 2 >(*std::get< 0 >(best));

        const double i3 = std::get< 1 >(best);

        const double p1 = std::get< 2 >(best);
        const double p2 = std::get< 3 >(best);
        if (i3 <= 0) {
          _orientingVstructureMiic_(graph, marks, x, y, z, p1, p2);
        } else {
          _propagatingOrientationMiic_(graph, marks, x, y, z, p1, p2);
        }

        delete std::get< 0 >(best);
        proba_triples.erase(proba_triples.begin());
        // actualisation of the list of triples
        proba_triples = updateProbaTriples_(graph, proba_triples);

        if (!proba_triples.empty()) best = proba_triples[0];

        ++current_step_;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    (current_step_ * 100) / (steps_orient + past_steps),
                    0.,
                    timer_.step());
        }
      }   // while

      // erasing the double headed arcs
      for (auto iter = _latentCouples_.rbegin(); iter != _latentCouples_.rend(); ++iter) {
        graph.eraseArc(Arc(iter->head(), iter->tail()));
        if (_existsDirectedPath_(graph, iter->head(), iter->tail())) {
          // if we find a cycle, we force the competing edge
          graph.addArc(iter->head(), iter->tail());
          graph.eraseArc(Arc(iter->tail(), iter->head()));
          *iter = Arc(iter->head(), iter->tail());
        }
      }

      if (onProgress.hasListener()) { GUM_EMIT3(onProgress, 100, 0., timer_.step()); }
    }

    /// finds the best contributor node for a pair given a conditioning set
    void Miic::findBestContributor_(NodeId                                 x,
                                    NodeId                                 y,
                                    const std::vector< NodeId >&           ui,
                                    const MixedGraph&                      graph,
                                    CorrectedMutualInformation<>&          mutualInformation,
                                    Heap< CondRanking, GreaterPairOn2nd >& rank) {
      double maxP = -1.0;
      NodeId maxZ = 0;

      // compute N
      // __N = I.N();
      const double Ixy_ui = mutualInformation.score(x, y, ui);

      for (const NodeId z: graph) {
        // if z!=x and z!=y and z not in ui
        if (z != x && z != y && std::find(ui.begin(), ui.end(), z) == ui.end()) {
          double Pnv;
          double Pb;

          // Computing Pnv
          const double Ixyz_ui    = mutualInformation.score(x, y, z, ui);
          double       calc_expo1 = -Ixyz_ui * M_LN2;
          // if exponentials are too high or to low, crop them at | __maxLog|
          if (calc_expo1 > _maxLog_) {
            Pnv = 0.0;
          } else if (calc_expo1 < -_maxLog_) {
            Pnv = 1.0;
          } else {
            Pnv = 1 / (1 + std::exp(calc_expo1));
          }

          // Computing Pb
          const double Ixz_ui = mutualInformation.score(x, z, ui);
          const double Iyz_ui = mutualInformation.score(y, z, ui);

          calc_expo1        = -(Ixz_ui - Ixy_ui) * M_LN2;
          double calc_expo2 = -(Iyz_ui - Ixy_ui) * M_LN2;

          // if exponentials are too high or to low, crop them at  _maxLog_
          if (calc_expo1 > _maxLog_ || calc_expo2 > _maxLog_) {
            Pb = 0.0;
          } else if (calc_expo1 < -_maxLog_ && calc_expo2 < -_maxLog_) {
            Pb = 1.0;
          } else {
            double expo1, expo2;
            if (calc_expo1 < -_maxLog_) {
              expo1 = 0.0;
            } else {
              expo1 = std::exp(calc_expo1);
            }
            if (calc_expo2 < -_maxLog_) {
              expo2 = 0.0;
            } else {
              expo2 = std::exp(calc_expo2);
            }
            Pb = 1 / (1 + expo1 + expo2);
          }

          // Getting max(min(Pnv, pb))
          const double min_pnv_pb = std::min(Pnv, Pb);
          if (min_pnv_pb > maxP) {
            maxP = min_pnv_pb;
            maxZ = z;
          }
        }   // if z not in (x, y)
      }     // for z in graph.nodes
      // storing best z in rank_
      CondRanking final;
      auto        tup = new CondThreePoints{x, y, maxZ, ui};
      final.first     = tup;
      final.second    = maxP;
      rank.insert(final);
    }

    /// gets the list of unshielded triples in the graph in decreasing value of
    ///|I'(x, y, z|{ui})|
    std::vector< Ranking > Miic::unshieldedTriples_(
       const MixedGraph&                                                      graph,
       CorrectedMutualInformation<>&                                          mutualInformation,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      std::vector< Ranking > triples;
      for (NodeId z: graph) {
        for (NodeId x: graph.neighbours(z)) {
          for (NodeId y: graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              std::vector< NodeId >       ui;
              std::pair< NodeId, NodeId > key     = {x, y};
              std::pair< NodeId, NodeId > rev_key = {y, x};
              if (sepSet.exists(key)) {
                ui = sepSet[key];
              } else if (sepSet.exists(rev_key)) {
                ui = sepSet[rev_key];
              }
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) { ui.erase(iter_z_place); }

              double  Ixyz_ui = mutualInformation.score(x, y, z, ui);
              Ranking triple;
              auto    tup   = new ThreePoints{x, y, z};
              triple.first  = tup;
              triple.second = Ixyz_ui;
              triples.push_back(triple);
            }
          }
        }
      }
      std::sort(triples.begin(), triples.end(), GreaterAbsPairOn2nd());
      return triples;
    }

    /// gets the list of unshielded triples in the graph in decreasing value of
    ///|I'(x, y, z|{ui})|, prepares the orientation matrix for MIIC
    std::vector< ProbabilisticRanking > Miic::unshieldedTriplesMiic_(
       const MixedGraph&                                                      graph,
       CorrectedMutualInformation<>&                                          mutualInformation,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
       HashTable< std::pair< NodeId, NodeId >, char >&                        marks) {
      std::vector< ProbabilisticRanking > triples;
      for (NodeId z: graph) {
        for (NodeId x: graph.neighbours(z)) {
          for (NodeId y: graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              std::vector< NodeId >       ui;
              std::pair< NodeId, NodeId > key     = {x, y};
              std::pair< NodeId, NodeId > rev_key = {y, x};
              if (sepSet.exists(key)) {
                ui = sepSet[key];
              } else if (sepSet.exists(rev_key)) {
                ui = sepSet[rev_key];
              }
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) { ui.erase(iter_z_place); }

              const double         Ixyz_ui = mutualInformation.score(x, y, z, ui);
              auto                 tup     = new ThreePoints{x, y, z};
              ProbabilisticRanking triple{tup, Ixyz_ui, 0.5, 0.5};
              triples.push_back(triple);
              if (!marks.exists({x, z})) { marks.insert({x, z}, 'o'); }
              if (!marks.exists({z, x})) { marks.insert({z, x}, 'o'); }
              if (!marks.exists({y, z})) { marks.insert({y, z}, 'o'); }
              if (!marks.exists({z, y})) { marks.insert({z, y}, 'o'); }
            }
          }
        }
      }
      triples = updateProbaTriples_(graph, triples);
      std::sort(triples.begin(), triples.end(), GreaterTupleOnLast());
      return triples;
    }

    /// Gets the orientation probabilities like MIIC for the orientation phase
    std::vector< ProbabilisticRanking >
       Miic::updateProbaTriples_(const MixedGraph&                   graph,
                                 std::vector< ProbabilisticRanking > probaTriples) {
      for (auto& triple: probaTriples) {
        NodeId x, y, z;
        x                 = std::get< 0 >(*std::get< 0 >(triple));
        y                 = std::get< 1 >(*std::get< 0 >(triple));
        z                 = std::get< 2 >(*std::get< 0 >(triple));
        const double Ixyz = std::get< 1 >(triple);
        double       Pxz  = std::get< 2 >(triple);
        double       Pyz  = std::get< 3 >(triple);

        if (Ixyz <= 0) {
          const double expo = std::exp(Ixyz);
          const double P0   = (1 + expo) / (1 + 3 * expo);
          // distinguish betweeen the initialization and the update process
          if (Pxz == Pyz && Pyz == 0.5) {
            std::get< 2 >(triple) = P0;
            std::get< 3 >(triple) = P0;
          } else {
            if (graph.existsArc(x, z) && Pxz >= P0) {
              std::get< 3 >(triple) = Pxz * (1 / (1 + expo) - 0.5) + 0.5;
            } else if (graph.existsArc(y, z) && Pyz >= P0) {
              std::get< 2 >(triple) = Pyz * (1 / (1 + expo) - 0.5) + 0.5;
            }
          }
        } else {
          const double expo = std::exp(-Ixyz);
          if (graph.existsArc(x, z) && Pxz >= 0.5) {
            std::get< 3 >(triple) = Pxz * (1 / (1 + expo) - 0.5) + 0.5;
          } else if (graph.existsArc(y, z) && Pyz >= 0.5) {
            std::get< 2 >(triple) = Pyz * (1 / (1 + expo) - 0.5) + 0.5;
          }
        }
      }
      std::sort(probaTriples.begin(), probaTriples.end(), GreaterTupleOnLast());
      return probaTriples;
    }

    /// learns the structure of an Bayesian network, ie a DAG, from an Essential
    /// graph.
    DAG Miic::learnStructure(CorrectedMutualInformation<>& I, MixedGraph initialGraph) {
      MixedGraph essentialGraph = learnMixedStructure(I, initialGraph);
      // std::cout << "Le mixed graph mesdames et messieurs: "
      //<< essentialGraph.toDot() << std::endl;

      // Second, orientate remaining edges
      const Sequence< NodeId > order = essentialGraph.topologicalOrder();
      // first, propagate existing orientations
      for (NodeId x: order) {
        if (!essentialGraph.parents(x).empty()) { propagatesHead_(essentialGraph, x); }
      }
      // std::cout << "Le mixed graph après une première propagation mesdames et
      // messieurs: "
      //<< essentialGraph.toDot() << std::endl;
      // then decide the orientation by the topological order and propagate them
      for (NodeId x: order) {
        if (!essentialGraph.neighbours(x).empty()) { propagatesHead_(essentialGraph, x); }
      }

      // std::cout << "Le mixed graph après une deuxième propagation mesdames et
      // messieurs: "
      //<< essentialGraph.toDot() << std::endl;
      // std::cout << "Le graphe contient maintenant : " <<
      // essentialGraph.sizeArcs() << " arcs."
      //<< std::endl;
      // std::cout << "Que voici: " << essentialGraph.arcs() << std::endl;
      // turn the mixed graph into a dag
      DAG dag;
      for (auto node: essentialGraph) {
        dag.addNodeWithId(node);
      }
      for (const Arc& arc: essentialGraph.arcs()) {
        dag.addArc(arc.tail(), arc.head());
      }

      return dag;
    }

    /// Propagates the orientation from a node to its neighbours
    void Miic::propagatesHead_(MixedGraph& graph, NodeId node) {
      const auto neighbours = graph.neighbours(node);
      for (auto& neighbour: neighbours) {
        // std::cout << "Orientation de l'edge (" << node << "," << neighbour <<
        // ")" << std::endl;
        if (graph.neighbours(neighbour).contains(node)) {
          if (!_existsDirectedPath_(graph, neighbour, node)
              && !(_initialMarks_.exists({node, neighbour})
                   && _initialMarks_[{node, neighbour}] == '-')
              && graph.parents(neighbour).empty()) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(node, neighbour);
            // std::cout << "1. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "1. Adding arc (" << node << "," <<
            // neighbour << ")" << std::endl;
            propagatesHead_(graph, neighbour);
          } else if (!_existsDirectedPath_(graph, node, neighbour)
                     && !(_initialMarks_.exists({neighbour, node})
                          && _initialMarks_[{neighbour, node}] == '-')
                     && graph.parents(node).empty()) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(neighbour, node);
            // std::cout << "2. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "2. Adding arc (" << neighbour << "," <<
            // node << ")" << std::endl;
          } else if (!_existsDirectedPath_(graph, node, neighbour)
                     && !(_initialMarks_.exists({neighbour, node})
                          && _initialMarks_[{neighbour, node}] == '-')) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(neighbour, node);
            if (!graph.parents(neighbour).empty() && !graph.parents(node).empty()) {
              _latentCouples_.emplace_back(node, neighbour);
            }

            // std::cout << "3. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "3. Adding arc (" << neighbour << "," <<
            // node << ")" << std::endl;
          } else if (!_existsDirectedPath_(graph, neighbour, node)
                     && !(_initialMarks_.exists({node, neighbour})
                          && _initialMarks_[{node, neighbour}] == '-')) {
            graph.eraseEdge(Edge(node, neighbour));
            graph.addArc(node, neighbour);
            if (!graph.parents(neighbour).empty() && !graph.parents(node).empty()) {
              _latentCouples_.emplace_back(node, neighbour);
            }
            // std::cout << "4. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "4. Adding arc (" << node << "," <<
            // neighbour << ")" << std::endl;
          }
          // else if (!graph.parents(neighbour).empty()
          //&& !graph.parents(node).empty()) {
          // graph.eraseEdge(Edge(neighbour, node));
          // graph.addArc(node, neighbour);
          // __latent_couples.emplace_back(node, neighbour);
          //}
          else {
            graph.eraseEdge(Edge(neighbour, node));
            // std::cout << "5. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl;
          }
        }
      }
    }

    /// get the list of arcs hiding latent variables
    const std::vector< Arc > Miic::latentVariables() const { return _latentCouples_; }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > Miic::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                         PARAM_ESTIMATOR&        estimator,
                                         DAG                     initial_dag) {
      return DAG2BNLearner<>::createBN< GUM_SCALAR >(estimator,
                                                     learnStructure(selector, initial_dag));
    }

    void Miic::setMiicBehaviour() { this->_useMiic_ = true; }

    void Miic::set3of2Behaviour() { this->_useMiic_ = false; }

    void Miic::addConstraints(HashTable< std::pair< NodeId, NodeId >, char > constraints) {
      this->_initialMarks_ = constraints;
    }

    bool Miic::_existsNonTrivialDirectedPath_(const MixedGraph& graph,
                                              const NodeId      n1,
                                              const NodeId      n2) {
      for (const auto parent: graph.parents(n2)) {
        if (graph.existsArc(parent,
                            n2))   // if there is a double arc, pass
          continue;
        if (parent == n1)   // trivial directed path => not recognized
          continue;
        if (_existsDirectedPath_(graph, n1, parent)) return true;
      }
      return false;
    }

    bool Miic::_existsDirectedPath_(const MixedGraph& graph, const NodeId n1, const NodeId n2) {
      // not recursive version => use a FIFO for simulating the recursion
      List< NodeId > nodeFIFO;
      // mark[node] = successor if visited, else mark[node] does not exist
      Set< NodeId > mark;

      mark.insert(n2);
      nodeFIFO.pushBack(n2);

      NodeId current;

      while (!nodeFIFO.empty()) {
        current = nodeFIFO.front();
        nodeFIFO.popFront();

        // check the parents
        for (const auto new_one: graph.parents(current)) {
          if (graph.existsArc(current,
                              new_one))   // if there is a double arc, pass
            continue;

          if (new_one == n1) { return true; }

          if (mark.exists(new_one))   // if this node is already marked, do not
            continue;                 // check it again

          mark.insert(new_one);
          nodeFIFO.pushBack(new_one);
        }
      }

      return false;
    }

    void Miic::_orientingVstructureMiic_(MixedGraph&                                     graph,
                                         HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                         NodeId                                          x,
                                         NodeId                                          y,
                                         NodeId                                          z,
                                         double                                          p1,
                                         double                                          p2) {
      // v-structure discovery
      if (marks[{x, z}] == 'o' && marks[{y, z}] == 'o') {   // If x-z-y
        if (!_existsNonTrivialDirectedPath_(graph, z, x)) {
          graph.eraseEdge(Edge(x, z));
          graph.addArc(x, z);
          // std::cout << "1.a Removing edge (" << x << "," << z << ")" <<
          // std::endl; std::cout << "1.a Adding arc (" << x << "," << z << ")"
          // << std::endl;
          marks[{x, z}] = '>';
          if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
            // std::cout << "Adding latent couple (" << z << "," << x << ")" <<
            // std::endl;
            _latentCouples_.emplace_back(z, x);
          }
          if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
        } else {
          graph.eraseEdge(Edge(x, z));
          // std::cout << "1.b Adding arc (" << x << "," << z << ")" <<
          // std::endl;
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            graph.addArc(z, x);
            // std::cout << "1.b Removing edge (" << x << "," << z << ")" <<
            // std::endl;
            marks[{z, x}] = '>';
          }
        }

        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          graph.eraseEdge(Edge(y, z));
          graph.addArc(y, z);
          // std::cout << "1.c Removing edge (" << y << "," << z << ")" <<
          // std::endl; std::cout << "1.c Adding arc (" << y << "," << z << ")"
          // << std::endl;
          marks[{y, z}] = '>';
          if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
            _latentCouples_.emplace_back(z, y);
          }
          if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
        } else {
          graph.eraseEdge(Edge(y, z));
          // std::cout << "1.d Removing edge (" << y << "," << z << ")" <<
          // std::endl;
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            graph.addArc(z, y);
            // std::cout << "1.d Adding arc (" << z << "," << y << ")" <<
            // std::endl;
            marks[{z, y}] = '>';
          }
        }
      } else if (marks[{x, z}] == '>' && marks[{y, z}] == 'o') {   // If x->z-y
        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          graph.eraseEdge(Edge(y, z));
          graph.addArc(y, z);
          // std::cout << "2.a Removing edge (" << y << "," << z << ")" <<
          // std::endl; std::cout << "2.a Adding arc (" << y << "," << z << ")"
          // << std::endl;
          marks[{y, z}] = '>';
          if (graph.existsArc(z, y)
              && std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(z, y))
                    == _latentCouples_.end()
              && std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(y, z))
                    == _latentCouples_.end()) {
            _latentCouples_.emplace_back(z, y);
          }
          if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
        } else {
          graph.eraseEdge(Edge(y, z));
          // std::cout << "2.b Removing edge (" << y << "," << z << ")" <<
          // std::endl;
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            graph.addArc(z, y);
            // std::cout << "2.b Adding arc (" << y << "," << z << ")" <<
            // std::endl;
            marks[{z, y}] = '>';
          }
        }
      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o') {
        if (!_existsNonTrivialDirectedPath_(graph, z, x)) {
          graph.eraseEdge(Edge(x, z));
          graph.addArc(x, z);
          // std::cout << "3.a Removing edge (" << x << "," << z << ")" <<
          // std::endl; std::cout << "3.a Adding arc (" << x << "," << z << ")"
          // << std::endl;
          marks[{x, z}] = '>';
          if (graph.existsArc(z, x)
              && std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(z, x))
                    == _latentCouples_.end()
              && std::find(_latentCouples_.begin(), _latentCouples_.end(), Arc(x, z))
                    == _latentCouples_.end()) {
            _latentCouples_.emplace_back(z, x);
          }
          if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
        } else {
          graph.eraseEdge(Edge(x, z));
          // std::cout << "3.b Removing edge (" << x << "," << z << ")" <<
          // std::endl;
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            graph.addArc(z, x);
            // std::cout << "3.b Adding arc (" << x << "," << z << ")" <<
            // std::endl;
            marks[{z, x}] = '>';
          }
        }
      }
    }


    void Miic::_propagatingOrientationMiic_(MixedGraph&                                     graph,
                                            HashTable< std::pair< NodeId, NodeId >, char >& marks,
                                            NodeId                                          x,
                                            NodeId                                          y,
                                            NodeId                                          z,
                                            double                                          p1,
                                            double                                          p2) {
      // orientation propagation
      if (marks[{x, z}] == '>' && marks[{y, z}] == 'o' && marks[{z, y}] != '-') {
        graph.eraseEdge(Edge(z, y));
        // std::cout << "4. Removing edge (" << z << "," << y << ")" <<
        // std::endl;
        if (!_existsDirectedPath_(graph, y, z) && graph.parents(y).empty()) {
          graph.addArc(z, y);
          // std::cout << "4.a Adding arc (" << z << "," << y << ")" <<
          // std::endl;
          marks[{z, y}] = '>';
          marks[{y, z}] = '-';
          if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
        } else if (!_existsDirectedPath_(graph, z, y) && graph.parents(z).empty()) {
          graph.addArc(y, z);
          // std::cout << "4.b Adding arc (" << y << "," << z << ")" <<
          // std::endl;
          marks[{z, y}] = '-';
          marks[{y, z}] = '>';
          _latentCouples_.emplace_back(y, z);
          if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
        } else if (!_existsDirectedPath_(graph, y, z)) {
          graph.addArc(z, y);
          // std::cout << "4.c Adding arc (" << z << "," << y << ")" <<
          // std::endl;
          marks[{z, y}] = '>';
          marks[{y, z}] = '-';
          if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
        } else if (!_existsDirectedPath_(graph, z, y)) {
          graph.addArc(y, z);
          // std::cout << "4.d Adding arc (" << y << "," << z << ")" <<
          // std::endl;
          _latentCouples_.emplace_back(y, z);
          marks[{z, y}] = '-';
          marks[{y, z}] = '>';
          if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
        }
      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o' && marks[{z, x}] != '-') {
        graph.eraseEdge(Edge(z, x));
        // std::cout << "5. Removing edge (" << z << "," << x << ")" <<
        // std::endl;
        if (!_existsDirectedPath_(graph, x, z) && graph.parents(x).empty()) {
          graph.addArc(z, x);
          // std::cout << "5.a Adding arc (" << z << "," << x << ")" <<
          // std::endl;
          marks[{z, x}] = '>';
          marks[{x, z}] = '-';
          if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
        } else if (!_existsDirectedPath_(graph, z, x) && graph.parents(z).empty()) {
          graph.addArc(x, z);
          // std::cout << "5.b Adding arc (" << x << "," << z << ")" <<
          // std::endl;
          marks[{z, x}] = '-';
          marks[{x, z}] = '>';
          _latentCouples_.emplace_back(x, z);
          if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
        } else if (!_existsDirectedPath_(graph, x, z)) {
          graph.addArc(z, x);
          // std::cout << "5.c Adding arc (" << z << "," << x << ")" <<
          // std::endl;
          marks[{z, x}] = '>';
          marks[{x, z}] = '-';
          if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
        } else if (!_existsDirectedPath_(graph, z, x)) {
          graph.addArc(x, z);
          // std::cout << "5.d Adding arc (" << x << "," << z << ")" <<
          // std::endl;
          marks[{z, x}] = '-';
          marks[{x, z}] = '>';
          _latentCouples_.emplace_back(x, z);
          if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
        }
      }
    }

    bool Miic::_isNotLatentCouple_(const NodeId x, const NodeId y) {
      const auto& lbeg = _latentCouples_.begin();
      const auto& lend = _latentCouples_.end();

      return (std::find(lbeg, lend, Arc(x, y)) == lend)
          && (std::find(lbeg, lend, Arc(y, x)) == lend);
    }
  } /* namespace learning */

} /* namespace gum */

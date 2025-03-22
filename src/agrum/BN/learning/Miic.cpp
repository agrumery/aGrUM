/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/** @file
 * @brief Implementation of gum::learning::Constraint MIIC
 *
 * @author Quentin FALCAND, Marvin LASSERRE and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/base/core/hashTable.h>
#include <agrum/base/core/heap.h>
#include <agrum/base/core/timer.h>
#include <agrum/base/graphs/mixedGraph.h>
#include <agrum/BN/learning/correctedMutualInformation.h>
#include <agrum/BN/learning/Miic.h>
#include <agrum/BN/learning/paramUtils/DAG2BNLearner.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum {

  namespace learning {

    /// default constructor
    Miic::Miic() : _maxLog_(100), _size_(0) { GUM_CONSTRUCTOR(Miic); }

    /// default constructor with maxLog
    Miic::Miic(int maxLog) : _maxLog_(maxLog), _size_(0) { GUM_CONSTRUCTOR(Miic); }

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

    /// learns the skeleton of a Graph from a fully connected graph
    MixedGraph Miic::learnSkeleton(CorrectedMutualInformation& mutualInformation,
                                   MixedGraph                  graph) {
      // GUM_TRACE_VAR(mutualInformation.score(gum::NodeId (5), gum::NodeId (6)) );

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

      return graph;
    }

    MixedGraph Miic::learnMixedStructure(CorrectedMutualInformation& mutualInformation,
                                         MixedGraph                  graph) {
      // GUM_TRACE_VAR(mutualInformation.score(gum::NodeId (5), gum::NodeId (6)) );

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

      orientationMiic_(mutualInformation, graph, sep_set);
      // Propagates existing orientations thanks to Meek rules

      return meekRules_.propagate(graph);
      // todo : check meekRules_.choices()
    }

    /// learns the structure of a PDAG from à MixedGraph.
    /// It returns a MixedGraph with the constraints of a PDAG, to avoid changing the dependencies
    /// in the other methods of the MIIC class.

    PDAG Miic::learnPDAG(CorrectedMutualInformation& I, MixedGraph initialGraph) {
      return meekRules_.propagateToCPDAG(learnMixedStructure(I, initialGraph));
      // @todo: check the meekRules.choices()
    }

    DAG Miic::learnStructure(CorrectedMutualInformation& I, MixedGraph initialGraph) {
      return meekRules_.propagateToDAG(learnMixedStructure(I, initialGraph));
      // @todo: check the meekRules.choices()
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR, typename GRAPH_CHANGES_SELECTOR, typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > Miic::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                         PARAM_ESTIMATOR&        estimator,
                                         DAG                     initial_dag) {
      return DAG2BNLearner::createBN< GUM_SCALAR >(estimator,
                                                   learnStructure(selector, initial_dag));
    }

    /*
     * PHASE 1 : INITIATION
     *
     * We go over all edges and test if the variables are independent. If they
     * are,
     * the edge is deleted. If not, the best contributor is found.
     */
    void Miic::initiation_(CorrectedMutualInformation& mutualInformation,
                           MixedGraph&                 graph,
                           HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                           Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      NodeId  x, y;
      EdgeSet edges      = graph.edges();
      Size    steps_init = edges.size();

      for (const Edge& edge: edges) {
        x = edge.first();
        y = edge.second();
        if (isForbiddenEdge_(x, y)) {   // Erase Forbidden edges
          GUM_SL_EMIT(x, y, "Remove " << x << " - " << y, " Constraints : Forbidden edge")
          graph.eraseEdge(edge);
        }

        else {
          double Ixy = mutualInformation.score(x, y);

          if (Ixy <= 0) {   //< K
            graph.eraseEdge(edge);
            GUM_SL_EMIT(x,
                        y,
                        "Remove " << x << " - " << y,
                        "Independent based on Mutual Information :" << Ixy)

            sepSet.insert(std::make_pair(x, y), _emptySet_);
          } else {
            findBestContributor_(x, y, _emptySet_, graph, mutualInformation, rank);
            GUM_SL_EMIT(x,
                        y,
                        "Keep " << x << " - " << y,
                        "Dependent based on Mutual Information :" << Ixy)
          }

          ++current_step_;
          if (onProgress.hasListener()) {
            GUM_EMIT3(onProgress, (current_step_ * 33) / steps_init, 0., timer_.step());
          }
        }
      }
    }

    /*
     * PHASE 2 : ITERATION
     *
     * As long as we find important nodes for edges, we go over them to see if
     * we can assess the independence of the variables.
     */
    void Miic::iteration_(CorrectedMutualInformation& mutualInformation,
                          MixedGraph&                 graph,
                          HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet,
                          Heap< CondRanking, GreaterPairOn2nd >&                           rank) {
      // if no triples to further examine pass
      CondRanking best;
      Size        steps_init = current_step_;
      Size        steps_iter = rank.size();

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
            GUM_SL_EMIT(x,
                        y,
                        "Remove " << x << " - " << y,
                        "Independent based on MutualInformation knowing Sep "
                            << ui << "Mutual information:" << i_xy_ui)

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

    /// The orientation protocol of MIIC
    void Miic::orientationMiic_(
        CorrectedMutualInformation&                                            mutualInformation,
        MixedGraph&                                                            graph,
        const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sepSet) {
      // structure to store the orientations marks -, o, or >,
      // Considers the head of the arc/edge: first node -* second node
      HashTable< std::pair< NodeId, NodeId >, char > marks = _initialMarks_;

      // marks always correspond to the head of the arc/edge. - is for a forbidden
      // arc, > for a mandatory arc
      // we start @by adding the mandatory arcs


      /* for (auto& iter = mandaArcs.begin(); iter != mandaArcs.end(); ++iter) {
      //   if (graph.existsEdge(iter.value_type().head(), iter.tail()) &&
      //   _isMandatoryArc_(iter.head(), iter.tail())) {
      //     graph.eraseEdge(Edge(iter.head(), iter.tail()));
      //     graph.addArc(iter.head(), iter.tail());
      //   }
       }*/

      for (auto& arc: _mandatoryGraph_.arcs()) {
        // Check if the edge exists in graph
        if (graph.existsEdge(arc.head(), arc.tail())) {
          graph.eraseEdge(Edge(arc.head(), arc.tail()));
          GUM_SL_EMIT(arc.tail(),
                      arc.head(),
                      "Add Arc" << arc.tail() << "->" << arc.head(),
                      "Mandatory")
          graph.addArc(arc.tail(), arc.head());
          marks.insert({arc.tail(), arc.head()}, '>');
          marks.insert({arc.head(), arc.tail()}, '-');
        } else {
          // If the edge doesn't exist we add the arc as is to graph
          graph.addArc(arc.tail(), arc.head());
          marks.insert({arc.tail(), arc.head()}, '>');
          marks.insert({arc.head(), arc.tail()}, '-');
        }
      }

      for (Arc arc: _forbiddenGraph_.arcs()) {
        // Check if the edge exists in graph
        if (graph.existsEdge(Edge(arc.tail(), arc.head()))) {
          graph.eraseEdge(Edge(arc.tail(), arc.head()));

          // Arc is forced in the other direction
          graph.addArc(arc.head(), arc.tail());
          GUM_SL_EMIT(arc.head(),
                      arc.tail(),
                      "Add Arc" << arc.head() << "->" << arc.tail(),
                      "Forbidden in the other orientation")
          marks.insert({arc.tail(), arc.head()}, '-');
          marks.insert({arc.head(), arc.tail()}, '>');
        }
      }

      std::vector< ProbabilisticRanking > proba_triples
          = unshieldedTriplesMiic_(graph, mutualInformation, sepSet, marks);

      const Size steps_orient = proba_triples.size();
      Size       past_steps   = current_step_;

      ProbabilisticRanking best;
      if (steps_orient > 0) { best = proba_triples[0]; }

      while (!proba_triples.empty() && std::max(std::get< 2 >(best), std::get< 3 >(best)) >= 0.5) {
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
          if (isArcValid_(graph, x, z)) {
            graph.eraseEdge(Edge(x, z));
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "V-structure Orientation")
            // GUM_TRACE("1.a Removing edge (" << x << "," << z << ")")
            // GUM_TRACE("1.a Adding arc (" << x << "," << z << ")")

            marks[{x, z}] = '>';
            if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
              // GUM_TRACE("Adding latent couple (" << z << "," << x << ")")
              _latentCouples_.emplace_back(z, x);
            }
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }

        } else {
          graph.eraseEdge(Edge(x, z));
          // GUM_TRACE("1.b Adding arc (" << x << "," << z << ")")
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            if (isArcValid_(graph, z, x)) {
              graph.addArc(z, x);
              GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "V-structure Orientation")
              // GUM_TRACE("1.b Removing edge (" << x << "," << z << ")")
              marks[{z, x}] = '>';
            }
          }
        }

        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.eraseEdge(Edge(y, z));
            graph.addArc(y, z);
            GUM_SL_EMIT(y, z, "Add Arc " << y << " -> " << z, "V-structure Orientation")
            // GUM_TRACE("1.c Removing edge (" << y << "," << z << ")")
            // GUM_TRACE("1.c Adding arc (" << y << "," << z << ")")
            marks[{y, z}] = '>';
            if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
              _latentCouples_.emplace_back(z, y);
            }
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        } else {
          graph.eraseEdge(Edge(y, z));
          // GUM_TRACE("1.d Removing edge (" << y << "," << z << ")")
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            if (isArcValid_(graph, z, y)) {
              graph.addArc(z, y);
              GUM_SL_EMIT(z, y, "Add Arc " << z << " -> " << y, "V-structure Orientation")
              // GUM_TRACE("1.d Adding arc (" << z << "," << y << ")")
              marks[{z, y}] = '>';
            }
          }
        }
      } else if (marks[{x, z}] == '>' && marks[{y, z}] == 'o') {   // If x->z-y
        if (!_existsNonTrivialDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.eraseEdge(Edge(y, z));
            graph.addArc(y, z);
            GUM_SL_EMIT(y,
                        z,
                        "Add Arc " << y << " -> " << z,
                        "V-structure Orientation | existing "
                            << x << " -> " << z << ", then orienting " << y << " -> " << z)
            // GUM_TRACE("2.a Removing edge (" << y << "," << z << ")")
            // GUM_TRACE("2.a Adding arc (" << y << "," << z << ")")
            marks[{y, z}] = '>';
            if (graph.existsArc(z, y) && _isNotLatentCouple_(z, y)) {
              _latentCouples_.emplace_back(z, y);
            }
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        } else {
          graph.eraseEdge(Edge(y, z));
          // GUM_TRACE("2.b Removing edge (" << y << "," << z << ")")
          if (!_existsNonTrivialDirectedPath_(graph, y, z)) {
            if (isArcValid_(graph, z, y)) {
              graph.addArc(z, y);
              GUM_SL_EMIT(z,
                          y,
                          "Add Arc " << z << " -> " << y,
                          "V-structure Orientation | existing "
                              << x << " -> " << z << ", then orienting " << z << " -> " << y)
              // GUM_TRACE("2.b Adding arc (" << y << "," << z << ")")
              marks[{z, y}] = '>';
            }
          }
        }

      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o') {   // If y->z-x
        if (!_existsNonTrivialDirectedPath_(graph, z, x)) {
          if (isArcValid_(graph, x, z)) {
            graph.eraseEdge(Edge(x, z));
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "V-structure Orientation")
            // GUM_TRACE("3.a Removing edge (" << x << "," << z << ")")
            // GUM_TRACE("3.a Adding arc (" << x << "," << z << ")")
            marks[{x, z}] = '>';
            if (graph.existsArc(z, x) && _isNotLatentCouple_(z, x)) {
              _latentCouples_.emplace_back(z, x);
            }
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }

        } else {
          graph.eraseEdge(Edge(x, z));
          // GUM_TRACE("3.b Removing edge (" << x << "," << z << ")")
          if (!_existsNonTrivialDirectedPath_(graph, x, z)) {
            if (isArcValid_(graph, z, x)) {
              graph.addArc(z, x);
              GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "V-structure Orientation")
              // GUM_TRACE("3.b Adding arc (" << x << "," << z << ")")
              marks[{z, x}] = '>';
            }
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
        if (!_existsDirectedPath_(graph, y, z) && graph.parents(y).empty()) {
          if (isArcValid_(graph, z, y)) {
            graph.addArc(z, y);
            GUM_SL_EMIT(z,
                        y,
                        "Add Arc " << z << " -> " << y,
                        "Propagation MIIC (919) | existing x -> " << z << " and " << z << " - "
                                                                  << y)
            // GUM_TRACE("4.a Adding arc (" << z << "," << y << ")")
            marks[{z, y}] = '>';
            marks[{y, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
          }

        } else if (!_existsDirectedPath_(graph, z, y) && graph.parents(z).empty()) {
          if (isArcValid_(graph, y, z)) {
            graph.addArc(y, z);
            GUM_SL_EMIT(y, z, "Add Arc " << y << " -> " << z, "Propagation MIIC line 932 ")
            // GUM_TRACE("4.b Adding arc (" << y << "," << z << ")")

            marks[{z, y}] = '-';
            marks[{y, z}] = '>';
            _latentCouples_.emplace_back(y, z);
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }

        } else if (!_existsDirectedPath_(graph, y, z)) {
          if (isArcValid_(graph, z, y)) {
            graph.addArc(z, y);
            GUM_SL_EMIT(z, y, "Add Arc " << z << "->" << y, "Propagation MIIC 947 ")
            // GUM_TRACE("4.c Adding arc (" << z << "," << y << ")")
            marks[{z, y}] = '>';
            marks[{y, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, y))) _arcProbas_.insert(Arc(z, y), p2);
          }

        } else if (!_existsDirectedPath_(graph, z, y)) {
          if (isArcValid_(graph, y, z)) {
            graph.addArc(y, z);
            GUM_SL_EMIT(z, y, "Add Arc " << z << "->" << y, "Propagation MIIC 959")
            // GUM_TRACE("4.d Adding arc (" << y << "," << z << ")")

            _latentCouples_.emplace_back(y, z);
            marks[{z, y}] = '-';
            marks[{y, z}] = '>';
            if (!_arcProbas_.exists(Arc(y, z))) _arcProbas_.insert(Arc(y, z), p2);
          }
        }
      } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o' && marks[{z, x}] != '-') {
        graph.eraseEdge(Edge(z, x));
        // GUM_TRACE("5. Removing edge (" << z << "," << x << ")")
        if (!_existsDirectedPath_(graph, x, z) && graph.parents(x).empty()) {
          if (isArcValid_(graph, z, x)) {
            graph.addArc(z, x);
            GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "Propagation MIIC 977")
            // GUM_TRACE("5.a Adding arc (" << z << "," << x << ")")
            marks[{z, x}] = '>';
            marks[{x, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
          }

        } else if (!_existsDirectedPath_(graph, z, x) && graph.parents(z).empty()) {
          if (isArcValid_(graph, x, z)) {
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << "->" << z, "Propagation MIIC 990")
            // GUM_TRACE("5.b Adding arc (" << x << "," << z << ")")
            marks[{z, x}] = '-';
            marks[{x, z}] = '>';
            _latentCouples_.emplace_back(x, z);
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }

        } else if (!_existsDirectedPath_(graph, x, z)) {
          if (isArcValid_(graph, z, x)) {
            graph.addArc(z, x);
            GUM_SL_EMIT(z, x, "Add Arc " << z << " -> " << x, "Propagation MIIC 1004")
            // GUM_TRACE("5.c Adding arc (" << z << "," << x << ")")
            marks[{z, x}] = '>';
            marks[{x, z}] = '-';
            if (!_arcProbas_.exists(Arc(z, x))) _arcProbas_.insert(Arc(z, x), p1);
          }
        } else if (!_existsDirectedPath_(graph, z, x)) {
          if (isArcValid_(graph, x, z)) {
            graph.addArc(x, z);
            GUM_SL_EMIT(x, z, "Add Arc " << x << " -> " << z, "Propagation MIIC 1016")
            // GUM_TRACE("5.d Adding arc (" << x << "," << z << ")")
            marks[{z, x}] = '-';
            marks[{x, z}] = '>';
            _latentCouples_.emplace_back(x, z);
            if (!_arcProbas_.exists(Arc(x, z))) _arcProbas_.insert(Arc(x, z), p1);
          }
        }
      }
    }

    void Miic::orientDoubleHeadedArcs_(MixedGraph& mg) {
      gum::ArcSet L;   // create a set of all double-headed arcs
      for (gum::NodeId x: mg.nodes())
        for (NodeId y: mg.parents(x))
          // If there is a mutual parent-child relationship, add the arc to the set
          if (mg.parents(y).contains(x)) {
            if (x > y) {
              continue;   // Avoid duplicate arcs by considering only one direction
            } else {
              L.insert(gum::Arc(x, y));
            }
          }

      // If there are double-headed arcs
      if (not L.empty()) {
        while (true) {
          bool withdrawFlag_L = false;
          for (auto arc: ArcSet(L)) {
            bool tail_head        = _existsDirectedPath_(mg, arc.tail(), arc.head());
            bool head_tail        = _existsDirectedPath_(mg, arc.head(), arc.tail());
            bool withdrawFlag_arc = false;

            // Case 1: There is already a path from tail to head and no path from head to tail
            if (tail_head && !head_tail) {
              // Erase the arc from head to tail to avoid cycles
              mg.eraseArc(Arc(arc.head(), arc.tail()));
              withdrawFlag_arc = true;

              // Case 2: There is already a path from head to tail and no path from tail to head
            } else if (!tail_head && head_tail) {
              // Erase the arc from tail to head to avoid cycles
              mg.eraseArc(Arc(arc.tail(), arc.head()));
              withdrawFlag_arc = true;

              // Case 3: There is no path between tail and head
            } else if (!tail_head && !head_tail) {
              // Choose an arbitrary orientation and erase the corresponding arc
              mg.eraseArc(Arc(arc.head(), arc.tail()));
              withdrawFlag_arc = true;
            }

            // Remove the arc from the set if it was processed
            if (withdrawFlag_arc) {
              L.erase(arc);
              withdrawFlag_L = true;
            }
          }
          // If all double-headed arcs are processed, exit the loop
          if (L.empty()) { break; }

          // If no arcs were withdrawn, erase an arbitrary double arc in the graph (the first one in
          // L). Hoping the situation will improve. ┐(￣ヘ￣)┌ If we arrive here, it's because the
          // double-headed arc creates cycles in both directions
          if (!withdrawFlag_L) {
            auto it  = L.begin();
            auto arc = *it;
            mg.eraseArc(Arc(arc.head(), arc.tail()));
            mg.eraseArc(Arc(arc.tail(), arc.head()));
            L.erase(arc);
          }
        }
      }
    }

    /// finds the best contributor node for a pair given a conditioning set
    void Miic::findBestContributor_(NodeId                                 x,
                                    NodeId                                 y,
                                    const std::vector< NodeId >&           ui,
                                    const MixedGraph&                      graph,
                                    CorrectedMutualInformation&            mutualInformation,
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
          // if exponential are too high or to low, crop them at _maxLog_
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

          // if exponential are too high or to low, crop them at  _maxLog_
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
      }   // for z in graph.nodes
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
        CorrectedMutualInformation&                                            mutualInformation,
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
        CorrectedMutualInformation&                                            mutualInformation,
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
          // distinguish between the initialization and the update process
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

    /// get the list of arcs hiding latent variables
    const std::vector< Arc > Miic::latentVariables() const { return _latentCouples_; }

    void Miic::addConstraints(HashTable< std::pair< NodeId, NodeId >, char > constraints) {
      this->_initialMarks_ = constraints;
    }

    bool Miic::_existsNonTrivialDirectedPath_(const MixedGraph& graph,
                                              const NodeId      n1,
                                              const NodeId      n2) {
      for (const auto parent: graph.parents(n2)) {
        if (graph.existsArc(n2, parent))   // if there is a double arc, pass
          continue;
        if (parent == n1)                  // trivial directed path => not recognized
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

    bool Miic::_isNotLatentCouple_(const NodeId x, const NodeId y) {
      const auto& lbeg = _latentCouples_.begin();
      const auto& lend = _latentCouples_.end();

      return (std::find(lbeg, lend, Arc(x, y)) == lend)
          && (std::find(lbeg, lend, Arc(y, x)) == lend);
    }

    void Miic::setMandatoryGraph(const gum::DAG mandaGraph) { this->_mandatoryGraph_ = mandaGraph; }

    void Miic::setForbiddenGraph(const gum::DiGraph forbidGraph) {
      this->_forbiddenGraph_ = forbidGraph;
    }

    void Miic::setMaxIndegree(gum::Size n) { this->_maxIndegree_ = n; }

    bool Miic::isMaxIndegree_(const MixedGraph graph, NodeId x) {
      return ((graph.parents(x).size() >= _maxIndegree_));
    }

    bool Miic::isForbiddenArc_(NodeId x, NodeId y) const {
      return (_forbiddenGraph_.existsArc(x, y));
    }

    bool Miic::isArcValid_(const MixedGraph graph, NodeId x, NodeId y) {
      return (isForbiddenArc_(x, y) == false && isMaxIndegree_(graph, y) == false);
    }

    bool Miic::isForbiddenEdge_(NodeId x, NodeId y) {
      return (_forbiddenGraph_.existsArc(x, y) && _forbiddenGraph_.existsArc(y, x));
    }

  } /* namespace learning */

} /* namespace gum */

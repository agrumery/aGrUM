/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief Implementation of gum::learning::ThreeOffTwo
 *
 * @author Quentin FALCAND and Pierre-Henri WUILLEMIN(@LIP6)
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
    Miic::Miic() {
      GUM_CONSTRUCTOR(Miic);
      ;
    }

    /// default constructor with maxLog
    Miic::Miic(int maxLog) : maxLog__(maxLog) {
      GUM_CONSTRUCTOR(Miic);
      ;
    }

    /// copy constructor
    Miic::Miic(const Miic& from) : ApproximationScheme(from) {
      GUM_CONS_CPY(Miic);
    }

    /// move constructor
    Miic::Miic(Miic&& from) : ApproximationScheme(std::move(from)) {
      GUM_CONS_MOV(Miic);
    }

    /// destructor
    Miic::~Miic() {
      GUM_DESTRUCTOR(Miic);
      ;
    }

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


    bool GreaterPairOn2nd::operator()(
       const std::pair<
          std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
          double >& e1,
       const std::pair<
          std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
          double >& e2) const {
      return e1.second > e2.second;
    }

    bool GreaterAbsPairOn2nd::operator()(
       const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e1,
       const std::pair< std::tuple< NodeId, NodeId, NodeId >*, double >& e2)
       const {
      return std::abs(e1.second) > std::abs(e2.second);
    }

    bool GreaterTupleOnLast::operator()(
       const std::
          tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double >&
             e1,
       const std::
          tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double >&
             e2) const {
      double p1xz = std::get< 2 >(e1);
      double p1yz = std::get< 3 >(e1);
      double p2xz = std::get< 2 >(e2);
      double p2yz = std::get< 3 >(e2);
      double I1   = std::get< 1 >(e1);
      double I2   = std::get< 1 >(e2);
      // First, we look at the sign of information.
      // Then, the probility values
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
    MixedGraph Miic::learnMixedStructure(CorrectedMutualInformation<>& I,
                                         MixedGraph                    graph) {
      timer_.reset();
      current_step_ = 0;

      // clear the vector of latent arcs to be sure
      latent_couples__.clear();

      /// the heap of ranks, with the score, and the NodeIds of x, y and z.
      Heap<
         std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                    double >,
         GreaterPairOn2nd >
         rank_;

      /// the variables separation sets
      HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > > sep_set;

      initiation_(I, graph, sep_set, rank_);

      iteration_(I, graph, sep_set, rank_);

      // std::cout << "Le graphe contient: " << graph.sizeEdges() << " edges." <<
      // std::endl; std::cout << "En voici la liste: " << graph.edges() <<
      // std::endl;

      if (usemiic__) {
        orientation_miic_(I, graph, sep_set);
      } else {
        orientation_3off2_(I, graph, sep_set);
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
    void Miic::initiation_(
       CorrectedMutualInformation<>&                                    I,
       MixedGraph&                                                      graph,
       HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sep_set,
       Heap<
          std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                     double >,
          GreaterPairOn2nd >& rank_) {
      NodeId  x, y;
      EdgeSet edges      = graph.edges();
      Size    steps_init = edges.size();

      for (const Edge& edge: edges) {
        x          = edge.first();
        y          = edge.second();
        double Ixy = I.score(x, y);

        if (Ixy <= 0) {   //< K
          graph.eraseEdge(edge);
          sep_set.insert(std::make_pair(x, y), empty_set__);
        } else {
          findBestContributor_(x, y, empty_set__, graph, I, rank_);
        }

        ++current_step_;
        if (onProgress.hasListener()) {
          GUM_EMIT3(onProgress,
                    (current_step_ * 33) / steps_init,
                    0.,
                    timer_.step());
        }
      }
    }

    /*
     * PHASE 2 : ITERATION
     *
     * As long as we find important nodes for edges, we go over them to see if
     * we can assess the independence of the variables.
     */
    void Miic::iteration_(
       CorrectedMutualInformation<>&                                    I,
       MixedGraph&                                                      graph,
       HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >& sep_set,
       Heap<
          std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                     double >,
          GreaterPairOn2nd >& rank_) {
      // if no triples to further examine pass
      std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                 double >
         best;

      Size steps_init = current_step_;
      Size steps_iter = rank_.size();

      try {
        while (rank_.top().second > 0.5) {
          best = rank_.pop();

          const NodeId          x  = std::get< 0 >(*(best.first));
          const NodeId          y  = std::get< 1 >(*(best.first));
          const NodeId          z  = std::get< 2 >(*(best.first));
          std::vector< NodeId > ui = std::move(std::get< 3 >(*(best.first)));

          ui.push_back(z);
          const double Ixy_ui = I.score(x, y, ui);
          if (Ixy_ui < 0) {
            graph.eraseEdge(Edge(x, y));
            sep_set.insert(std::make_pair(x, y), std::move(ui));
          } else {
            findBestContributor_(x, y, ui, graph, I, rank_);
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
      if (onProgress.hasListener()) {
        GUM_EMIT3(onProgress, 66, 0., timer_.step());
      }
      current_step_ = steps_init + steps_iter;
    }

    /*
     * PHASE 3 : ORIENTATION
     *
     * Try to assess v-structures and propagate them.
     */
    void Miic::orientation_3off2_(
       CorrectedMutualInformation<>& I,
       MixedGraph&                   graph,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >&
          sep_set) {
      std::vector< std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > >
           triples      = getUnshieldedTriples_(graph, I, sep_set);
      Size steps_orient = triples.size();
      Size past_steps   = current_step_;

      // marks always correspond to the head of the arc/edge. - is for a forbidden
      // arc, > for a mandatory arc
      // we start by adding the mandatory arcs
      for (auto iter = initial_marks__.begin(); iter != initial_marks__.end();
           ++iter) {
        if (graph.existsEdge(iter.key().first, iter.key().second)
            && iter.val() == '>') {
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
        std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > triple
           = triples[i];
        NodeId x, y, z;
        x = std::get< 0 >(*triple.first);
        y = std::get< 1 >(*triple.first);
        z = std::get< 2 >(*triple.first);

        std::vector< NodeId >       ui;
        std::pair< NodeId, NodeId > key     = {x, y};
        std::pair< NodeId, NodeId > rev_key = {y, x};
        if (sep_set.exists(key)) {
          ui = sep_set[key];
        } else if (sep_set.exists(rev_key)) {
          ui = sep_set[rev_key];
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
              if (!existsDirectedPath__(graph, z, x)
                  && !(initial_marks__.exists({x, z})
                       && initial_marks__[{x, z}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(x, z));
                graph.addArc(x, z);
              } else if (existsDirectedPath__(graph, z, x)
                         && !(initial_marks__.exists({z, x})
                              && initial_marks__[{z, x}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(x, z));
                // if we find a cycle, we force the competing edge
                graph.addArc(z, x);
                if (std::find(latent_couples__.begin(),
                              latent_couples__.end(),
                              Arc(z, x))
                    == latent_couples__.end()) {
                  latent_couples__.push_back(Arc(z, x));
                }
              }
            } else if (!graph.existsArc(y, z) && !graph.existsArc(z, y)) {
              if (!existsDirectedPath__(graph, z, y)
                  && !(initial_marks__.exists({x, z})
                       && initial_marks__[{x, z}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(y, z));
                graph.addArc(y, z);
              } else if (existsDirectedPath__(graph, z, y)
                         && !(initial_marks__.exists({z, y})
                              && initial_marks__[{z, y}] == '-')) {
                reset = true;
                graph.eraseEdge(Edge(y, z));
                // if we find a cycle, we force the competing edge
                graph.addArc(z, y);
                if (std::find(latent_couples__.begin(),
                              latent_couples__.end(),
                              Arc(z, y))
                    == latent_couples__.end()) {
                  latent_couples__.push_back(Arc(z, y));
                }
              }
            } else {
              // checking if the anti-directed arc already exists, to register a
              // latent variable
              if (graph.existsArc(z, x)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, x))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(x, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, x));
              }
              if (graph.existsArc(z, y)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, y))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(y, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, y));
              }
            }
          }
        } else {   // try Rule 1
          if (graph.existsArc(x, z) && !graph.existsArc(z, y)
              && !graph.existsArc(y, z)) {
            if (!existsDirectedPath__(graph, y, z)
                && !(initial_marks__.exists({z, y})
                     && initial_marks__[{z, y}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, y));
              graph.addArc(z, y);
            } else if (existsDirectedPath__(graph, y, z)
                       && !(initial_marks__.exists({y, z})
                            && initial_marks__[{y, z}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, y));
              // if we find a cycle, we force the competing edge
              graph.addArc(y, z);
              if (std::find(latent_couples__.begin(),
                            latent_couples__.end(),
                            Arc(y, z))
                  == latent_couples__.end()) {
                latent_couples__.push_back(Arc(y, z));
              }
            }
          }
          if (graph.existsArc(y, z) && !graph.existsArc(z, x)
              && !graph.existsArc(x, z)) {
            if (!existsDirectedPath__(graph, x, z)
                && !(initial_marks__.exists({z, x})
                     && initial_marks__[{z, x}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, x));
              graph.addArc(z, x);
            } else if (existsDirectedPath__(graph, x, z)
                       && !(initial_marks__.exists({x, z})
                            && initial_marks__[{x, z}] == '-')) {
              reset = true;
              graph.eraseEdge(Edge(z, x));
              // if we find a cycle, we force the competing edge
              graph.addArc(x, z);
              if (std::find(latent_couples__.begin(),
                            latent_couples__.end(),
                            Arc(x, z))
                  == latent_couples__.end()) {
                latent_couples__.push_back(Arc(x, z));
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
      for (const Arc& arc: latent_couples__) {
        graph.eraseArc(Arc(arc.head(), arc.tail()));
      }
    }

    /// varient trying to propagate both orientations in a bidirected arc
    void Miic::orientation_latents_(
       CorrectedMutualInformation<>& I,
       MixedGraph&                   graph,
       const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >&
          sep_set) {
      std::vector< std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > >
           triples      = getUnshieldedTriples_(graph, I, sep_set);
      Size steps_orient = triples.size();
      Size past_steps   = current_step_;

      NodeId i = 0;
      // list of elements that we shouldnt read again, ie elements that are
      // eligible to
      // rule 0 after the first time they are tested, and elements on which rule 1
      // has been applied
      while (i < triples.size()) {
        // if i not in do_not_reread
        std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > triple
           = triples[i];
        NodeId x, y, z;
        x = std::get< 0 >(*triple.first);
        y = std::get< 1 >(*triple.first);
        z = std::get< 2 >(*triple.first);

        std::vector< NodeId >       ui;
        std::pair< NodeId, NodeId > key     = {x, y};
        std::pair< NodeId, NodeId > rev_key = {y, x};
        if (sep_set.exists(key)) {
          ui = sep_set[key];
        } else if (sep_set.exists(rev_key)) {
          ui = sep_set[rev_key];
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
                  latent_couples__.push_back(Arc(z, x));
                } catch (gum::NotFound) { graph.addArc(x, z); }
                graph.addArc(z, x);
              } else {
                try {
                  std::vector< NodeId > path = graph.directedPath(z, x);
                  // if we find a cycle, we force the competing edge
                  graph.addArc(z, x);
                  latent_couples__.push_back(Arc(z, x));
                } catch (gum::NotFound) { graph.addArc(x, z); }
              }
              if (graph.existsArc(z, y)) {
                graph.eraseArc(Arc(z, y));
                try {
                  std::vector< NodeId > path = graph.directedPath(z, y);
                  // if we find a cycle, we force the competing edge
                  latent_couples__.push_back(Arc(z, y));
                } catch (gum::NotFound) { graph.addArc(y, z); }
                graph.addArc(z, y);
              } else {
                try {
                  std::vector< NodeId > path = graph.directedPath(z, y);
                  // if we find a cycle, we force the competing edge
                  graph.addArc(z, y);
                  latent_couples__.push_back(Arc(z, y));

                } catch (gum::NotFound) { graph.addArc(y, z); }
              }
              if (graph.existsArc(z, x)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, x))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(x, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, x));
              }
              if (graph.existsArc(z, y)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, y))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(y, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, y));
              }
            }
          } else {
            ++i;
          }
        } else {   // try Rule 1
          bool reset{false};
          if (graph.existsArc(x, z) && !graph.existsArc(z, y)
              && !graph.existsArc(y, z)) {
            reset = true;
            graph.eraseEdge(Edge(z, y));
            try {
              std::vector< NodeId > path = graph.directedPath(y, z);
              // if we find a cycle, we force the competing edge
              graph.addArc(y, z);
              latent_couples__.push_back(Arc(y, z));
            } catch (gum::NotFound) { graph.addArc(z, y); }
          }
          if (graph.existsArc(y, z) && !graph.existsArc(z, x)
              && !graph.existsArc(x, z)) {
            reset = true;
            graph.eraseEdge(Edge(z, x));
            try {
              std::vector< NodeId > path = graph.directedPath(x, z);
              // if we find a cycle, we force the competing edge
              graph.addArc(x, z);
              latent_couples__.push_back(Arc(x, z));
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
      for (const Arc& arc: latent_couples__) {
        graph.eraseArc(Arc(arc.head(), arc.tail()));
      }
    }

    /// varient using the orientation protocol of MIIC
    void
       Miic::orientation_miic_(CorrectedMutualInformation<>&             I,
                               MixedGraph&                               graph,
                               const HashTable< std::pair< NodeId, NodeId >,
                                                std::vector< NodeId > >& sep_set) {
      // structure to store the orientations marks -, o, or >,
      // Considers the head of the arc/edge first node -* second node
      HashTable< std::pair< NodeId, NodeId >, char > marks = initial_marks__;

      // marks always correspond to the head of the arc/edge. - is for a forbidden
      // arc, > for a mandatory arc
      // we start by adding the mandatory arcs
      for (auto iter = marks.begin(); iter != marks.end(); ++iter) {
        if (graph.existsEdge(iter.key().first, iter.key().second)
            && iter.val() == '>') {
          graph.eraseEdge(Edge(iter.key().first, iter.key().second));
          graph.addArc(iter.key().first, iter.key().second);
        }
      }

      std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                               double,
                               double,
                               double > >
         proba_triples = getUnshieldedTriplesMIIC_(graph, I, sep_set, marks);

      Size steps_orient = proba_triples.size();
      Size past_steps   = current_step_;

      std::tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double >
         best;
      if (steps_orient > 0) { best = proba_triples[0]; }

      while (!proba_triples.empty()
             && std::max(std::get< 2 >(best), std::get< 3 >(best)) > 0.5) {
        NodeId x, y, z;
        x = std::get< 0 >(*std::get< 0 >(best));
        y = std::get< 1 >(*std::get< 0 >(best));
        z = std::get< 2 >(*std::get< 0 >(best));
        // std::cout << "Triple: (" << x << "," << y << "," << z << ")" <<
        // std::endl;

        const double i3 = std::get< 1 >(best);

        if (i3 <= 0) {
          // v-structure discovery
          if (marks[{x, z}] == 'o' && marks[{y, z}] == 'o') {   // If x-z-y
            if (!existsDirectedPath__(graph, z, x, false)) {
              graph.eraseEdge(Edge(x, z));
              graph.addArc(x, z);
              // std::cout << "1.a Removing edge (" << x << "," << z << ")" <<
              // std::endl; std::cout << "1.a Adding arc (" << x << "," << z << ")"
              // << std::endl;
              marks[{x, z}] = '>';
              if (graph.existsArc(z, x)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, x))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(x, z))
                        == latent_couples__.end()) {
                // std::cout << "Adding latent couple (" << z << "," << x << ")" <<
                // std::endl;
                latent_couples__.push_back(Arc(z, x));
              }
              if (!arc_probas__.exists(Arc(x, z)))
                arc_probas__.insert(Arc(x, z), std::get< 2 >(best));
            } else {
              graph.eraseEdge(Edge(x, z));
              // std::cout << "1.b Adding arc (" << x << "," << z << ")" <<
              // std::endl;
              if (!existsDirectedPath__(graph, x, z, false)) {
                graph.addArc(z, x);
                // std::cout << "1.b Removing edge (" << x << "," << z << ")" <<
                // std::endl;
                marks[{z, x}] = '>';
              }
            }

            if (!existsDirectedPath__(graph, z, y, false)) {
              graph.eraseEdge(Edge(y, z));
              graph.addArc(y, z);
              // std::cout << "1.c Removing edge (" << y << "," << z << ")" <<
              // std::endl; std::cout << "1.c Adding arc (" << y << "," << z << ")"
              // << std::endl;
              marks[{y, z}] = '>';
              if (graph.existsArc(z, y)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, y))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(y, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, y));
              }
              if (!arc_probas__.exists(Arc(y, z)))
                arc_probas__.insert(Arc(y, z), std::get< 3 >(best));
            } else {
              graph.eraseEdge(Edge(y, z));
              // std::cout << "1.d Removing edge (" << y << "," << z << ")" <<
              // std::endl;
              if (!existsDirectedPath__(graph, y, z, false)) {
                graph.addArc(z, y);
                // std::cout << "1.d Adding arc (" << z << "," << y << ")" <<
                // std::endl;
                marks[{z, y}] = '>';
              }
            }
          } else if (marks[{x, z}] == '>' && marks[{y, z}] == 'o') {   // If x->z-y
            if (!existsDirectedPath__(graph, z, y, false)) {
              graph.eraseEdge(Edge(y, z));
              graph.addArc(y, z);
              // std::cout << "2.a Removing edge (" << y << "," << z << ")" <<
              // std::endl; std::cout << "2.a Adding arc (" << y << "," << z << ")"
              // << std::endl;
              marks[{y, z}] = '>';
              if (graph.existsArc(z, y)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, y))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(y, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, y));
              }
              if (!arc_probas__.exists(Arc(y, z)))
                arc_probas__.insert(Arc(y, z), std::get< 3 >(best));
            } else {
              graph.eraseEdge(Edge(y, z));
              // std::cout << "2.b Removing edge (" << y << "," << z << ")" <<
              // std::endl;
              if (!existsDirectedPath__(graph, y, z, false)) {
                graph.addArc(z, y);
                // std::cout << "2.b Adding arc (" << y << "," << z << ")" <<
                // std::endl;
                marks[{z, y}] = '>';
              }
            }
          } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o') {
            if (!existsDirectedPath__(graph, z, x, false)) {
              graph.eraseEdge(Edge(x, z));
              graph.addArc(x, z);
              // std::cout << "3.a Removing edge (" << x << "," << z << ")" <<
              // std::endl; std::cout << "3.a Adding arc (" << x << "," << z << ")"
              // << std::endl;
              marks[{x, z}] = '>';
              if (graph.existsArc(z, x)
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(z, x))
                        == latent_couples__.end()
                  && std::find(latent_couples__.begin(),
                               latent_couples__.end(),
                               Arc(x, z))
                        == latent_couples__.end()) {
                latent_couples__.push_back(Arc(z, x));
              }
              if (!arc_probas__.exists(Arc(x, z)))
                arc_probas__.insert(Arc(x, z), std::get< 2 >(best));
            } else {
              graph.eraseEdge(Edge(x, z));
              // std::cout << "3.b Removing edge (" << x << "," << z << ")" <<
              // std::endl;
              if (!existsDirectedPath__(graph, x, z, false)) {
                graph.addArc(z, x);
                // std::cout << "3.b Adding arc (" << x << "," << z << ")" <<
                // std::endl;
                marks[{z, x}] = '>';
              }
            }
          }

        } else {
          // orientation propagation
          if (marks[{x, z}] == '>' && marks[{y, z}] == 'o'
              && marks[{z, y}] != '-') {
            graph.eraseEdge(Edge(z, y));
            // std::cout << "4. Removing edge (" << z << "," << y << ")" <<
            // std::endl;
            if (!existsDirectedPath__(graph, y, z) && graph.parents(y).empty()) {
              graph.addArc(z, y);
              // std::cout << "4.a Adding arc (" << z << "," << y << ")" <<
              // std::endl;
              marks[{z, y}] = '>';
              marks[{y, z}] = '-';
              if (!arc_probas__.exists(Arc(z, y)))
                arc_probas__.insert(Arc(z, y), std::get< 3 >(best));
            } else if (!existsDirectedPath__(graph, z, y)
                       && graph.parents(z).empty()) {
              graph.addArc(y, z);
              // std::cout << "4.b Adding arc (" << y << "," << z << ")" <<
              // std::endl;
              marks[{z, y}] = '-';
              marks[{y, z}] = '>';
              latent_couples__.push_back(Arc(y, z));
              if (!arc_probas__.exists(Arc(y, z)))
                arc_probas__.insert(Arc(y, z), std::get< 3 >(best));
            } else if (!existsDirectedPath__(graph, y, z)) {
              graph.addArc(z, y);
              // std::cout << "4.c Adding arc (" << z << "," << y << ")" <<
              // std::endl;
              marks[{z, y}] = '>';
              marks[{y, z}] = '-';
              if (!arc_probas__.exists(Arc(z, y)))
                arc_probas__.insert(Arc(z, y), std::get< 3 >(best));
            } else if (!existsDirectedPath__(graph, z, y)) {
              graph.addArc(y, z);
              // std::cout << "4.d Adding arc (" << y << "," << z << ")" <<
              // std::endl;
              latent_couples__.push_back(Arc(y, z));
              marks[{z, y}] = '-';
              marks[{y, z}] = '>';
              if (!arc_probas__.exists(Arc(y, z)))
                arc_probas__.insert(Arc(y, z), std::get< 3 >(best));
            }

          } else if (marks[{y, z}] == '>' && marks[{x, z}] == 'o'
                     && marks[{z, x}] != '-') {
            graph.eraseEdge(Edge(z, x));
            // std::cout << "5. Removing edge (" << z << "," << x << ")" <<
            // std::endl;
            if (!existsDirectedPath__(graph, x, z) && graph.parents(x).empty()) {
              graph.addArc(z, x);
              // std::cout << "5.a Adding arc (" << z << "," << x << ")" <<
              // std::endl;
              marks[{z, x}] = '>';
              marks[{x, z}] = '-';
              if (!arc_probas__.exists(Arc(z, x)))
                arc_probas__.insert(Arc(z, x), std::get< 2 >(best));
            } else if (!existsDirectedPath__(graph, z, x)
                       && graph.parents(z).empty()) {
              graph.addArc(x, z);
              // std::cout << "5.b Adding arc (" << x << "," << z << ")" <<
              // std::endl;
              marks[{z, x}] = '-';
              marks[{x, z}] = '>';
              latent_couples__.push_back(Arc(x, z));
              if (!arc_probas__.exists(Arc(x, z)))
                arc_probas__.insert(Arc(x, z), std::get< 2 >(best));
            } else if (!existsDirectedPath__(graph, x, z)) {
              graph.addArc(z, x);
              // std::cout << "5.c Adding arc (" << z << "," << x << ")" <<
              // std::endl;
              marks[{z, x}] = '>';
              marks[{x, z}] = '-';
              if (!arc_probas__.exists(Arc(z, x)))
                arc_probas__.insert(Arc(z, x), std::get< 2 >(best));
            } else if (!existsDirectedPath__(graph, z, x)) {
              graph.addArc(x, z);
              // std::cout << "5.d Adding arc (" << x << "," << z << ")" <<
              // std::endl;
              marks[{z, x}] = '-';
              marks[{x, z}] = '>';
              latent_couples__.push_back(Arc(x, z));
              if (!arc_probas__.exists(Arc(x, z)))
                arc_probas__.insert(Arc(x, z), std::get< 2 >(best));
            }
          }
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
      for (auto iter = latent_couples__.rbegin(); iter != latent_couples__.rend();
           ++iter) {
        graph.eraseArc(Arc(iter->head(), iter->tail()));
        if (existsDirectedPath__(graph, iter->head(), iter->tail())) {
          // if we find a cycle, we force the competing edge
          graph.addArc(iter->head(), iter->tail());
          graph.eraseArc(Arc(iter->tail(), iter->head()));
          *iter = Arc(iter->head(), iter->tail());
        }
      }

      if (onProgress.hasListener()) {
        GUM_EMIT3(onProgress, 100, 0., timer_.step());
      }
    }

    /// finds the best contributor node for a pair given a conditioning set
    void Miic::findBestContributor_(
       NodeId                        x,
       NodeId                        y,
       const std::vector< NodeId >&  ui,
       const MixedGraph&             graph,
       CorrectedMutualInformation<>& I,
       Heap<
          std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                     double >,
          GreaterPairOn2nd >& rank_) {
      double maxP = -1.0;
      NodeId maxZ = 0;

      // compute N
      //__N = I.N();
      const double Ixy_ui = I.score(x, y, ui);

      for (const NodeId z: graph) {
        // if z!=x and z!=y and z not in ui
        if (z != x && z != y && std::find(ui.begin(), ui.end(), z) == ui.end()) {
          double Pnv;
          double Pb;

          // Computing Pnv
          const double Ixyz_ui    = I.score(x, y, z, ui);
          double       calc_expo1 = -Ixyz_ui * M_LN2;
          // if exponentials are too high or to low, crop them at |__maxLog|
          if (calc_expo1 > maxLog__) {
            Pnv = 0.0;
          } else if (calc_expo1 < -maxLog__) {
            Pnv = 1.0;
          } else {
            Pnv = 1 / (1 + std::exp(calc_expo1));
          }

          // Computing Pb
          const double Ixz_ui = I.score(x, z, ui);
          const double Iyz_ui = I.score(y, z, ui);

          calc_expo1        = -(Ixz_ui - Ixy_ui) * M_LN2;
          double calc_expo2 = -(Iyz_ui - Ixy_ui) * M_LN2;

          // if exponentials are too high or to low, crop them at maxLog__
          if (calc_expo1 > maxLog__ || calc_expo2 > maxLog__) {
            Pb = 0.0;
          } else if (calc_expo1 < -maxLog__ && calc_expo2 < -maxLog__) {
            Pb = 1.0;
          } else {
            double expo1, expo2;
            if (calc_expo1 < -maxLog__) {
              expo1 = 0.0;
            } else {
              expo1 = std::exp(calc_expo1);
            }
            if (calc_expo2 < -maxLog__) {
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
      std::pair< std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >*,
                 double >
           final;
      auto tup
         = new std::tuple< NodeId, NodeId, NodeId, std::vector< NodeId > >{x,
                                                                           y,
                                                                           maxZ,
                                                                           ui};
      final.first  = tup;
      final.second = maxP;
      rank_.insert(final);
    }

    /// gets the list of unshielded triples in the graph in decreasing value of
    ///|I'(x, y, z|{ui})|
    std::vector< std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > >
       Miic::getUnshieldedTriples_(
          const MixedGraph&             graph,
          CorrectedMutualInformation<>& I,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >&
             sep_set) {
      std::vector< std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > >
         triples;
      for (NodeId z: graph) {
        for (NodeId x: graph.neighbours(z)) {
          for (NodeId y: graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              std::vector< NodeId >       ui;
              std::pair< NodeId, NodeId > key     = {x, y};
              std::pair< NodeId, NodeId > rev_key = {y, x};
              if (sep_set.exists(key)) {
                ui = sep_set[key];
              } else if (sep_set.exists(rev_key)) {
                ui = sep_set[rev_key];
              }
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) { ui.erase(iter_z_place); }

              double Ixyz_ui = I.score(x, y, z, ui);
              std::pair< std::tuple< NodeId, NodeId, NodeId >*, double > triple;
              auto tup      = new std::tuple< NodeId, NodeId, NodeId >{x, y, z};
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
    std::vector<
       std::
          tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double > >
       Miic::getUnshieldedTriplesMIIC_(
          const MixedGraph&             graph,
          CorrectedMutualInformation<>& I,
          const HashTable< std::pair< NodeId, NodeId >, std::vector< NodeId > >&
                                                          sep_set,
          HashTable< std::pair< NodeId, NodeId >, char >& marks) {
      std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                               double,
                               double,
                               double > >
         triples;
      for (NodeId z: graph) {
        for (NodeId x: graph.neighbours(z)) {
          for (NodeId y: graph.neighbours(z)) {
            if (y < x && !graph.existsEdge(x, y)) {
              std::vector< NodeId >       ui;
              std::pair< NodeId, NodeId > key     = {x, y};
              std::pair< NodeId, NodeId > rev_key = {y, x};
              if (sep_set.exists(key)) {
                ui = sep_set[key];
              } else if (sep_set.exists(rev_key)) {
                ui = sep_set[rev_key];
              }
              // remove z from ui if it's present
              const auto iter_z_place = std::find(ui.begin(), ui.end(), z);
              if (iter_z_place != ui.end()) { ui.erase(iter_z_place); }

              const double Ixyz_ui = I.score(x, y, z, ui);
              auto         tup = new std::tuple< NodeId, NodeId, NodeId >{x, y, z};
              std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                          double,
                          double,
                          double >
                 triple{tup, Ixyz_ui, 0.5, 0.5};
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
    std::vector<
       std::
          tuple< std::tuple< NodeId, NodeId, NodeId >*, double, double, double > >
       Miic::updateProbaTriples_(
          const MixedGraph&                   graph,
          std::vector< std::tuple< std::tuple< NodeId, NodeId, NodeId >*,
                                   double,
                                   double,
                                   double > > proba_triples) {
      for (auto& triple: proba_triples) {
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
      std::sort(proba_triples.begin(), proba_triples.end(), GreaterTupleOnLast());
      return proba_triples;
    }

    /// learns the structure of an Bayesian network, ie a DAG, from an Essential
    /// graph.
    DAG Miic::learnStructure(CorrectedMutualInformation<>& I,
                             MixedGraph                    initialGraph) {
      MixedGraph essentialGraph = learnMixedStructure(I, initialGraph);
      // std::cout << "Le mixed graph mesdames et messieurs: "
      //<< essentialGraph.toDot() << std::endl;

      // Second, orientate remaining edges
      const Sequence< NodeId > order = essentialGraph.topologicalOrder();
      // first, propagate existing orientations
      for (NodeId x: order) {
        if (!essentialGraph.parents(x).empty()) {
          propagatesHead_(essentialGraph, x);
        }
      }
      // std::cout << "Le mixed graph après une première propagation mesdames et
      // messieurs: "
      //<< essentialGraph.toDot() << std::endl;
      // then decide the orientation by the topological order and propagate them
      for (NodeId x: order) {
        if (!essentialGraph.neighbours(x).empty()) {
          propagatesHead_(essentialGraph, x);
        }
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
          if (!existsDirectedPath__(graph, neighbour, node)
              && !(initial_marks__.exists({node, neighbour})
                   && initial_marks__[{node, neighbour}] == '-')
              && graph.parents(neighbour).empty()) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(node, neighbour);
            // std::cout << "1. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "1. Adding arc (" << node << "," <<
            // neighbour << ")" << std::endl;
            propagatesHead_(graph, neighbour);
          } else if (!existsDirectedPath__(graph, node, neighbour)
                     && !(initial_marks__.exists({neighbour, node})
                          && initial_marks__[{neighbour, node}] == '-')
                     && graph.parents(node).empty()) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(neighbour, node);
            // std::cout << "2. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "2. Adding arc (" << neighbour << "," <<
            // node << ")" << std::endl;
          } else if (!existsDirectedPath__(graph, node, neighbour)
                     && !(initial_marks__.exists({neighbour, node})
                          && initial_marks__[{neighbour, node}] == '-')) {
            graph.eraseEdge(Edge(neighbour, node));
            graph.addArc(neighbour, node);
            if (!graph.parents(neighbour).empty()
                && !graph.parents(node).empty()) {
              latent_couples__.push_back(Arc(node, neighbour));
            }

            // std::cout << "3. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "3. Adding arc (" << neighbour << "," <<
            // node << ")" << std::endl;
          } else if (!existsDirectedPath__(graph, neighbour, node)
                     && !(initial_marks__.exists({node, neighbour})
                          && initial_marks__[{node, neighbour}] == '-')) {
            graph.eraseEdge(Edge(node, neighbour));
            graph.addArc(node, neighbour);
            if (!graph.parents(neighbour).empty()
                && !graph.parents(node).empty()) {
              latent_couples__.push_back(Arc(node, neighbour));
            }
            // std::cout << "4. Removing edge (" << neighbour << "," << node << ")"
            // << std::endl; std::cout << "4. Adding arc (" << node << "," <<
            // neighbour << ")" << std::endl;
          }
          // else if (!graph.parents(neighbour).empty()
          //&& !graph.parents(node).empty()) {
          // graph.eraseEdge(Edge(neighbour, node));
          // graph.addArc(node, neighbour);
          //__latent_couples.push_back(Arc(node, neighbour));
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
    const std::vector< Arc > Miic::latentVariables() const {
      return latent_couples__;
    }

    /// learns the structure and the parameters of a BN
    template < typename GUM_SCALAR,
               typename GRAPH_CHANGES_SELECTOR,
               typename PARAM_ESTIMATOR >
    BayesNet< GUM_SCALAR > Miic::learnBN(GRAPH_CHANGES_SELECTOR& selector,
                                         PARAM_ESTIMATOR&        estimator,
                                         DAG                     initial_dag) {
      return DAG2BNLearner<>::createBN< GUM_SCALAR >(
         estimator,
         learnStructure(selector, initial_dag));
    }

    void Miic::setMiicBehaviour() { this->usemiic__ = true; }
    void Miic::set3off2Behaviour() { this->usemiic__ = false; }

    void Miic::addConstraints(
       HashTable< std::pair< NodeId, NodeId >, char > constraints) {
      this->initial_marks__ = constraints;
    }


    const bool Miic::existsDirectedPath__(const MixedGraph& graph,
                                          const NodeId      n1,
                                          const NodeId      n2,
                                          const bool        countArc) const {
      // not recursive version => use a FIFO for simulating the recursion
      List< NodeId > nodeFIFO;
      nodeFIFO.pushBack(n2);

      // mark[node] = successor if visited, else mark[node] does not exist
      NodeProperty< NodeId > mark;
      mark.insert(n2, n2);

      NodeId current;

      while (!nodeFIFO.empty()) {
        current = nodeFIFO.front();
        nodeFIFO.popFront();

        // check the parents

        for (const auto new_one: graph.parents(current)) {
          if (!countArc && current == n2
              && new_one == n1)   // If countArc is set to false
            continue;             // paths of length 1 are ignored

          if (mark.exists(new_one))   // if this node is already marked, do not
            continue;                 // check it again

          if (graph.existsArc(current,
                              new_one))   // if there is a double arc, pass
            continue;

          mark.insert(new_one, current);

          if (new_one == n1) { return true; }

          nodeFIFO.pushBack(new_one);
        }
      }

      return false;
    }

  } /* namespace learning */

} /* namespace gum */

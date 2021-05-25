/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @brief source implementations of simplicial set
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/graphs/algorithms/simplicialSet.h>
#include <agrum/tools/graphs/graphElements.h>
#include <limits>
#include <limits>
#include <sstream>
#include <sstream>

#ifdef GUM_NO_INLINE
#  include <agrum/tools/graphs/algorithms/simplicialSet_inl.h>
#endif   // GUM_NO_INLINE

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  /* ===========================================================================
   */
  /* ===========================================================================
   */
  /* ===  CLASS FOR RETRIEVING SIMPLICIAL, ALMOST AND QUASI SIMPLICIAL NODES ===
   */
  /* ===========================================================================
   */
  /* ===========================================================================
   */

  /// constructor. initialize the simplicial set w.r.t. a given  _graph_
  SimplicialSet::SimplicialSet(UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               double                        theRatio,
                               double                        theThreshold) :
       _graph_(graph != nullptr
                 ? graph
                 : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                     "SimplicialSet requires a valid graph")),
       _log_weights_(log_weights != nullptr
                       ? log_weights
                       : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                           "SimplicialSet "
                                           "requires non-null log weights")),
       _log_domain_sizes_(log_domain_sizes != nullptr
                            ? log_domain_sizes
                            : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                                "SimplicialSet "
                                                "requires non-null domain sizes")),
       _simplicial_nodes_(std::less< double >(),  _graph_->size()),
       _almost_simplicial_nodes_(std::less< double >(),  _graph_->size()),
       _quasi_simplicial_nodes_(std::less< double >(),  _graph_->size()),
       _containing_list_( _graph_->size()),
       _nb_triangles_( _graph_->size() *  _graph_->size() / 2),
       _nb_adjacent_neighbours_( _graph_->size()),  _quasi_ratio_(theRatio),
       _log_threshold_(std::log(1 + theThreshold)) {
    if (graph != nullptr) { GUM_CONSTRUCTOR(SimplicialSet); }

    // end of initialization: compute  _nb_triangles_,  _nb_adjacent_neighbours_,
    // etc
     _initialize_();
  }

  /// copy constructor
  SimplicialSet::SimplicialSet(const SimplicialSet&          from,
                               UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               bool                          avoid_check) :
       _graph_(graph != nullptr
                 ? graph
                 : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                     "SimplicialSet requires a valid graph")),
       _log_weights_(log_weights != nullptr
                       ? log_weights
                       : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                           "SimplicialSet "
                                           "requires non-null log weights")),
       _log_domain_sizes_(
         log_domain_sizes != nullptr
            ? log_domain_sizes
            : GUM_ERROR_IN_EXPR(OperationNotAllowed,
                                "SimplicialSet "
                                "requires non-null domain sizes")) {
    if (!avoid_check) {
      // check whether the graph, the log weights and the log_modalities
      // are similar to those of from
      if (( _graph_ == from. _graph_) || ( _log_weights_ == from. _log_weights_)
          || (* _graph_ != *from. _graph_)
          || (* _log_domain_sizes_ != *from. _log_domain_sizes_)) {
        GUM_ERROR(OperationNotAllowed,
                  "SimplicialSet requires fresh copies of "
                  "graph, log weights and log domain sizes");
      }
    }

    // copy the current content of from
    * _log_weights_            = *from. _log_weights_;
     _simplicial_nodes_        = from. _simplicial_nodes_;
     _almost_simplicial_nodes_ = from. _almost_simplicial_nodes_;
     _quasi_simplicial_nodes_  = from. _quasi_simplicial_nodes_;
     _containing_list_         = from. _containing_list_;
     _nb_triangles_            = from. _nb_triangles_;
     _nb_adjacent_neighbours_  = from. _nb_adjacent_neighbours_;
     _log_tree_width_          = from. _log_tree_width_;
     _quasi_ratio_             = from. _quasi_ratio_;
     _log_threshold_           = from. _log_threshold_;
     _changed_status_          = from. _changed_status_;
     _we_want_fill_ins_        = from. _we_want_fill_ins_;
     _fill_ins_list_           = from. _fill_ins_list_;

    GUM_CONS_CPY(SimplicialSet);
  }

  /// move constructor
  SimplicialSet::SimplicialSet(SimplicialSet&& from) :
       _graph_(from. _graph_),  _log_weights_(from. _log_weights_),
       _log_domain_sizes_(from. _log_domain_sizes_),
       _simplicial_nodes_(std::move(from. _simplicial_nodes_)),
       _almost_simplicial_nodes_(std::move(from. _almost_simplicial_nodes_)),
       _quasi_simplicial_nodes_(std::move(from. _quasi_simplicial_nodes_)),
       _containing_list_(std::move(from. _containing_list_)),
       _nb_triangles_(std::move(from. _nb_triangles_)),
       _nb_adjacent_neighbours_(std::move(from. _nb_adjacent_neighbours_)),
       _log_tree_width_(from. _log_tree_width_),  _quasi_ratio_(from. _quasi_ratio_),
       _log_threshold_(from. _log_threshold_),
       _changed_status_(std::move(from. _changed_status_)),
       _we_want_fill_ins_(from. _we_want_fill_ins_),
       _fill_ins_list_(std::move(from. _fill_ins_list_)) {
    GUM_CONS_MOV(SimplicialSet);
  }

  /// destructor
  SimplicialSet::~SimplicialSet() { GUM_DESTRUCTOR(SimplicialSet); }

  /// adds the necessary edges so that node 'id' and its neighbors form a clique
  void SimplicialSet::makeClique(const NodeId id) {
    // first, update the list to which belongs the node
    if ( _changed_status_.contains(id))  _updateList_(id);

    // to make id be a clique, we may have to add edges. Hence, this will create
    // new triangles and we should update the number of triangles passing
    // through the new edges. Moreover, we should also update the number of
    // adjacent neighbors for each node involved in these new triangles as well
    // as the new weights of the nodes. Finally, we should update the
    // simplicial,
    // almost and quasi simplicial lists. However, to optimize the code, we use
    // a lazy list update: we just update a hashtable indicating whether a list
    // update should be performed for a given node. This enables performing the
    // updates only when necessary. if the node is known to be simplicial, there
    // is nothing to do
    if ( _simplicial_nodes_.contains(id)) {
      return;
    } else if ( _almost_simplicial_nodes_.contains(id)) {
      // get the neighbor that does not form a clique with the other neighbors
      // recall that id is an almost simplicial node if there exists a node,
      // say Y, such that, after deleting Y, id and its adjacent nodes form a
      // clique.
      const NodeSet& nei    =  _graph_->neighbours(id);
      Size           nb_adj = nei.size();
      Size           nb     =  _nb_adjacent_neighbours_[id];

      // nb_almost = the number of edges that should link the neighbors of
      // node id, after node Y mentioned above has been removed. Recall that
      // these neighbors and id form a clique. Hence this corresponds to the
      // number of triangles involving id and 2 of its neighbors, after node
      // Y has been removed.
      Size   nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
      NodeId node1     = 0;

      for (const auto current_node: nei) {
        if (nb_almost == nb -  _nb_triangles_[Edge(current_node, id)]) {
          // we found the neighbor we were looking for: nb = the number of
          // pairs of neighbors of id that are adjacent. In other words, this
          // is the number of triangles involving node id. Now remove from it
          // the
          // triangles involving edge (id,node1), and you get the number of
          // triangles involving id, but not node1. If id is almost simplicial,
          // then this number should be equal to the set of combinations of
          // all the possible pairs of neighbors of id except node1, hence
          // to nb_almost.
          node1 = current_node;
          break;
        }
      }

      double  log_domain_size_node1 = (* _log_domain_sizes_)[node1];
      double&  _log_weights_node1_   = (* _log_weights_)[node1];

      // now, to make a clique between id and its neighbors, there just remains
      // to add the missing edges between node1 and the other neighbors of id.

      // nb_n1 will contain the number of pairs of neighbors of node1 that
      // will be adjacent after the clique is constructed but that
      // are not yet adjacent
      unsigned int nb_n1 = 0;

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      for (const auto node2: nei) {
        if ((node2 != node1) && ! _graph_->existsEdge(node1, node2)) {
          // add the edge
          const Edge e1_2(node1, node2);
           _graph_->addEdge(node1, node2);

          if ( _we_want_fill_ins_)  _fill_ins_list_.insert(e1_2);

          if (! _changed_status_.contains(node2))  _changed_status_.insert(node2);

           _log_weights_node1_ += (* _log_domain_sizes_)[node2];
          (* _log_weights_)[node2] += log_domain_size_node1;
           _nb_triangles_.insert(e1_2, 0);

          // nb_n2 will contain the number of pairs of neighbors of node2 that
          // will be adjacent after the clique is constructed but that
          // are not yet adjacent
          unsigned int nb_n2 = 0;

          // for all common neighbors of node1 and node2, update the number of
          // triangles as well as the number of adjacent neighbors
          if ( _graph_->neighbours(node1).size()
              <=  _graph_->neighbours(node2).size()) {
            for (const auto neighbor:  _graph_->neighbours(node1)) {
              if ( _graph_->existsEdge(neighbor, node2)) {
                // here iter->other (node1) is a neighbor of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node1 )
                ++nb_n1;
                ++nb_n2;
                ++ _nb_adjacent_neighbours_[neighbor];
                ++ _nb_triangles_[Edge(node1, neighbor)];
                ++ _nb_triangles_[Edge(node2, neighbor)];

                if (! _changed_status_.contains(neighbor))
                   _changed_status_.insert(neighbor);
              }
            }
          } else {
            for (const auto neighbor:  _graph_->neighbours(node2)) {
              if ( _graph_->existsEdge(neighbor, node1)) {
                // here iter->other (node2) is a neighbor of node1 and node2
                // hence there is a new triangle to be taken into account:
                // that between node1, node2 and iterEdge->other( node2 )
                ++nb_n1;
                ++nb_n2;
                ++ _nb_adjacent_neighbours_[neighbor];
                ++ _nb_triangles_[Edge(node2, neighbor)];
                ++ _nb_triangles_[Edge(node1, neighbor)];

                if (! _changed_status_.contains(neighbor))
                   _changed_status_.insert(neighbor);
              }
            }
          }

           _nb_adjacent_neighbours_[node2] += nb_n2;
           _nb_triangles_[e1_2] += nb_n2;
        }
      }

      if (! _changed_status_.contains(node1))  _changed_status_.insert(node1);

       _nb_adjacent_neighbours_[node1] += nb_n1;
    } else {
      // here, id is neither a simplicial node nor an almost simplicial node

      // update the number of triangles of the edges and keep track of the
      // nodes involved.
      const NodeSet& nei =  _graph_->neighbours(id);

      for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
        NodeId       node1                 = *iter1;
        double       log_domain_size_node1 = (* _log_domain_sizes_)[node1];
        double&       _log_weights_node1_   = (* _log_weights_)[node1];
        bool         node1_status          = false;
        unsigned int nb_n1                 = 0;

        auto iterEdge2 = iter1;

        for (++iterEdge2; iterEdge2 != nei.end(); ++iterEdge2) {
          const NodeId node2 = *iterEdge2;
          const Edge   e1_2(node1, node2);

          if (! _graph_->existsEdge(e1_2)) {
            // add the edge
             _graph_->addEdge(node1, node2);

            if ( _we_want_fill_ins_)  _fill_ins_list_.insert(e1_2);

            if (! _changed_status_.contains(node2))  _changed_status_.insert(node2);

            node1_status = true;
             _log_weights_node1_ += (* _log_domain_sizes_)[node2];
            (* _log_weights_)[node2] += log_domain_size_node1;
             _nb_triangles_.insert(e1_2, 0);

            // for all common neighbors of node1 and node2, update the number
            // of triangles as well as the number of adjacent neighbors
            unsigned int nb_n2 = 0;

            if ( _graph_->neighbours(node1).size()
                <=  _graph_->neighbours(node2).size()) {
              for (const auto neighbor:  _graph_->neighbours(node1))
                if ( _graph_->existsEdge(neighbor, node2)) {
                  // here iterEdge->other (node1) is a neighbor of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++ _nb_adjacent_neighbours_[neighbor];
                  ++ _nb_triangles_[Edge(node1, neighbor)];
                  ++ _nb_triangles_[Edge(node2, neighbor)];

                  if (! _changed_status_.contains(neighbor))
                     _changed_status_.insert(neighbor);
                }
            } else {
              for (const auto neighbor:  _graph_->neighbours(node2)) {
                if ( _graph_->existsEdge(neighbor, node1)) {
                  // here iterEdge->other (node2) is a neighbor of
                  // both node1 and node2
                  ++nb_n1;
                  ++nb_n2;
                  ++ _nb_adjacent_neighbours_[neighbor];
                  ++ _nb_triangles_[Edge(node2, neighbor)];
                  ++ _nb_triangles_[Edge(node1, neighbor)];

                  if (! _changed_status_.contains(neighbor))
                     _changed_status_.insert(neighbor);
                }
              }
            }

             _nb_adjacent_neighbours_[node2] += nb_n2;
             _nb_triangles_[e1_2] += nb_n2;
          }
        }

         _nb_adjacent_neighbours_[node1] += nb_n1;

        if (node1_status && ! _changed_status_.contains(node1))
           _changed_status_.insert(node1);
      }
    }

    // update the  _changed_status_ of node id as well as its containing list
    if (! _simplicial_nodes_.contains(id)) {
      if ( _changed_status_.contains(id))  _changed_status_.erase(id);

      switch ( _containing_list_[id]) {
        case  _Belong_::ALMOST_SIMPLICIAL:
           _almost_simplicial_nodes_.erase(id);
          break;

        case  _Belong_::QUASI_SIMPLICIAL:
           _quasi_simplicial_nodes_.erase(id);
          break;

        default:
          break;
      }

       _simplicial_nodes_.insert(id, (* _log_weights_)[id]);
       _containing_list_[id] =  _Belong_::SIMPLICIAL;
    } else {
      if ( _changed_status_.contains(id)) {  _changed_status_.erase(id); }
    }
  }

  /// removes a node and its adjacent edges from the underlying  _graph_
  void SimplicialSet::eraseClique(const NodeId id) {
    // check if the node we wish to remove actually belongs to the  _graph_
    if (! _graph_->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " does not belong to the graph")
    }

    const NodeSet nei =  _graph_->neighbours(id);

    // check that node id is actually a clique
    Size nb_adj = nei.size();
    if ( _nb_adjacent_neighbours_[id] != (nb_adj * (nb_adj - 1)) / 2) {
      GUM_ERROR(NotFound, "Node " << id << " is not a clique")
    }

    // remove the node and its adjacent edges
    double log_domain_size_id = (* _log_domain_sizes_)[id];
    for (auto iter1 = nei.begin(); iter1 != nei.end(); ++iter1) {
      const NodeId node1 = *iter1;
       _nb_adjacent_neighbours_[node1] -= nb_adj - 1;
      (* _log_weights_)[node1] -= log_domain_size_id;

      if (! _changed_status_.contains(node1))  _changed_status_.insert(node1);

       _nb_triangles_.erase(Edge(node1, id));

      auto iter2 = iter1;
      for (++iter2; iter2 != nei.end(); ++iter2)
        -- _nb_triangles_[Edge(node1, *iter2)];
    }

     _log_tree_width_ = std::max( _log_tree_width_, (* _log_weights_)[id]);

    switch ( _containing_list_[id]) {
      case  _Belong_::SIMPLICIAL:
         _simplicial_nodes_.erase(id);
        break;

      case  _Belong_::ALMOST_SIMPLICIAL:
         _almost_simplicial_nodes_.erase(id);
        break;

      case  _Belong_::QUASI_SIMPLICIAL:
         _quasi_simplicial_nodes_.erase(id);
        break;

      default:
        break;
    }

     _nb_adjacent_neighbours_.erase(id);
     _containing_list_.erase(id);
     _changed_status_.erase(id);
     _graph_->eraseNode(id);
     _log_weights_->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying  _graph_
  void SimplicialSet::eraseNode(const NodeId id) {
    // check if the node we wish to remove actually belongs to the  _graph_
    if (! _graph_->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " does not belong to the graph")
    }

    // remove the node and its adjacent edges
    const NodeSet& nei =  _graph_->neighbours(id);

    for (auto iter = nei.beginSafe(); iter != nei.endSafe();
         ++iter)   // safe iterator needed here for deletions
      eraseEdge(Edge(*iter, id));

    switch ( _containing_list_[id]) {
      case  _Belong_::SIMPLICIAL:
         _simplicial_nodes_.erase(id);
        break;

      case  _Belong_::ALMOST_SIMPLICIAL:
         _almost_simplicial_nodes_.erase(id);
        break;

      case  _Belong_::QUASI_SIMPLICIAL:
         _quasi_simplicial_nodes_.erase(id);
        break;

      default:
        break;
    }

     _nb_adjacent_neighbours_.erase(id);
     _containing_list_.erase(id);
     _changed_status_.erase(id);
     _graph_->eraseNode(id);
     _log_weights_->erase(id);
  }

  /// removes a node and its adjacent edges from the underlying  _graph_
  void SimplicialSet::eraseEdge(const Edge& edge) {
    // check if the edge we wish to remove actually belongs to the  _graph_
    if (! _graph_->existsEdge(edge)) {
      GUM_ERROR(NotFound, "Edge " << edge << " does not belong to the graph")
    }

    // get the extremal nodes of the edge
    const NodeId node1 = edge.first();
    const NodeId node2 = edge.second();

    // remove the edge
     _graph_->eraseEdge(edge);
     _nb_triangles_.erase(edge);

    // update the  _log_weights_ of both nodes
    (* _log_weights_)[node1] -= (* _log_domain_sizes_)[node2];
    (* _log_weights_)[node2] -= (* _log_domain_sizes_)[node1];

    // update the number of triangles and the adjacent neighbors
    unsigned int nb_neigh_n1_n2 = 0;
    for (const auto othernode:  _graph_->neighbours(node1)) {
      if ( _graph_->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the
        //  _graph_
        -- _nb_triangles_[Edge(node1, othernode)];
        -- _nb_triangles_[Edge(node2, othernode)];
        // update the neighbors
        ++nb_neigh_n1_n2;
        -- _nb_adjacent_neighbours_[othernode];

        if (! _changed_status_.contains(othernode))
           _changed_status_.insert(othernode);
      }
    }

     _nb_adjacent_neighbours_[node1] -= nb_neigh_n1_n2;
     _nb_adjacent_neighbours_[node2] -= nb_neigh_n1_n2;

    if (! _changed_status_.contains(node1))  _changed_status_.insert(node1);
    if (! _changed_status_.contains(node2))  _changed_status_.insert(node2);
  }

  /// adds a new edge to the  _graph_ and recomputes the simplicial set
  void SimplicialSet::addEdge(NodeId node1, NodeId node2) {
    // if the edge already exists, do nothing
    const Edge edge(node1, node2);

    if ( _graph_->existsEdge(edge)) return;

    // update the  _log_weights_ of both nodes
    (* _log_weights_)[node1] += (* _log_domain_sizes_)[node2];
    (* _log_weights_)[node2] += (* _log_domain_sizes_)[node1];

    unsigned int nb_triangle_in_new_edge = 0;
    unsigned int nb_neigh_n1_n2          = 0;

    for (const auto othernode:  _graph_->neighbours(node1)) {
      if ( _graph_->existsEdge(node2, othernode)) {
        // udate the number of triangles passing through the egdes of the
        //  _graph_
        ++ _nb_triangles_[Edge(node1, othernode)];
        ++ _nb_triangles_[Edge(node2, othernode)];
        ++nb_triangle_in_new_edge;

        // update the neighbors
        ++nb_neigh_n1_n2;
        ++ _nb_adjacent_neighbours_[othernode];

        if (! _changed_status_.contains(othernode))
           _changed_status_.insert(othernode);
      }
    }

     _nb_adjacent_neighbours_[node1] += nb_neigh_n1_n2;
     _nb_adjacent_neighbours_[node2] += nb_neigh_n1_n2;

    // add the edge
     _graph_->addEdge(node1, node2);
     _nb_triangles_.insert(edge, nb_triangle_in_new_edge);

    if (! _changed_status_.contains(node1))  _changed_status_.insert(node1);
    if (! _changed_status_.contains(node2))  _changed_status_.insert(node2);
  }

  /// put node id in the correct simplicial/almost simplicial/quasi simplicial
  /// list
  void SimplicialSet:: _updateList_(const NodeId id) {
    // check if the node belongs to the  _graph_
    if (! _graph_->exists(id)) {
      GUM_ERROR(NotFound, "Node " << id << " could not be found")
    }

    // check if the status of the node has changed
    if (! _changed_status_.contains(id)) return;

     _changed_status_.erase(id);

     _Belong_&      belong =  _containing_list_[id];
    const NodeSet& nei    =  _graph_->neighbours(id);
    Size           nb_adj = nei.size();

    // check if the node should belong to the simplicial set
    if ( _nb_adjacent_neighbours_[id] == (nb_adj * (nb_adj - 1)) / 2) {
      if (belong !=  _Belong_::SIMPLICIAL) {
        if (belong ==  _Belong_::ALMOST_SIMPLICIAL)
           _almost_simplicial_nodes_.erase(id);
        else if (belong ==  _Belong_::QUASI_SIMPLICIAL)
           _quasi_simplicial_nodes_.erase(id);

         _simplicial_nodes_.insert(id, (* _log_weights_)[id]);
        belong =  _Belong_::SIMPLICIAL;
      }

      return;
    }

    // check if the node should belong to the almost simplicial set
    Size nb_almost = ((nb_adj - 1) * (nb_adj - 2)) / 2;
    Size nb        =  _nb_adjacent_neighbours_[id];

    for (const auto cur: nei) {
      if (nb_almost == nb -  _nb_triangles_[Edge(cur, id)]) {
        // the node is an almost simplicial node
        if (belong !=  _Belong_::ALMOST_SIMPLICIAL) {
          if (belong ==  _Belong_::SIMPLICIAL)
             _simplicial_nodes_.erase(id);
          else if (belong ==  _Belong_::QUASI_SIMPLICIAL)
             _quasi_simplicial_nodes_.erase(id);

           _almost_simplicial_nodes_.insert(id, (* _log_weights_)[id]);
          belong =  _Belong_::ALMOST_SIMPLICIAL;
        } else
           _almost_simplicial_nodes_.setPriority(id, (* _log_weights_)[id]);

        return;
      }
    }

    // check if the node should belong to the quasi simplicial nodes
    if ( _nb_adjacent_neighbours_[id] / ((nb_adj * (nb_adj - 1)) / 2)
        >=  _quasi_ratio_) {
      if (belong !=  _Belong_::QUASI_SIMPLICIAL) {
        if (belong ==  _Belong_::SIMPLICIAL)
           _simplicial_nodes_.erase(id);
        else if (belong ==  _Belong_::ALMOST_SIMPLICIAL)
           _almost_simplicial_nodes_.erase(id);

         _quasi_simplicial_nodes_.insert(id, (* _log_weights_)[id]);
        belong =  _Belong_::QUASI_SIMPLICIAL;
      } else
         _quasi_simplicial_nodes_.setPriority(id, (* _log_weights_)[id]);

      return;
    }

    // the node does not belong to any list, so remove the node from
    // its current list
    if (belong ==  _Belong_::QUASI_SIMPLICIAL)
       _quasi_simplicial_nodes_.erase(id);
    else if (belong ==  _Belong_::ALMOST_SIMPLICIAL)
       _almost_simplicial_nodes_.erase(id);
    else if (belong ==  _Belong_::SIMPLICIAL)
       _simplicial_nodes_.erase(id);

    belong =  _Belong_::NO_LIST;
  }

  /// indicates whether there exists an almost simplicial node
  bool SimplicialSet::hasAlmostSimplicialNode() {
    // set the limit weight value
    double limit =  _log_tree_width_ +  _log_threshold_;

    // update the elements currently in the almost simplicial list that may
    // now be contained in another list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
      if ( _almost_simplicial_nodes_.contains(*iter))  _updateList_(*iter);
    }

    // check the current almost simplicial list
    if (! _almost_simplicial_nodes_.empty()
        && ((* _log_weights_)[ _almost_simplicial_nodes_.top()] <= limit))
      return true;

    // if the almost simplicial list does not contain any node that has a low
    // weight, check if a node can enter the almost simplicial list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
       _updateList_(*iter);

      if (! _almost_simplicial_nodes_.empty()
          && ((* _log_weights_)[ _almost_simplicial_nodes_.top()] <= limit))
        return true;
    }

    return false;
  }

  /// indicates whether there exists an almost simplicial node
  bool SimplicialSet::hasSimplicialNode() {
    // update the elements currently in the simplicial list that may
    // now be contained in another list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
      if ( _simplicial_nodes_.contains(*iter))  _updateList_(*iter);
    }

    // check the current almost simplicial list
    if (! _simplicial_nodes_.empty()) return true;

    // if the simplicial list does not contain any node, check if a
    // node can enter the simplicial list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
       _updateList_(*iter);

      if (! _simplicial_nodes_.empty()) return true;
    }

    return false;
  }

  /// indicates whether there exists a quasi simplicial node
  bool SimplicialSet::hasQuasiSimplicialNode() {
    // set the limit weight value
    double limit =  _log_tree_width_ +  _log_threshold_;

    // update the elements currently in the quasi simplicial list that may
    // now be contained in another list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
      if ( _quasi_simplicial_nodes_.contains(*iter))  _updateList_(*iter);
    }

    // check the current quasi simplicial list
    if (! _quasi_simplicial_nodes_.empty()
        && ((* _log_weights_)[ _quasi_simplicial_nodes_.top()] <= limit))
      return true;

    // if the quasi simplicial list does not contain any node that has a low
    // weight, check if a node can enter the quasi simplicial list
    for (auto iter =  _changed_status_.beginSafe();   // safe iterator needed here
         iter !=  _changed_status_.endSafe();
         ++iter) {
       _updateList_(*iter);

      if (! _quasi_simplicial_nodes_.empty()
          && ((* _log_weights_)[ _quasi_simplicial_nodes_.top()] <= limit))
        return true;
    }

    return false;
  }

  /** @brief initialize: compute  _nb_triangles_,  _nb_adjacent_neighbours_, etc
   * when a new graph is set */
  void SimplicialSet:: _initialize_() {
    // if the  _graph_ is empty, do nothing
    if ( _graph_->size() == 0) return;

    // set the weights of the nodes and the initial tree_width (min of the
    // weights)
     _log_tree_width_ = std::numeric_limits< double >::max();
     _log_weights_->clear();

    for (const auto nodeX: * _graph_) {
      double log_weight = (* _log_domain_sizes_)[nodeX];
      for (const auto& nei:  _graph_->neighbours(nodeX))
        log_weight += (* _log_domain_sizes_)[nei];

       _log_weights_->insert(nodeX, log_weight);
      if ( _log_tree_width_ > log_weight)  _log_tree_width_ = log_weight;
    }

    // initialize the  _nb_triangles_ so that there is no need to check whether
    //  _nb_triangles_ need new insertions
     _nb_triangles_           =  _graph_->edgesProperty(Size(0));
     _nb_adjacent_neighbours_ =  _graph_->nodesProperty(Size(0));
     _containing_list_        =  _graph_->nodesProperty( _Belong_::NO_LIST);
     _changed_status_         =  _graph_->asNodeSet();

    // set the  _nb_triangles_ and the  _nb_adjacent_neighbours_: for each
    // triangle, update the  _nb_triangles_. To count the triangles only once,
    // parse for each node X the set of its neighbors Y,Z that are adjacent to
    // each other and such that the Id of Y and Z are greater than X.
    for (const auto nodeX: * _graph_) {
      Size&          nb_adjacent_neighbors_idX =  _nb_adjacent_neighbours_[nodeX];
      const NodeSet& nei                       =  _graph_->neighbours(nodeX);

      for (auto iterY = nei.begin(); iterY != nei.end(); ++iterY)
        if (*iterY > nodeX) {
          const NodeId node_idY           = *iterY;
          Size& nb_adjacent_neighbors_idY =  _nb_adjacent_neighbours_[node_idY];

          auto iterZ = iterY;
          for (++iterZ; iterZ != nei.end(); ++iterZ)
            if ((*iterZ > nodeX) &&  _graph_->existsEdge(node_idY, *iterZ)) {
              const NodeId node_idZ = *iterZ;
              ++nb_adjacent_neighbors_idX;
              ++nb_adjacent_neighbors_idY;
              ++ _nb_adjacent_neighbours_[node_idZ];
              ++ _nb_triangles_[Edge(nodeX, node_idY)];
              ++ _nb_triangles_[Edge(nodeX, node_idZ)];
              ++ _nb_triangles_[Edge(node_idZ, node_idY)];
            }
        }
    }
  }

  /// initialize the simplicial set w.r.t. a new graph
  void SimplicialSet::setGraph(UndiGraph*                    graph,
                               const NodeProperty< double >* log_domain_sizes,
                               NodeProperty< double >*       log_weights,
                               double                        theRatio,
                               double                        theThreshold) {
    // check that the pointers passed in argument are all different from 0
    if ((graph == nullptr) || (log_domain_sizes == nullptr)
        || (log_weights == nullptr)) {
      GUM_ERROR(OperationNotAllowed, "SimplicialSet requires non-null pointers")
    }

    // clear the structures used for the previous graph and assign the new graph
     _graph_            = graph;
     _log_weights_      = log_weights;
     _log_domain_sizes_ = log_domain_sizes;

     _simplicial_nodes_.clear();
     _almost_simplicial_nodes_.clear();
     _quasi_simplicial_nodes_.clear();
     _simplicial_nodes_.resize( _graph_->size());
     _almost_simplicial_nodes_.resize( _graph_->size());
     _quasi_simplicial_nodes_.resize( _graph_->size());

     _containing_list_.clear();
     _containing_list_.resize( _graph_->size());
     _nb_triangles_.clear();
     _nb_triangles_.resize( _graph_->size() *  _graph_->size() / 2);
     _nb_adjacent_neighbours_.clear();
     _nb_adjacent_neighbours_.resize( _graph_->size());

     _log_tree_width_ = std::numeric_limits< double >::max();
     _quasi_ratio_    = theRatio;
     _log_threshold_  = std::log(1 + theThreshold);
     _changed_status_.clear();
     _fill_ins_list_.clear();

    // end of initialization: compute  _nb_triangles_,  _nb_adjacent_neighbours_,
    // etc
     _initialize_();
  }

  /// reassigns a new set of cliques' log weights (with the same content)
  void SimplicialSet::replaceLogWeights(NodeProperty< double >* old_weights,
                                        NodeProperty< double >* new_weights) {
    // check that the current weights are the old_weights
    if (old_weights !=  _log_weights_)
      GUM_ERROR(InvalidArgument,
                "the old set of weights shall be identical "
                "to the current one");

     _log_weights_ = new_weights;
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

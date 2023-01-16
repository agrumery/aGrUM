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


/**
 * @file
 * @brief Headers of the Pattern class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_PATTERN_H
#define GUM_PATTERN_H

#include <agrum/agrum.h>


#include <agrum/PRM/gspan/DFSCode.h>
#include <agrum/PRM/gspan/interfaceGraph.h>

namespace gum {
  namespace prm {
    namespace gspan {

      class NeighborIterator;

      /**
       * @class Pattern
       * @headerfile pattern.h <agrum/PRM/gspan/pattern.h>
       * This contains all the information we want for a node in a DFSTree.
       *
       * Several rules are used regarding nodes in Pattern::graph.
       * First nodes are added respecting a depth-first search, thus each node
       * is labelled from 1 to n, where n is the number of nodes in
       *Pattern::graph.
       *
       * Given two nodes id i and j, if i < j then i was discovered before j in
       *the
       * depth first search.
       *
       * To retrieve the depth first search tree from Pattern::graph simple
       *consider
       *only
       * arcs (u, v) with u < v. The set of arcs { (u,v) | u < v} is called the
       *forward edge
       * set and the set of arcs { (u,v) | u > v} is called the backward edge
       *set.
       *
       * The orientation in Pattern::graph is only used to differentiate forward
       *edges
       * from backward edges.
       *
       */
      class Pattern: private DiGraph {
        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        Pattern();

        /// Copy constructor.
        Pattern(const Pattern& source);

        /// Destructor.
        ~Pattern();

        /// @}
        // =========================================================================
        /// @name Graphical getters and setters.
        // ==========================================================================
        /// @{

        /**
         * @brief Insert a node with the given LabelData.
         * @returns The id assigned to the inserted node.
         */
        NodeId addNodeWithLabel(LabelData& l);

        /// Returns the LabelData assigned to node.
        LabelData& label(NodeId node);

        /// Returns the LabelData assigned to node.
        const LabelData& label(NodeId node) const;

        /// Returns the LabelData assigned to arc.
        LabelData& label(NodeId i, NodeId j);

        /// Returns the LabelData assigned to arc.
        const LabelData& label(NodeId i, NodeId j) const;

        /// Returns the LabelData assigned to arc.
        LabelData& label(const Arc& arc);

        /// Returns the LabelData assigned to arc.
        const LabelData& label(const Arc& arc) const;

        // Returns the last added LabelData.
        LabelData& lastAdded();

        // Returns the last added LabelData.
        const LabelData& lastAdded() const;

        /**
         * @brief Add an arc to this Pattern.
         *
         * This create an EdgeCode and check if it respects
         * neighborhood restriction, if not an exception is raised.
         *
         * @param i The DFS subscript of the first node in the code.
         * @param j The DFS subscript of the second node in the code.
         * @param l The label data of the added edge.
         *
         * @throw OperationNotAllowed Raised if the neighborhood restriction
         *                            is not respected.
         */
        void addArc(NodeId i, NodeId j, LabelData& l);

        /// Returns true if id is a node in this Pattern.
        bool exists(NodeId id) const;

        /// Returns true if (tail, head) is an arc in this Pattern.
        bool exists(NodeId tail, NodeId head) const;

        /// Returns the number of nodes in this Pattern.
        Size size() const;

        /// Returns the number of arcs in this Pattern.
        Size sizeArcs() const;

        /// Fill r_path with the rightmost path of this Pattern.
        /// The list is supposed empty.
        void rightmostPath(std::list< NodeId >& r_path) const;

        /// Print the pattern in the DOT syntax.
        std::string toDot(size_t name) const;

        /// @}
        // =========================================================================
        /// @name Access to topology
        // ==========================================================================
        /// @{
        const NodeGraphPart& nodes() const;

        const ArcSet& arcs() const;

        /// @}
        // =========================================================================
        /// @name DFSCode related methods.
        // ==========================================================================
        /// @{

        /// Returns the DFSCode of this Pattern.
        DFSCode& code();

        /// Returns the DFSCode of this Pattern.
        const DFSCode& code() const;

        /// Returns the EdgeCode of an edge of this Pattern.
        EdgeCode& edgeCode(NodeId tail, NodeId head);

        /// Returns the EdgeCode of an edge of this Pattern.
        EdgeCode& edgeCode(const Arc& arc);

        /// Returns the EdgeCode of an edge of this Pattern.
        const EdgeCode& edgeCode(NodeId tail, NodeId head) const;

        /// Returns the EdgeCode of an edge of this Pattern.
        const EdgeCode& edgeCode(const Arc& arc) const;

        /// Remove the last EdgeCode of this pattern.
        void pop_back();

        /// Remove a node if it has no neighbors, raise an OperationNotAllowed
        /// otherwise
        void remove(NodeId node);

        bool isMinimal();

        /// @}

        private:
        /// The DFSCode of this Pattern.
        DFSCode _code_;

        /// Mapping between nodes in this Pattern and their respective
        /// LabelData.
        NodeProperty< LabelData* > _node_map_;

        /// Mapping between edges in this Pattern and their respective
        /// LabelData.
        ArcProperty< std::pair< LabelData*, EdgeCode* > > _arc_map_;

        /// The last LabelData added to this pattern.
        LabelData* _last_ = nullptr;

        /// Returns true if the expand code by adding and edge betwenne u and v
        /// is
        /// minimal
        /// with respect to  _code_.
        /// @param u A node in this Pattern.
        /// @param v A node in this Pattern.
        /// @returns true if the expand code by adding and edge betwenne u and v
        /// is
        /// minimal
        ///          with respect to  _code_.
        bool _expandCodeIsMinimal_(NodeId u, NodeId v);

        /// Recurisve method used by  _expandCodeIsMinimal_.
        /// @param p A Pattern.
        /// @param node_map A bijection.
        /// @param u A node in this Pattern.
        /// @param v A node in this Pattern.
        /// @return true if the expansion is minimal.
        bool _rec_(Pattern& p, Bijection< NodeId, NodeId >& node_map, NodeId u, NodeId v);

        /// A non recursive bugged version of  _rec_.
        bool _not_rec_(Pattern& p, Bijection< NodeId, NodeId >& node_map, NodeId u, NodeId v);

        // to avoid clang++ warnings
        using DiGraph::addNode;
        using DiGraph::addArc;
        using DiGraph::toDot;
        using DiGraph::parents;
        using DiGraph::children;
      };
    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#  include <agrum/PRM/gspan/pattern_inl.h>
#endif   // GUM_NO_INLINE

#endif /* GUM_PATTERN_H */

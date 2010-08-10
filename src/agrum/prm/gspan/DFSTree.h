// /***************************************************************************
//  *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
//  *   {prenom.nom}_at_lip6.fr                                               *
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   This program is distributed in the hope that it will be useful,       *
//  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//  *   GNU General Public License for more details.                          *
//  *                                                                         *
//  *   You should have received a copy of the GNU General Public License     *
//  *   along with this program; if not, write to the                         *
//  *   Free Software Foundation, Inc.,                                       *
//  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
//  ***************************************************************************/
// /**
//  * @file
//  * @brief Headers of the DFSTree class.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// #include <list>
// #include <utility>
// #include <ostream>
// // ============================================================================
// #include <agrum/core/sequence.h>
// #include <agrum/core/set.h>
// #include <agrum/core/bijection.h>
// // ============================================================================
// #include <agrum/graphs/diGraph.h>
// // ============================================================================
// #include <agrum/prm/interfaceGraph.h>
// #include <agrum/prm/pattern.h>
// // ============================================================================
// #ifndef GUM_DFS_TREE_H
// #define GUM_DFS_TREE_H
// // ============================================================================
// namespace gum {
// 
// /**
//  * @class DFSTree DFSTree.h <agrum/prm/DFSTree.h>
//  *
//  * A DFSTree is used by gspan to sort lexicographically patterns discovered
//  * in an interface graph.
//  */
// class DFSTree: private DiGraph {
// 
//   public:
// 
//     // =========================================================================
//     /// @name Constructor and destructor.
//     // ==========================================================================
//     /// @{
// 
//     // Default constructor.
//     DFSTree(InterfaceGraph* graph);
// 
//     // Destructor.
//     ~DFSTree();
// 
//     /// @}
//     // =========================================================================
//     /// @name DFSTree getters and setters.
//     // ==========================================================================
//     /// @{
// 
//     /// Returns the list of root patterns in this DFSTree.
//     std::list<NodeId>& roots();
// 
//     /// Returns the list of root patterns in this DFSTree.
//     const std::list<NodeId>& roots() const;
// 
//     /// Returns the parent of p in this DFSTree.
//     Pattern& parent(const Pattern& p);
// 
//     /// Returns the parent of p in this DFSTree.
//     const Pattern& parent(const Pattern& p) const;
// 
//     /// Returns the list of p children in this DFSTree.
//     std::list<NodeId>& children(const Pattern& p);
// 
//     /// Returns the list of p children in this DFSTree.
//     const std::list<NodeId>& children(const Pattern& p) const;
// 
//     /// Returns the pattern represented by id in this DFSTree.
//     Pattern& pattern(NodeId id);
// 
//     /// Returns the pattern represented by id in this DFSTree.
//     const Pattern& pattern(NodeId id) const;
// 
//     /**
//      * @brief Add a one edge Pattern in this DFSTree.
//      *
//      * @param data Data over the edge used to create a root of this DFSTree.
//      * @return Returns the Pattern added as a root of this DFSTree.
//      */
//     void addRoot(LabelData& data);
// 
//     /**
//      * @class DFSTree::EdgeGrowth DFSTree.h <agrum/prm/DFSTree.h>
//      * This class is used to define an edge growth of a pattern
//      * in this DFSTree.
//      */
//     struct EdgeGrowth {
//       friend class DFSTree;
//       /// Constructor.
//       EdgeGrowth(NodeId a_u, LabelData* an_edge, LabelData* a_l_v, NodeId a_v=0);
//       /// Copy constructor.
//       EdgeGrowth(const EdgeGrowth& from);
//       /// Destructor.
//       ~EdgeGrowth();
//       /// The NodeId of the node from which we grow an edge.
//       NodeId u;
//       /// Returns the number of matches in the interface graph
//       /// for this edge growth.
//       Size count();
//       /// The LabelData over the edge of this edge growth.
//       LabelData* edge;
//       /// The LabelData over the node of this edge growth.
//       LabelData* l_v;
//       /// If the growth is backward you must assigned the subscript of v,
//       /// otherwise 0 is assigned (recall that subscripts start from 1)
//       NodeId v;
//       /// Add the pair (u,v) as a match for the current growth.
//       void insert(Instance* u, Instance* v);
//       /// The mapping between the u and v for each match in the interface graph.
//       Property< std::pair<Instance*, Instance*> >::onNodes matches;
//       /// Return a string representation of this
//       std::string toString() {
//         std::stringstream str;
//         str << u << "-" << edge << "-" << l_v << "-" << v;
//         return str.str();
//       }
//       private:
//       /// The iso graph for computing the maximum independent set of matches.
//       UndiGraph iso_graph;
//       /// Vector used for computation
//       std::vector<NodeId>* degree_list;
//       /// The max indep set of matches
//       Set<NodeId> max_indep_set;
//     };
// 
//     /**
//      * @brief Add a one edge growth of p as one of its child.
//      *
//      * The child is inserted lexicographically among the children of p.
//      * However if the child is found to be not minimal an
//      * OperationNotAllowed is raised.
//      *
//      * @param p The Pattern from which a one edge growth is spawned.
//      * @param edge_growth The data about the edge growth of p.
//      *
//      * @throw FatalError Raised if the grow is an illegal backedge growth.
//      * @throw OperationNotAllowed Raised if the grow is found to be not minimal.
//      */
//     Pattern& growPattern(Pattern& p, EdgeGrowth& edge_growth, Size min_freq);
// 
//     /// @}
//     // =========================================================================
//     /// @name Isomorphisms for patterns in this DFSTree.
//     // ==========================================================================
//     /// @{
// 
//     /**
//      * @brief Returns the isomorphism graph of p in the interface graph.
//      *
//      * The isomorphism graph is a undirected graph in which each node represents
//      * a set of Instance matching p in the interface graph.
//      *
//      * If there exists an edge between two nodes in the isomorphism graph, then
//      * the two respective set of instances are not disjoint.
//      *
//      * @param p The pattern for which we want the isomorphism graph.
//      * @return The isomorphism graph of p.
//      *
//      * @throw NotFound Raised if p is not a node in this DFSTree.
//      */
//     UndiGraph& iso_graph(const Pattern& p);
// 
//     /**
//      * @brief Given a pattern and a node in its isomorphism graph, this methods
//      *        returns the sequence of instance matching p in the interface graph.
//      *
//      * The sequence of instances respect DSF subscripting. Each node in the
//      * pattern's graph have a DSF subscript from 1 to n, where n is the number
//      * of nodes in the pattern's graph.
//      *
//      * If for a given match you want the k-th instance repecting p's DFS subscripting,
//      * then it will be the (k - 1)th element in the sequence.
//      *
//      * @param p The pattern for which we want a match in the interface graph.
//      * @param node The node in p isomorphism graph for which we want the matching
//      *             set if instances.
//      * @return Returns the sequence of instances matching p and node.
//      *
//      * @throw NotFound Raised if p or node does not exists.
//      */
//     Sequence<Instance*>& iso_map(const Pattern& p, NodeId node);
// 
//     /**
//      * @brief Returns the maximal independent set of p isomorphism graph.
//      *
//      * @pram p The pattern for which we want its maximal independent set.
//      *
//      * @throw NotFound Raised if p is not a node in this DFSTree.
//      */
//     Set<NodeId>& max_indep_set(const Pattern& p);
// 
//     Sequence< HashTable<ClassElement*, Size>* >& sub_patterns(const Pattern& p);
// 
//     HashTable<NodeId, Idx>& sub_patterns_map(const Pattern& p);
// 
//     /// Returns the frequency of p respecting it's maximal independent set.
//     double frequency(const Pattern& p) const;
// 
//     /// Returns the cost induced by p.
//     double cost(const Pattern& p);
// 
//     /// Returns the gain induced by p.
//     double gain(const Pattern& p);
// 
//     /// Returns the score of p.
//     double score(const Pattern& p);
// 
//     /// @class This is used to generate the max_indep_set of a Pattern.
//     struct NeighborDegreeSort {
//       /// Constructor
//       NeighborDegreeSort(UndiGraph& graph):
//         g(graph)
//       {
//         GUM_CONSTRUCTOR( DFSTree::NeighborDegreeSort );
//       }
//       /// Copy constructor.
//       NeighborDegreeSort(const NeighborDegreeSort& source):
//         g(source.g)
//       {
//         GUM_CONS_CPY( DFSTree::NeighborDegreeSort );
//       }
//       /// Destructor.
//       ~NeighborDegreeSort() {
//         GUM_DESTRUCTOR( DFSTree::NeighborDegreeSort );
//       }
//       /// The isomorphism graph.
//       UndiGraph& g;
//       /// The operator used to sort stuff.
//       bool operator() (NodeId i, NodeId j) {
//         return g.neighbours(i).size() < g.neighbours(j).size();
//       }
//     };
// 
//     /// @}
//   private:
// 
//     /// Stuff we want to know about patterns in this DFSTree.
//     struct PatternData {
//       /// Constructor.
//       PatternData(Pattern* p):
//         pattern(p), cost(0), gain(0)
//       {
//         GUM_CONSTRUCTOR( DFSTree::PatternData );
//       }
//       /// Copy constructor.
//       PatternData(const PatternData& from):
//         pattern(from.pattern), children(from.children), iso_graph(from.iso_graph),
//         max_indep_set(from.max_indep_set), cost(from.cost), gain(from.gain)
//       {
//         GUM_CONS_CPY( DFSTree::PatternData );
//         typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
//         for (Iter iter = from.iso_map.begin(); iter != from.iso_map.end(); ++iter) {
//           iso_map.insert(iter.key(), new Sequence<Instance*>(**iter));
//         }
//       }
//       /// Destructor.
//       ~PatternData() {
//         GUM_DESTRUCTOR( DFSTree::PatternData );
//         typedef Property<Sequence<Instance*>*>::onNodes::const_iterator Iter;
//         for (Iter iter = iso_map.begin(); iter != iso_map.end(); ++iter) {
//           delete *iter;
//         }
//         typedef Sequence< HashTable<ClassElement*, Size>* >::iterator SubPatIter;
//         for (SubPatIter iter = sub_patterns.begin(); iter != sub_patterns.end(); ++iter) {
//           delete *iter;
//         }
//       }
//       /// The pattern.
//       Pattern* pattern;
//       /// The list of the pattern's children, sorted lexicographically.
//       std::list<NodeId> children;
//       /// The isomorphism graph of the pattern.
//       UndiGraph iso_graph;
//       /// The instances matching p in the interface graph.
//       Property<Sequence<Instance*>*>::onNodes iso_map;
//       /// The maximal independent set of p.
//       Set<NodeId> max_indep_set;
//       /// The cost of this Pattern
//       Size cost;
//       /// The gain of this Pattern
//       Size gain;
//       /// The different sub_patterns of p given the iso_map.
//       Sequence< HashTable<ClassElement*, Size>* > sub_patterns;
//       /// The mapping between an iso_map and the given sub pattern.
//       HashTable<NodeId, Idx> sub_patterns_map;
//       /// The number of each sub pattern
//       std::vector<Size> sub_patterns_count;
//     };
// 
//     /// The interface graph on which this DFSTree applies.
//     InterfaceGraph* __graph;
// 
//     /// The list of root patterns in this DFSTree.
//     std::list<NodeId> __roots;
// 
//     /// The mapping between nodes in this DFSTree and the patterns they represents.
//     Bijection<NodeId, Pattern*> __node_map;
// 
//     /// Data about patterns in this DFSTree.
//     HashTable<Pattern*, PatternData*> __data;
// 
//     /// Raise different exceptions if child is invalid or illegal
//     void __checkGrowth(Pattern& p, Pattern* child, EdgeGrowth& edge_growth);
// 
//     /// Add a child to this DFSTree.
//     void __addChild(Pattern& p, Pattern* child, EdgeGrowth& edge_growth);
// 
//     void __find_sub_pattern(Pattern& p, NodeId iso_map);
// 
//     bool __test_equality(HashTable<ClassElement*, Size>& x, HashTable<ClassElement*, Size>& y);
// 
//     bool __is_new_seq(Sequence<Instance*>& seq, Property<Sequence<Instance*>*>::onNodes& iso_map);
// 
//     void __initialiaze_root(Pattern* p, Sequence<EdgeData*>& seq);
// 
// };
// 
// std::ostream& operator<<(std::ostream& out, const DFSTree::EdgeGrowth &edge);
// 
// } /* namespace gum */
// #ifndef GUM_NO_INLINE
// #include <agrum/prm/DFSTree.inl>
// #endif // GUM_NO_INLINE
// // ============================================================================
// #endif /* GUM_DFS_TREE_H */
// // ============================================================================

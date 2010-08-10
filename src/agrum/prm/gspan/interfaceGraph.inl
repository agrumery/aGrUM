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
//  * @brief Inline implementation of gum::InterfaceGraph.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// namespace gum {
// 
// INLINE
// UndiGraph&
// InterfaceGraph::graph() { return __graph; }
// 
// INLINE
// const UndiGraph&
// InterfaceGraph::graph() const { return __graph; }
// 
// INLINE
// Bijection<Idx, LabelData*>&
// InterfaceGraph::labels() { return *__labels; }
// 
// INLINE
// const Bijection<Idx, LabelData*>&
// InterfaceGraph::labels() const { return *__labels; }
// 
// INLINE
// Size
// InterfaceGraph::size(const LabelData* l) const {
//   try {
//     return __nodeMap[const_cast<LabelData*>(l)]->size();
//   } catch ( NotFound& ) {
//     return __edgeMap[const_cast<LabelData*>(l)]->size();
//   }
// }
// 
// INLINE
// Set<NodeData*>&
// InterfaceGraph::nodes(const LabelData* l) {
//   return *(__nodeMap[const_cast<LabelData*>(l)]);
// }
// 
// INLINE
// const Set<NodeData*>&
// InterfaceGraph::nodes(const LabelData* l) const {
//   return *(__nodeMap[const_cast<LabelData*>(l)]);
// }
// 
// INLINE
// Set<EdgeData*>&
// InterfaceGraph::edges(const LabelData* l) {
//   return *(__edgeMap[const_cast<LabelData*>(l)]);
// }
// 
// INLINE
// const Set<EdgeData*>&
// InterfaceGraph::edges(const LabelData* l) const {
//   return *(__edgeMap[const_cast<LabelData*>(l)]);
// }
// 
// INLINE
// LabelData*
// InterfaceGraph::label(Idx id) {
//   return __labels->second(id);
// }
// 
// INLINE
// NodeId
// InterfaceGraph::id(const Instance& i) const {
//   return __idMap[const_cast<Instance*>(&i)];
// }
// 
// INLINE
// NodeId
// InterfaceGraph::id(const Instance* i) const {
//   return __idMap[const_cast<Instance*>(i)];
// }
// 
// INLINE
// NodeData&
// InterfaceGraph::node(const Instance* i) {
//   return node(id(i));
// }
// 
// INLINE
// const NodeData&
// InterfaceGraph::node(const Instance* i) const {
//   return node(id(i));
// }
// 
// INLINE
// NodeData&
// InterfaceGraph::node(NodeId id) {
//   return *(__nodes[id]);
// }
// 
// INLINE
// const NodeData&
// InterfaceGraph::node(NodeId id) const {
//   return *(__nodes[id]);
// }
// 
// INLINE
// EdgeData&
// InterfaceGraph::edge(NodeId u, NodeId v) {
//   try {
//     return *(__edges[Edge(u, v)]);
//   } catch (NotFound&) {
//     return *(__edges[Edge(v, u)]);
//   }
// }
// 
// INLINE
// const EdgeData&
// InterfaceGraph::edge(NodeId u, NodeId v) const {
//   try {
//     return *(__edges[Edge(u, v)]);
//   } catch (NotFound&) {
//     return *(__edges[Edge(v, u)]);
//   }
// }
// 
// INLINE
// std::ostream&
// operator<<(std::ostream& out, const LabelData& data) {
//   out << data.l;
//   return out;
// }
// 
// INLINE
// std::ostream&
// operator<<(std::ostream& out, const NodeData& data) {
//   out << data.n->name() << "(" << data.l->l << ")";
//   return out;
// }
// 
// INLINE
// std::ostream&
// operator<<(std::ostream& out, const EdgeData& data) {
//   out << data.u->name() << " -> " << data.v->name() << "(" << data.l->l << ")";
//   return out;
// }
// 
// } /* namespace gum */
// // ============================================================================

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
//  * @brief Implementation of HollowBayesNet.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// #include <agrum/prm/hollowBayesNet.h>
// // ============================================================================
// #ifdef GUM_NO_INLINE
// #include <agrum/prm/hollowBayesNet.inl>
// #endif // GUM_NO_INLINE
// // ============================================================================
// namespace gum {
// namespace prm {
// 
// HollowBayesNet::HollowBayesNet(const ClassElementContainer& cec):
//   AbstractBayesNet<prm_float>(), __cec(&cec), __moralGraph(0), __topo(0)
// {
//   GUM_CONSTRUCTOR( HollowBayesNet );
//   __init(cec);
// }
// 
// void
// HollowBayesNet::__init(const ClassElementContainer& cec) {
//   std::list<NodeId> queue;
//   for (DAG::NodeIterator node = cec.dag().beginNodes();
//        node != cec.dag().endNodes(); ++node) {
//     if (cec.dag().parents(*node).empty()) {
//       queue.push_back(*node);
//     }
//   }
//   while (not queue.empty()) {
//     if (not __dag.exists(queue.front())) {
//       if (ClassElement::isAttribute(cec.get(queue.front())) or
//           ClassElement::isAggregate(cec.get(queue.front()))) {
//         __dag.insertNode(queue.front());
//         __varNodeMap.insert(&(__cec->get(queue.front()).type().variable()),
//                             queue.front());
//         __modalities.insert(queue.front(), __cec->get(queue.front()).type().variable().domainSize());
//       }
//       // Adding children
//       for (DAG::ArcIterator chld = cec.children(queue.front()).begin();
//           chld != cec.children(queue.front()).end(); ++chld) {
//           queue.push_back(chld->head());
//       }
//     }
//     queue.pop_front();
//   }
// }
// 
// HollowBayesNet::HollowBayesNet(const HollowBayesNet& from):
//   AbstractBayesNet<prm_float>(from), __cec(from.__cec), __dag(from.__dag),
//   __moralGraph(0), __topo(0)
// {
//   GUM_CONS_CPY( HollowBayesNet );
//   if (from.__moralGraph != 0) {
//     __moralGraph = new UndiGraph(*(from.__moralGraph));
//   }
//   if (from.__topo != 0) {
//     __topo = new Sequence<NodeId>(*(from.__topo));
//   }
// }
// 
// HollowBayesNet&
// HollowBayesNet::operator=(const HollowBayesNet& from) {
//   if (this != &from) {
//     AbstractBayesNet<prm_float>::operator=(from);
//     if (__moralGraph != 0) {
//       delete __moralGraph;
//     }
//     if (__topo != 0) {
//       delete __topo;
//     }
//     __cec = from.__cec;
//     __dag = from.__dag;
//     if (from.__moralGraph != 0) {
//       __moralGraph = new UndiGraph(*(from.__moralGraph));
//     }
//     if (from.__topo != 0) {
//       __topo = new Sequence<NodeId>(*(from.__topo));
//     }
//   }
//   return *this;
// }
// 
// } /* namespace prm */
// } /* namespace gum */
// // ============================================================================

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
//  * @brief Inline implementation of gspan.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// namespace gum {
// 
// INLINE
// GSpan::GSpan(InterfaceGraph& graph, Size min_freq, Size depth_stop):
//   __graph(&graph), __min_freq(min_freq), __depth_stop(depth_stop), __tree(&graph)
// {
//   GUM_CONSTRUCTOR( GSpan );
// }
// 
// INLINE
// GSpan::~GSpan() {
//   GUM_DESTRUCTOR( GSpan );
//   for (HashTable<Pattern*, GSpan::MatchedInstances* >::iterator match = __matched_instances.begin();
//        match != __matched_instances.end(); ++match) {
//     delete *match;
//   }
//   typedef HashTable<Pattern*, BayesNet<prm_float>*>::iterator BNIter;
//   for (BNIter iter = __bn_map.begin(); iter != __bn_map.end(); ++iter) {
//     delete *iter;
//   }
// }
// 
// INLINE
// DFSTree&
// GSpan::tree() { return __tree; }
// 
// INLINE
// unsigned long
// GSpan::__cost_func(unsigned int interface, unsigned int frequency) {
//   return interface * frequency;
// }
// 
// INLINE
// std::vector<Pattern*>&
// GSpan::patterns() {
//   return __patterns;
// }
// 
// INLINE
// HashTable<Pattern*, GSpan::MatchedInstances*>&
// GSpan::matches() {
//   return __matched_instances;
// }
// 
// INLINE
// InterfaceGraph&
// GSpan::ig() {
//   return *__graph;
// }
// 
// INLINE
// BayesNet<prm_float>&
// GSpan::baseBN(Pattern& p) {
//   return *(__bn_map[&p]);
// }
// 
// } /* namespace gum */
// // ============================================================================

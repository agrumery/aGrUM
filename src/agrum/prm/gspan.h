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
//  * @brief Headers of gspan.
//  *
//  * @author Lionel TORTI
//  */
// // ============================================================================
// #include <string>
// #include <ostream>
// #include <vector>
// #include <algorithm>
// // ============================================================================
// #include <agrum/BN/inference/valueElimination.h>
// #include <agrum/multidim/CPFBijection.h>
// #include <agrum/prm/PRM.h>
// #include <agrum/prm/interfaceGraph.h>
// #include <agrum/prm/DFSTree.h>
// // ============================================================================
// #ifndef GUM_GSPAN_H
// #define GUM_GSPAN_H
// // ============================================================================
// namespace gum {
// 
// 
// /**
//  * @class gspan gspan.h <agrum/prm/gspan.h>
//  *
//  */
// class GSpan {
// 
//   public:
// 
//     /**
//      * Default constructor.
//      *
//      * @param graph The interfaceGraph on which gspan proceeds with a
//      *              pattern mining.
//      * @param min_freq The minimal frequency value used by gspan.
//      */
//     GSpan(InterfaceGraph& graph, Size min_freq=2, Size depth_stop=10);
// 
//     /// Destructor.
//     ~GSpan();
// 
//     /**
//      * Do the stuff.
//      */
//     void goForIt();
// 
//     /// Returns the DFSTree.
//     DFSTree& tree();
// 
//     /// Returns the InterfaceGraph used by this.
//     InterfaceGraph& ig();
// 
//     /// Returns the Pattern mined by this in a decreasing order of
//     /// interest.
//     std::vector<Pattern*>& patterns();
// 
//     /// Code alias.
//     typedef Set< Sequence<Instance*>* > MatchedInstances;
// 
//     /// Returns a mapping between patterns and the sequence of instance
//     /// in the interface graph matching them.
//     HashTable<Pattern*, MatchedInstances*>& matches();
// 
//     /// Returns the cumulative sum of all the cliques size created after lifting
//     /// the patterns.
//     double patterns_cost();
// 
//     /// Returns the cumulative sum of all the cliques size created after lifting
//     /// the inner attributes.
//     double sve_cost();
// 
//     /// Generate a bayes net for each usable pattern.
//     void generateBN(size_t number);
// 
//     /// Returns the basic BayesNet of a pattern.
//     BayesNet<prm_float>& baseBN(Pattern& p);
// 
//     void infer(size_t number);
// 
//   private:
// 
//     InterfaceGraph* __graph;
// 
//     Size __min_freq;
// 
//     Size __depth_stop;
// 
//     std::vector<LabelData*> __nodes;
// 
//     std::vector<LabelData*> __edges;
// 
//     HashTable<LabelData*, unsigned long> __cost;
// 
//     DFSTree __tree;
// 
//     std::vector<Pattern*> __patterns;
// 
//     HashTable<Pattern*, MatchedInstances* > __matched_instances;
// 
//     Set<Instance*> __chosen;
// 
//     HashTable<Pattern*, BayesNet<prm_float>*> __bn_map;
// 
//     //HashTable<Pattern*, Sequence<
// 
//     void __sortNodesAndEdges();
// 
//     void __subgraph_mining(InterfaceGraph& graph, Pattern& p);
// 
//     unsigned long __cost_func(unsigned int interface, unsigned int frequency);
// 
//     void __printIsoMap(Pattern& p);
// 
//     void __sortPatterns();
// 
//     double __instance_cost(Instance* i);
// 
//     BayesNet<prm_float>* __generateBNTopology(Sequence<Instance*>& seq);
// 
//     void __declare_variables(BayesNetFactory<prm_float>& factory,
//                              Sequence<Instance*>& seq);
// 
//     void __declare_parents(BayesNetFactory<prm_float>& factory,
//                            Sequence<Instance*>& seq);
// 
//     void __fill_inner_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq);
// 
//     void __fill_input_nodes(BayesNet<prm_float>& bn, Sequence<Instance*>& seq);
// 
//     // ========================================================================
//     /// @name Private classes
//     // ========================================================================
//     /// @{
// 
//     struct LabelSort;
// 
//     friend struct LabelSort;
// 
//     struct LabelSort {
//       LabelSort(GSpan* my_gspan):
//         gspan(my_gspan)
//       {
//         GUM_CONSTRUCTOR( GSpan::LabelSort );
//       }
// 
//       LabelSort(const LabelSort& source):
//         gspan(source.gspan)
//       {
//         GUM_CONS_CPY( GSpan::LabelSort );
//       }
// 
//       ~LabelSort() {
//         GUM_DESTRUCTOR( GSpan::LabelSort );
//       }
// 
//       bool operator() (LabelData* i, LabelData* j)
//       {
//         // We want a descending order
//         return gspan->__cost[i] > gspan->__cost[j];
//       }
// 
//       GSpan* gspan;
//     };
// 
//     struct PatternSort {
//       PatternSort(GSpan* my_gspan):
//         gspan(my_gspan)
//       {
//         GUM_CONSTRUCTOR( GSpan::PatternSort );
//       }
// 
//       PatternSort(const PatternSort& source):
//         gspan(source.gspan)
//       {
//         GUM_CONS_CPY( GSpan::PatternSort );
//       }
// 
//       ~PatternSort() {
//         GUM_DESTRUCTOR( GSpan::PatternSort );
//       }
// 
//       bool operator() (Pattern* i, Pattern* j)
//       {
//         // We want a descending order
//         return gspan->tree().frequency(*i) * (gspan->tree().gain(*i) / gspan->tree().cost(*i)) >
//                gspan->tree().frequency(*j) * (gspan->tree().gain(*j) / gspan->tree().cost(*j));
//       }
// 
//       GSpan* gspan;
//     };
// 
//     /// @}
// };
// 
// 
// } /* namespace gum */
// #ifndef GUM_NO_INLINE
// #include <agrum/prm/gspan.inl>
// #endif // GUM_NO_INLINE
// // ============================================================================
// #endif /* GUM_GSPAN_H */
// // ============================================================================

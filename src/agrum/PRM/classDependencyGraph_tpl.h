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


/**
 * @file
 * @brief Inline implementation of ClassDependencyGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(@LIP6)
 */
#include <agrum/PRM/classDependencyGraph.h>

namespace gum {
  namespace prm {

    // Destructor.
    template < typename GUM_SCALAR >
    ClassDependencyGraph< GUM_SCALAR >::~ClassDependencyGraph() {
      GUM_DESTRUCTOR(ClassDependencyGraph);

      for (const auto& elt: node_map__)
        delete elt.second;

      for (const auto& elt: elt_map__)
        delete elt.second;
    }

    // Build the class dependency graph.
    template < typename GUM_SCALAR >
    void ClassDependencyGraph< GUM_SCALAR >::buildGraph__(
       const PRM< GUM_SCALAR >& prm) {
      // First we add all nodes
      for (const auto ci: prm.classes()) {
        node_map__.insert(
           ci, new HashTable< const PRMClassElement< GUM_SCALAR >*, NodeId >());

        for (const auto node: ci->containerDag().nodes())
          addNode__(ci, ci->get(node));
      }

      for (const auto ii: prm.interfaces()) {
        node_map__.insert(
           ii, new HashTable< const PRMClassElement< GUM_SCALAR >*, NodeId >());

        for (const auto node: ii->containerDag().nodes()) {
          addNode__(ii, ii->get(node));
        }
      }

      // Then we add the arcs
      for (const auto cc: prm.classes())
        for (const auto node: cc->containerDag().nodes())
          addArcs__(*cc, node, *(node_map__[cc]));
    }

    // Add arcs in graph__.
    template < typename GUM_SCALAR >
    void ClassDependencyGraph< GUM_SCALAR >::addArcs__(
       const PRMClassElementContainer< GUM_SCALAR >&              c,
       NodeId                                                     node,
       HashTable< const PRMClassElement< GUM_SCALAR >*, NodeId >& map) {
      switch (c.get(node).elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_slotchain: {
          const PRMSlotChain< GUM_SCALAR >& sc =
             static_cast< const PRMSlotChain< GUM_SCALAR >& >(c.get(node));

          for (const auto chi: c.containerDag().children(node))
            graph__.addArc((*(node_map__[&(sc.end())]))[&(
                              sc.end().get(sc.lastElt().safeName()))],
                           map[&(c.get(chi))]);

          break;
        }

        case PRMClassElement< GUM_SCALAR >::prm_aggregate:
        case PRMClassElement< GUM_SCALAR >::prm_attribute: {
          for (const auto chi: c.containerDag().children(node))
            graph__.addArc(map[&(c.get(node))], map[&(c.get(chi))]);

          break;
        }

        default: { /* do nothing */ break;
        }
      }
    }

    template < typename GUM_SCALAR >
    INLINE ClassDependencyGraph< GUM_SCALAR >::ClassDependencyGraph(
       const PRM< GUM_SCALAR >& prm) {
      GUM_CONSTRUCTOR(ClassDependencyGraph);
      buildGraph__(prm);
    }

    template < typename GUM_SCALAR >
    INLINE ClassDependencyGraph< GUM_SCALAR >::ClassDependencyGraph(
       const ClassDependencyGraph< GUM_SCALAR >& source) :
        graph__(source.graph__),
        modalitites__(source.modalitites__), elt_map__(source.elt_map__) {
      GUM_CONS_CPY(ClassDependencyGraph);

      for (const auto elt: source.node_map__) {
        node_map__.insert(
           elt.first,
           new HashTable< const PRMClassElement< GUM_SCALAR >*, NodeId >(
              *elt.second));
      }
    }

    template < typename GUM_SCALAR >
    INLINE const DAG& ClassDependencyGraph< GUM_SCALAR >::dag() const {
      return graph__;
    }

    template < typename GUM_SCALAR >
    INLINE const typename ClassDependencyGraph< GUM_SCALAR >::EltPair&
       ClassDependencyGraph< GUM_SCALAR >::get(NodeId id) const {
      return *(elt_map__[id]);
    }

    template < typename GUM_SCALAR >
    INLINE NodeId ClassDependencyGraph< GUM_SCALAR >::get(
       const PRMClassElementContainer< GUM_SCALAR >& c,
       const PRMClassElement< GUM_SCALAR >&          elt) const {
      return (*(node_map__[&c]))[&elt];
    }

    template < typename GUM_SCALAR >
    INLINE const NodeProperty< Size >&
                 ClassDependencyGraph< GUM_SCALAR >::modalities() const {
      return modalitites__;
    }

    template < typename GUM_SCALAR >
    INLINE void ClassDependencyGraph< GUM_SCALAR >::addNode__(
       const PRMClassElementContainer< GUM_SCALAR >* c,
       const PRMClassElement< GUM_SCALAR >&          elt) {
      switch (elt.elt_type()) {
        case PRMClassElement< GUM_SCALAR >::prm_attribute:
        case PRMClassElement< GUM_SCALAR >::prm_aggregate: {
          NodeId id = graph__.addNode();
          elt_map__.insert(
             id, new ClassDependencyGraph< GUM_SCALAR >::EltPair(c, &elt));
          node_map__[c]->insert(&elt, id);
          modalitites__.insert(id, elt.type().variable().domainSize());
          break;
        }

        default: { /* do nothing */ break;
        }
      }
    }

  } /* namespace prm */
} /* namespace gum */

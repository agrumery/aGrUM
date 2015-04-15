/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Headers of ClassDependencyGraph<GUM_SCALAR>.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_ClassDependencyGraph_H
#define GUM_ClassDependencyGraph_H

#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {

    /**
     * @class ClassDependencyGraph<GUM_SCALAR> ClassDependencyGraph<GUM_SCALAR>.h
     *<agrum/PRM/ClassDependencyGraph<GUM_SCALAR>.h>
     * @brief This class represent the dependencies of all classes in a
     *PRM<GUM_SCALAR>.
     *
     * A Class Dependency Graph does listen to changes in it's PRM<GUM_SCALAR>.
     */
    template <typename GUM_SCALAR> class ClassDependencyGraph {

      public:
      /// Association between a class element and it's holding class.
      typedef std::pair<const ClassElementContainer<GUM_SCALAR> *,
                        const ClassElement<GUM_SCALAR> *> EltPair;

      // ========================================================================
      /// @name Constructors and Destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      /// @param prm The PRM<GUM_SCALAR> for which this
      /// ClassDependencyGraph<GUM_SCALAR> is constructed.
      ClassDependencyGraph(const PRM<GUM_SCALAR> &prm);

      /// Copy constructor.
      ClassDependencyGraph(const ClassDependencyGraph<GUM_SCALAR> &source);

      /// Destructor.
      ~ClassDependencyGraph();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /// Returns a constant reference over the graph of the DAG representing
      /// the ClassDependencyGraph<GUM_SCALAR>.
      const DAG &dag() const;

      /// Returns a constant reference over the element assiociated with the node
      /// id in the ClassDependencyGraph<GUM_SCALAR>.
      /// @throw NotFound Raised if no nodes matches id.
      const EltPair &get(NodeId id) const;

      /// @brief Returns the NodeId assign to the given ClassElement<GUM_SCALAR> of a
      ///        given Class.
      /// Is is necessary to give both Class and ClassElement<GUM_SCALAR> because
      /// inherited ClassElement<GUM_SCALAR> are shared in the inheritance hierarchy.
      NodeId get(const ClassElementContainer<GUM_SCALAR> &c,
                 const ClassElement<GUM_SCALAR> &elt) const;

      /// Returns a mapping between the ClassDependencyGraph<GUM_SCALAR>'s nodes and
      /// their
      /// modalities.
      const NodeProperty<unsigned int> &modalities() const;

      /// @}
      private:
      /// Build the class dependency graph.
      void __buildGraph(const PRM<GUM_SCALAR> &prm);

      /// Add nodes in __graph while updating consequently all the mappings.
      void __addNode(const ClassElementContainer<GUM_SCALAR> *c,
                     const ClassElement<GUM_SCALAR> &elt);

      /// Add arcs in __graph.
      void __addArcs(const ClassElementContainer<GUM_SCALAR> &c, NodeId node,
                     HashTable<const ClassElement<GUM_SCALAR> *, NodeId> &map);

      /// The graph itself.
      DAG __graph;

      /// The modalities map for each node in the ClassDependencyGraph<GUM_SCALAR>.
      /// This
      /// is useful when using a Triangulation class over a
      /// ClassDependencyGraph<GUM_SCALAR>.
      NodeProperty<unsigned int> __modalitites;

      /// Mapping between the nodes in __graph with the ClassElement<GUM_SCALAR> in
      /// the
      /// PRM<GUM_SCALAR>.
      NodeProperty<EltPair *> __elt_map;

      /// Code shortcut.
      typedef HashTable<const ClassElementContainer<GUM_SCALAR> *,
                        HashTable<const ClassElement<GUM_SCALAR> *, NodeId> *>
          NodeMap;

      /// Map each Class to a HashTable mapping the Class's ClassElements to their
      /// assigned NodeId in __graph.
      NodeMap __node_map;
    };

    extern template class ClassDependencyGraph<double>;
  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/classDependencyGraph.tcc>

#endif /* GUM_ClassDependencyGraph_H */

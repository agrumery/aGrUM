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
 * @brief Headers of CDG.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/prm/PRM.h>

#ifndef GUM_CDG_H
#define GUM_CDG_H

namespace gum {
  namespace prm {

    /**
     * @class CDG CDG.h <agrum/prm/CDG.h>
     * @brief This class represent the dependencies of all classes in a PRM.
     *
     * A Class Dependency Graph does listen to changes in it's PRM.
     */
    class CDG {

      public:
        /// Association between a class element and it's holding class.
        typedef std::pair<const ClassElementContainer*, const ClassElement*> EltPair;

        // ========================================================================
        /// @name Constructors and Destructor.
        // ========================================================================
        /// @{

        /// Default constructor.
        /// @param prm The PRM for which this CDG is constructed.
        CDG( const PRM& prm );

        /// Copy constructor.
        CDG( const CDG& source );

        /// Destructor.
        ~CDG();

        /// @}
        // ========================================================================
        /// @name Getters & setters.
        // ========================================================================
        /// @{

        /// Returns a constant reference over the graph of the DAG representing
        /// the CDG.
        const DAG& dag() const;

        /// Returns a constant reference over the element assiociated with the node
        /// id in the CDG.
        /// @throw NotFound Raised if no nodes matches id.
        const EltPair& get( NodeId id ) const;

        /// @brief Returns the NodeId assign to the given ClassElement of a
        ///        given Class.
        /// Is is necessary to give both Class and ClassElement because
        /// inherited ClassElement are shared in the inheritance hierarchy.
        NodeId get( const ClassElementContainer& c, const ClassElement& elt ) const;

        /// Returns a mapping between the CDG's nodes and their
        /// modalities.
        const NodeProperty<unsigned int>& modalities() const;

        /// @}
      private:

        /// Build the class dependency graph.
        void __buildGraph( const PRM& prm );

        /// Add nodes in __graph while updating consequently all the mappings.
        void __addNode( const ClassElementContainer* c, const ClassElement& elt );

        /// Add arcs in __graph.
        void __addArcs( const ClassElementContainer& c, NodeId node,
                        HashTable<const ClassElement*, NodeId>& map );

        /// The graph itself.
        DAG __graph;

        /// The modalities map for each node in the CDG. This
        /// is useful when using a Triangulation class over a CDG.
        NodeProperty<unsigned int> __modalitites;

        /// Mapping between the nodes in __graph with the ClassElement in the
        /// PRM.
        NodeProperty< EltPair* > __elt_map;

        /// Code shortcut.
        typedef HashTable<const ClassElementContainer*, HashTable<const ClassElement*, NodeId>* > NodeMap;

        /// Map each Class to a HashTable mapping the Class's ClassElements to their
        /// assigned NodeId in __graph.
        NodeMap __node_map;
    };

  } /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/CDG.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_CDG_H */


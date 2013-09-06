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
 * @brief Headers of InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <string>
#include <sstream>
#include <list>

#include <agrum/core/hashTable.h>

#include <agrum/graphs/undiGraph.h>

#include <agrum/prm/PRM.h>

#ifndef GUM_INTERFACE_GRAPH_H
#define GUM_INTERFACE_GRAPH_H

namespace gum {
  namespace prm {
    class GSpan;
    namespace gspan {

      /**
       * @struct LabelData interfaceGraph.h <agrum/prm/gspan/interfaceGraph.h>
       * Inner class to handle data about labels in this interface graph.
       */
      struct LabelData {
        /// Constructor.
        LabelData();
        /// Copy constructor.
        LabelData( const LabelData& from );
        /// Destructor.
        ~LabelData();
        /// An unique identifier for this label.
        Idx id;
        /// The string version of this label.
        std::string l;
        /// The size in terms of tree width of the given label.
        unsigned long tree_width;
        /// Equality operator.
        bool operator==( const LabelData& from ) const;
        /// Difference operator.
        bool operator!=( const LabelData& from ) const;
      };

      /**
       * Print a LabelData in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      std::ostream& operator<<( std::ostream& out, const LabelData& data );

      /**
       * @struct NodeData interfaceGraph.h <agrum/prm/gspan/interfaceGraph.h>
       * Inner class to handle data about nodes in __graph.
       */
      struct NodeData {
        /// Constructor.
        NodeData();
        /// Copy Constructor.
        NodeData( const NodeData& from );
        /// Destructor.
        ~NodeData();
        /// The instance represented by this node.
        Instance* n;
        /// The label of this node.
        LabelData* l;
        /// Equality operator.
        bool operator==( const NodeData& from ) const;
        /// Difference operator.
        bool operator!=( const NodeData& from ) const;
      };

      /**
       * Print a NodeData in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      std::ostream& operator<<( std::ostream& out, const NodeData& data );

      /**
       * @struct EdgeData interfaceGraph.h <agrum/prm/gspan/interfaceGraph.h>
       * Inner class to handle data about edges in __graph.
       */
      struct EdgeData {
        /// Constructor.
        EdgeData();
        /// Copy constructor.
        EdgeData( const EdgeData& from );
        /// Destructor.
        ~EdgeData();
        /// One of the two instance represented by this edge.
        Instance* u;
        /// The label data of u.
        LabelData* l_u;
        /// The other instance represented by thus edge
        Instance* v;
        /// The label data of v.
        LabelData* l_v;
        /// The labal data of this edge.
        LabelData* l;
        /// Equality operator.
        bool operator==( const EdgeData& from ) const;
        /// Difference operator.
        bool operator!=( const EdgeData& from ) const;
      };

      /**
       * Print a EdgeData in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      std::ostream& operator<<( std::ostream& out, const EdgeData& data );

      /**
       * @class InterfaceGraph interfaceGraph.h <agrum/prm/gspan/interfaceGraph.h>
       *
       * @brief This class represent the interface graph of a given gum::prm::System.
       *
       * An interface graph is a labelled graph over the instances of a gum::prm::System,
       * where there exists an edge between two instance i and j if and only if their
       * shared interface is nonempty.
       *
       * Labels assigned to edges and nodes in the interface graph are technically
       * strings, however since we need a linear oder each label is assigned a unique
       * id.
       *
       */
      class InterfaceGraph {
          friend class gum::prm::GSpan;

        public:

          /// Default constructor.
          InterfaceGraph( const System& sys );

          /// Copy constructor, proceeds with a shallow copy so for friends only.
          InterfaceGraph( const InterfaceGraph& source );

          /// Copy operator.
          InterfaceGraph& operator=( const InterfaceGraph& source );

          /// Destructor.
          ~InterfaceGraph();

          /// Returns the graph of this interface graph
          UndiGraph& graph();

          /// Returns the graph of this interface graph
          const UndiGraph& graph() const;

          /// Returns the bijection between LabelData and their string representation.
          Bijection<Idx, LabelData*>& labels();

          /// Returns the bijection between LabelData and their string representation.
          const Bijection<Idx, LabelData*>& labels() const;

          /// Returns the number of node or edges labelled by l.
          Size size( const LabelData* l ) const;

          /// Returns the set of nodes labelled by l.
          Set<NodeData*>& nodes( const LabelData* l );

          /// Returns the set of nodes labelled by l.
          const Set<NodeData*>& nodes( const LabelData* l ) const;

          /// Returns the set of nodes labelled by l.
          Set<EdgeData*>& edges( const LabelData* l );

          /// Returns the set of nodes labelled by l.
          const Set<EdgeData*>& edges( const LabelData* l ) const;

          /// Returns a label given its id
          LabelData* label( Idx id );

          /// Returns the id of i in this interface graph
          NodeId id( const Instance& i ) const;

          /// Returns the id of i in this interface graph
          NodeId id( const Instance* i ) const;

          /// Returns data about a node.
          /// @throw NotFound
          NodeData& node( const Instance* i );

          /// Returns data about a node.
          /// @throw NotFound
          const NodeData& node( const Instance* i ) const;

          /// Returns data about a node.
          /// @throw NotFound
          NodeData& node( NodeId id );

          /// Returns data about a node.
          /// @throw NotFound
          const NodeData& node( NodeId id ) const;

          /// Returns data about an edge.
          /// @throw NotFound
          EdgeData& edge( NodeId u, NodeId v );

          /// Returns data about an edge.
          /// @throw NotFound
          const EdgeData& edge( NodeId u, NodeId v ) const;

        private:
          /// The gum::prm::System represented by this interface graph.
          const System* __sys;

          /// The interface graph.
          UndiGraph __graph;

          /// Data associated with a node in __graph.
          NodeProperty<NodeData*> __nodes;

          /// Mapping between Instance dans their NodeId in __graph.
          HashTable<Instance*, NodeId> __idMap;

          /// Data associated with edges in __graph.
          EdgeProperty<EdgeData*> __edges;

          /// Bijection between labels and their ids.
          Bijection<Idx, LabelData*>* __labels;

          /// Mapping between a LabelData and the set of NodeData with that label.
          HashTable<LabelData*, Set<NodeData*>*> __nodeMap;

          /// Mapping between a LabelData and the set of EdgeData with that label.
          HashTable<LabelData*, Set<EdgeData*>*> __edgeMap;

          /// A counter used of assigning ids to labels.
          Idx __counter;

          /// For shallow copies
          bool __erase_flag;

          /// Compute the label of node and add it to __labels if it does not
          /// exists yet. Update node with the correct label's id.
          void __label( NodeData* node, HashTable<std::string, LabelData*>& label_map );

          /// Compute the label of edge and add it to __labels if it does not
          /// exists yet. Update edge with the correct label's id.
          void __label( EdgeData* edge, HashTable<std::string, LabelData*>& label_map );

      };

    } /* namespace gspan */
  } /* namespace prm */
} /* namespace gum */
#ifndef GUM_NO_INLINE
#include <agrum/prm/gspan/interfaceGraph.inl>
#endif // GUM_NO_INLINE

#endif /* GUM_INTERFACE_GRAPH_H */


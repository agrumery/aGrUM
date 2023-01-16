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
 * @brief Headers of InterfaceGraph.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_INTERFACE_GRAPH_H
#define GUM_INTERFACE_GRAPH_H

#include <list>
#include <sstream>
#include <string>

#include <agrum/PRM/PRM.h>

namespace gum {
  namespace prm {
    template < typename GUM_SCALAR >
    class GSpan;

    namespace gspan {

      /**
       * @struct LabelData interfaceGraph.h <agrum/PRM/gspan/interfaceGraph.h>
       * Inner class to handle data about labels in this interface graph.
       */
      struct LabelData {
        /// Constructor.
        LabelData();
        /// Copy constructor.
        LabelData(const LabelData& from);
        /// Destructor.
        ~LabelData();
        /// An unique identifier for this label.
        Idx id;
        /// The string version of this label.
        std::string l;
        /// The size in terms of tree width of the given label.
        Size tree_width;
        /// Equality operator.
        bool operator==(const LabelData& from) const;
        /// Difference operator.
        bool operator!=(const LabelData& from) const;
      };

      /**
       * Print a LabelData in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      std::ostream& operator<<(std::ostream& out, const LabelData& data);

      /**
       * @struct NodeData interfaceGraph.h <agrum/PRM/gspan/interfaceGraph.h>
       * Inner class to handle data about nodes in  _graph_.
       */
      template < typename GUM_SCALAR >
      class NodeData {
        public:
        /// Constructor.
        NodeData();
        /// Copy Constructor.
        NodeData(const NodeData< GUM_SCALAR >& from);
        /// Destructor.
        ~NodeData();
        /// The instance represented by this node.
        PRMInstance< GUM_SCALAR >* n;
        /// The label of this node.
        LabelData* l;
        /// Equality operator.
        bool operator==(const NodeData< GUM_SCALAR >& from) const;
        /// Difference operator.
        bool operator!=(const NodeData< GUM_SCALAR >& from) const;
      };

      /**
       * Print a NodeData<GUM_SCALAR> in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      template < typename GUM_SCALAR >
      std::ostream& operator<<(std::ostream& out, const NodeData< GUM_SCALAR >& data);

      /**
       * @struct EdgeData interfaceGraph.h <agrum/PRM/gspan/interfaceGraph.h>
       * Inner class to handle data about edges in  _graph_.
       */
      template < typename GUM_SCALAR >
      class EdgeData {
        public:
        /// Constructor.
        EdgeData();
        /// Copy constructor.
        EdgeData(const EdgeData< GUM_SCALAR >& from);
        /// Destructor.
        ~EdgeData();
        /// One of the two instance represented by this edge.
        PRMInstance< GUM_SCALAR >* u;
        /// The label data of u.
        LabelData* l_u;
        /// The other instance represented by thus edge
        PRMInstance< GUM_SCALAR >* v;
        /// The label data of v.
        LabelData* l_v;
        /// The labal data of this edge.
        LabelData* l;
        /// Equality operator.
        bool operator==(const EdgeData< GUM_SCALAR >& from) const;
        /// Difference operator.
        bool operator!=(const EdgeData< GUM_SCALAR >& from) const;
      };

      /**
       * Print a EdgeData<GUM_SCALAR> in out.
       * @param out The stream in which data is printed.
       * @param data The data printed.
       * @return Returns out.
       */
      template < typename GUM_SCALAR >
      std::ostream& operator<<(std::ostream& out, const EdgeData< GUM_SCALAR >& data);

      /**
       * @class InterfaceGraph
       * @headerfile interfaceGraph.h <agrum/PRM/gspan/interfaceGraph.h>
       *
       * @brief This class represent the interface graph of a given
       * gum::prm::PRMSystem<GUM_SCALAR>.
       *
       * An interface graph is a labelled graph over the instances of a
       * gum::prm::PRMSystem<GUM_SCALAR>, where there exists an edge between two
       * instance i and j if and only if their shared interface is nonempty.
       *
       * Labels assigned to edges and nodes in the interface graph are
       * technically strings, however since we need a linear oder each label is
       * assigned a unique id.
       *
       */
      template < typename GUM_SCALAR >
      class InterfaceGraph {
        friend class gum::prm::GSpan< GUM_SCALAR >;

        public:
        /// Default constructor.
        explicit InterfaceGraph(const PRMSystem< GUM_SCALAR >& sys);

        /// Copy constructor, proceeds with a shallow copy so for friends only.
        InterfaceGraph(const InterfaceGraph& source);

        /// Copy operator.
        InterfaceGraph& operator=(const InterfaceGraph& source);

        /// Destructor.
        ~InterfaceGraph();

        /// Returns the graph of this interface graph
        UndiGraph& graph();

        /// Returns the graph of this interface graph
        const UndiGraph& graph() const;

        /// Returns the bijection between LabelData and their string
        /// representation.
        Bijection< Idx, LabelData* >& labels();

        /// Returns the bijection between LabelData and their string
        /// representation.
        const Bijection< Idx, LabelData* >& labels() const;

        /// Returns the number of node or edges labelled by l.
        Size size(const LabelData* l) const;

        /// Returns the set of nodes labelled by l.
        Set< NodeData< GUM_SCALAR >* >& nodes(const LabelData* l);

        /// Returns the set of nodes labelled by l.
        const Set< NodeData< GUM_SCALAR >* >& nodes(const LabelData* l) const;

        /// Returns the set of nodes labelled by l.
        Set< EdgeData< GUM_SCALAR >* >& edges(const LabelData* l);

        /// Returns the set of nodes labelled by l.
        const Set< EdgeData< GUM_SCALAR >* >& edges(const LabelData* l) const;

        /// Returns a label given its id
        LabelData* label(Idx id);

        /// Returns the id of i in this interface graph
        NodeId id(const PRMInstance< GUM_SCALAR >& i) const;

        /// Returns the id of i in this interface graph
        NodeId id(const PRMInstance< GUM_SCALAR >* i) const;

        /// Returns data about a node.
        /// @throw NotFound
        NodeData< GUM_SCALAR >& node(const PRMInstance< GUM_SCALAR >* i);

        /// Returns data about a node.
        /// @throw NotFound
        const NodeData< GUM_SCALAR >& node(const PRMInstance< GUM_SCALAR >* i) const;

        /// Returns data about a node.
        /// @throw NotFound
        NodeData< GUM_SCALAR >& node(NodeId id);

        /// Returns data about a node.
        /// @throw NotFound
        const NodeData< GUM_SCALAR >& node(NodeId id) const;

        /// Returns data about an edge.
        /// @throw NotFound
        EdgeData< GUM_SCALAR >& edge(NodeId u, NodeId v);

        /// Returns data about an edge.
        /// @throw NotFound
        const EdgeData< GUM_SCALAR >& edge(NodeId u, NodeId v) const;

        private:
        /// The gum::prm::PRMSystem<GUM_SCALAR> represented by this interface
        /// graph.
        const PRMSystem< GUM_SCALAR >* _sys_;

        /// The interface graph.
        UndiGraph _graph_;

        /// Data associated with a node in  _graph_.
        NodeProperty< NodeData< GUM_SCALAR >* > _nodes_;

        /// Mapping between PRMInstance<GUM_SCALAR> dans their id in  _graph_.
        HashTable< PRMInstance< GUM_SCALAR >*, NodeId > _idMap_;

        /// Data associated with edges in  _graph_.
        EdgeProperty< EdgeData< GUM_SCALAR >* > _edges_;

        /// Bijection between labels and their ids.
        Bijection< Idx, LabelData* >* _labels_;

        /// Mapping between a LabelData and the set of NodeData<GUM_SCALAR> with
        /// that
        /// label.
        HashTable< LabelData*, Set< NodeData< GUM_SCALAR >* >* > _nodeMap_;

        /// Mapping between a LabelData and the set of EdgeData<GUM_SCALAR> with
        /// that
        /// label.
        HashTable< LabelData*, Set< EdgeData< GUM_SCALAR >* >* > _edgeMap_;

        /// A counter used of assigning ids to labels.
        Idx _counter_;

        /// For shallow copies
        bool _erase_flag_;

        /// Compute the label of node and add it to  _labels_ if it does not
        /// exists yet. Update node with the correct label's id.
        void _label_(NodeData< GUM_SCALAR >* node, HashTable< std::string, LabelData* >& label_map);

        /// Compute the label of edge and add it to  _labels_ if it does not
        /// exists yet. Update edge with the correct label's id.
        void _label_(EdgeData< GUM_SCALAR >* edge, HashTable< std::string, LabelData* >& label_map);
      };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class NodeData< double >;
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class EdgeData< double >;
#    endif
#  endif
#endif
#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#  ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
#    ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
      extern template class InterfaceGraph< double >;
#    endif
#  endif
#endif


    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/gspan/interfaceGraph_tpl.h>

#endif /* GUM_INTERFACE_GRAPH_H */

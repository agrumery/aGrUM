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
 * @brief Inline implementation of InstanceBayesNet.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/instanceBayesNet.h>// to ease IDE parser
// ============================================================================
namespace gum {
namespace prm {

INLINE
InstanceBayesNet::InstanceBayesNet(const Instance& i):
  AbstractBayesNet<prm_float>(), __inst(&i), __moralGraph(0), __topo(0)
{
  GUM_CONSTRUCTOR( InstanceBayesNet );
  __init(i);
}

INLINE
InstanceBayesNet::InstanceBayesNet(const InstanceBayesNet& from):
  AbstractBayesNet<prm_float>(from), __varNodeMap(from.__varNodeMap),
  __inst(from.__inst), __dag(from.__dag), __moralGraph(0), __topo(0)
{
  GUM_CONS_CPY( InstanceBayesNet );
  if (from.__moralGraph != 0) {
    __moralGraph = new UndiGraph(*(from.__moralGraph));
  }
  if (from.__topo != 0) {
    __topo = new Sequence<NodeId>(*(from.__topo));
  }
}

INLINE
InstanceBayesNet::~InstanceBayesNet() {
  GUM_DESTRUCTOR( InstanceBayesNet );
  if (__moralGraph != 0) {
    delete __moralGraph;
  }
  if (__topo != 0) {
    delete __topo;
  }
}

INLINE
InstanceBayesNet&
InstanceBayesNet::operator=(const InstanceBayesNet& from) {
  if (this != &from) {
    AbstractBayesNet<prm_float>::operator=(from);
    if (__moralGraph != 0) {
      delete __moralGraph;
    }
    if (__topo != 0) {
      delete __topo;
    }
    __inst = from.__inst;
    __dag = from.__dag;
    if (from.__moralGraph != 0) {
      __moralGraph = new UndiGraph(*(from.__moralGraph));
    }
    if (from.__topo != 0) {
      __topo = new Sequence<NodeId>(*(from.__topo));
    }
    __varNodeMap = from.__varNodeMap;
  }
  return *this;
}

INLINE
const Potential<prm_float>&
InstanceBayesNet::cpt( NodeId varId ) const {
  return __get(varId).cpf();
}

INLINE
const DAG&
InstanceBayesNet::dag() const {
  return __dag;
}

INLINE
const VariableNodeMap&
InstanceBayesNet::variableNodeMap() const {
  GUM_ERROR(NotFound, "no VariableNodeMap in an InstanceBayesNet");
}

INLINE
Idx
InstanceBayesNet::size() const {
  return __dag.size();
}

INLINE
bool
InstanceBayesNet::empty() const {
  return __dag.empty();
}

INLINE
const DiscreteVariable&
InstanceBayesNet::variable( NodeId id ) const {
  return __get(id).type().variable();
}

INLINE
NodeId
InstanceBayesNet::nodeId( const DiscreteVariable &var ) const {
  return __varNodeMap[&var]->id();
}

INLINE
NodeId
InstanceBayesNet::idFromName( const std::string& name ) const {
  return __get(name).id();
}

INLINE
const DiscreteVariable&
InstanceBayesNet::variableFromName( const std::string& name ) const {
  return __get(name).type().variable();
}

INLINE
const UndiGraph&
InstanceBayesNet::moralGraph( bool clear ) const {
  if (__moralGraph == 0) {
    __moralGraph = new UndiGraph();
    AbstractBayesNet<prm_float>::_moralGraph(*__moralGraph);
  }
  return *__moralGraph;
}

INLINE
const Sequence<NodeId>&
InstanceBayesNet::getTopologicalOrder( bool clear ) const {
  if (__topo == 0) {
    __topo = new Sequence<NodeId>();
    AbstractBayesNet<prm_float>::_topologicalOrder(*__topo);
  }
  return *__topo;
}

INLINE
const ClassElement&
InstanceBayesNet::__get(NodeId id) const {
  return __inst->get(id);
}

INLINE
const ClassElement&
InstanceBayesNet::__get(const std::string& name) const {
  try {
    return __inst->get(name);
  } catch (NotFound&) {
    GUM_ERROR(NotFound, "no element found with that name");
  }
}

INLINE
const Property<unsigned int>::onNodes&
InstanceBayesNet::modalities() const {
  if (__modalities.empty()) {
    for (DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node) {
      __modalities.insert(*node, (unsigned int) variable(*node).domainSize());
    }
  }
  return __modalities;
}

INLINE
std::string
InstanceBayesNet::toDot() const {
  std::string tab = "  ";
  std::stringstream output;
  output << "digraph \"";
  output << __inst->name() << "\" {" << std::endl;
  for (DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
    if ( dag().children( *node_iter ).size() > 0 ) {
      const NodeSet& children = dag().children( *node_iter );
      for (NodeSetIterator arc_iter = children.begin();
           arc_iter != children.end(); ++arc_iter ) {
        output << tab << "\"" << variable( *node_iter ).name() << "\" -> ";
        output << "\"" << variable( *arc_iter ).name() << "\";" << std::endl;
      }
    } else if ( dag().parents( *node_iter ).size() == 0 ) {
      output << tab << "\"" << variable( *node_iter ).name() << "\";" << std::endl;
    }
  }
  output << "}" << std::endl;
  return output.str();
}

} /* namespace prm */
} /* namespace gum */
// ============================================================================

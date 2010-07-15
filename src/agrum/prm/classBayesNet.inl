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
 * @brief Inline implementation of ClassBayesNet.
 *
 * @author Lionel TORTI
 */
// ============================================================================
#include <agrum/prm/classBayesNet.h>// to ease IDE parser
// ============================================================================
namespace gum {
namespace prm {

INLINE
ClassBayesNet::ClassBayesNet(const Class& c):
  AbstractBayesNet<prm_float>(), __class(&c), __moralGraph(0), __topo(0)
{
  GUM_CONSTRUCTOR( ClassBayesNet );
  __init(c);
}

INLINE
ClassBayesNet::ClassBayesNet(const ClassBayesNet& from):
  AbstractBayesNet<prm_float>(from), __class(from.__class), __dag(from.__dag),
  __moralGraph(0), __topo(0)
{
  GUM_CONS_CPY( ClassBayesNet );
  if (from.__moralGraph != 0) {
    __moralGraph = new UndiGraph(*(from.__moralGraph));
  }
  if (from.__topo != 0) {
    __topo = new Sequence<NodeId>(*(from.__topo));
  }
}

INLINE
ClassBayesNet::~ClassBayesNet() {
  GUM_DESTRUCTOR( ClassBayesNet );
  if (__moralGraph != 0) {
    delete __moralGraph;
  }
  if (__topo != 0) {
    delete __topo;
  }
}

INLINE
ClassBayesNet&
ClassBayesNet::operator=(const ClassBayesNet& from) {
  if (this != &from) {
    AbstractBayesNet<prm_float>::operator=(from);
    if (__moralGraph != 0) {
      delete __moralGraph;
    }
    if (__topo != 0) {
      delete __topo;
    }
    __class = from.__class;
    __dag = from.__dag;
    if (from.__moralGraph != 0) {
      __moralGraph = new UndiGraph(*(from.__moralGraph));
    }
    if (from.__topo != 0) {
      __topo = new Sequence<NodeId>(*(from.__topo));
    }
  }
  return *this;
}

INLINE
const Potential<prm_float>&
ClassBayesNet::cpt( NodeId varId ) const {
  return __get(varId).cpf();
}

INLINE
const DAG&
ClassBayesNet::dag() const {
  return __dag;
}

INLINE
const VariableNodeMap&
ClassBayesNet::variableNodeMap() const {
  GUM_ERROR(FatalError, "Sorry no VarMap in a ClassBayesNet.");
}

INLINE
Idx
ClassBayesNet::size() const {
  return __dag.size();
}

INLINE
bool
ClassBayesNet::empty() const {
  return __dag.empty();
}

INLINE
const DiscreteVariable&
ClassBayesNet::variable( NodeId id ) const {
  return __get(id).type().variable();
}

INLINE
NodeId
ClassBayesNet::nodeId( const DiscreteVariable &var ) const {
  return __varNodeMap.get(var);
}

INLINE
NodeId
ClassBayesNet::idFromName( const std::string& name ) const {
  return __varNodeMap.idFromName(name);
}

INLINE
const DiscreteVariable&
ClassBayesNet::variableFromName( const std::string& name ) const {
  return __varNodeMap.variableFromName(name);
}

INLINE
const UndiGraph&
ClassBayesNet::moralGraph( bool clear ) const {
  if (__moralGraph == 0) {
    __moralGraph = new UndiGraph();
    AbstractBayesNet<prm_float>::_moralGraph(*__moralGraph);
  }
  return *__moralGraph;
}

INLINE
const Sequence<NodeId>&
ClassBayesNet::getTopologicalOrder( bool clear ) const {
  if (__topo == 0) {
    __topo = new Sequence<NodeId>();
    AbstractBayesNet<prm_float>::_topologicalOrder(*__topo);
  }
  return *__topo;
}

INLINE
const ClassElement&
ClassBayesNet::__get(NodeId id) const {
  if (__dag.exists(id)) {
    return __class->get(__varNodeMap.name(id));
  } else {
    GUM_ERROR(NotFound, "no element found with that id.");
  }
}

INLINE
const Property<unsigned int>::onNodes&
ClassBayesNet::modalities() const {
  if (__modalities.empty()) {
    for (DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node) {
      __modalities.insert(*node, (unsigned int) variable(*node).domainSize());
    }
  }
  return __modalities;
}

INLINE
std::string
ClassBayesNet::toDot() const {
  std::string tab = "  ";
  std::stringstream output;
  output << "digraph \"";
  output << __class->name() << "\" {" << std::endl;
  for (DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
    if ( dag().children( *node_iter ).size() > 0 ) {
      for (DAG::ArcIterator arc_iter = dag().children( *node_iter ).begin(); arc_iter != dag().children( *node_iter ).end(); ++arc_iter ) {
        output << tab << "\"" << variable( *node_iter ).name() << "\" -> ";
        output << "\"" << variable( arc_iter->head() ).name() << "\";" << std::endl;
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

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
 * @brief Inline implementation of HollowBayesNet.
 *
 * @author Lionel TORTI
 */
// ============================================================================
namespace gum {
namespace prm {

INLINE
HollowBayesNet::~HollowBayesNet() {
  GUM_DESTRUCTOR( HollowBayesNet );
  if (__moralGraph != 0) {
    delete __moralGraph;
  }
  if (__topo != 0) {
    delete __topo;
  }
}

INLINE
const Potential<prm_float>&
HollowBayesNet::cpt( NodeId varId ) const {
  return __get(varId).cpf();
}

INLINE
const DAG&
HollowBayesNet::dag() const {
  return __dag;
}

INLINE
const VariableNodeMap&
HollowBayesNet::variableNodeMap() const {
  GUM_ERROR(FatalError, "Sorry no VarMap in a HollowBayesNet.");
}

INLINE
Idx
HollowBayesNet::size() const {
  return __dag.size();
}

INLINE
bool
HollowBayesNet::empty() const {
  return __dag.empty();
}

INLINE
const DiscreteVariable&
HollowBayesNet::variable( NodeId id ) const {
  return __cec->get(id).type().variable();
}

INLINE
NodeId
HollowBayesNet::nodeId( const DiscreteVariable &var ) const {
  return __varNodeMap[&var];
}

INLINE
NodeId
HollowBayesNet::idFromName( const std::string& name ) const {
  if (__varNodeMap.exists(&(__cec->get(name).type().variable()))) {
    return __cec->get(name).id();
  } else {
    GUM_ERROR(NotFound, "no element found with that id.");
  }
}

INLINE
const DiscreteVariable&
HollowBayesNet::variableFromName( const std::string& name ) const {
  if (__varNodeMap.exists(&(__cec->get(name).type().variable()))) {
    return __cec->get(name).type().variable();
  } else {
    GUM_ERROR(NotFound, "no element found with that id.");
  }
}

INLINE
const UndiGraph&
HollowBayesNet::moralGraph( bool clear ) const {
  if (__moralGraph == 0) {
    __moralGraph = new UndiGraph();
    AbstractBayesNet<prm_float>::_moralGraph(*__moralGraph);
  }
  return *__moralGraph;
}

INLINE
const Sequence<NodeId>&
HollowBayesNet::getTopologicalOrder( bool clear ) const {
  if (__topo == 0) {
    __topo = new Sequence<NodeId>();
    AbstractBayesNet<prm_float>::_topologicalOrder(*__topo);
  }
  return *__topo;
}

INLINE
const ClassElement&
HollowBayesNet::__get(NodeId id) const {
  if (__dag.exists(id)) {
    return __cec->get(id);
  } else {
    GUM_ERROR(NotFound, "no element found with that id.");
  }
}

INLINE
const Property<unsigned int>::onNodes&
HollowBayesNet::modalities() const {
  return __modalities;
}

INLINE std::string
HollowBayesNet::toDot( void ) const {
  std::stringstream output;
  output << "digraph \"";
  try {
    output << this->property( "name" ) << "\" {" << std::endl;
  } catch ( NotFound& ) {
    output << "no_name\" {" << std::endl;
  }
  std::string tab = "  ";
  for ( gum::DAG::NodeIterator node_iter = dag().beginNodes();
  node_iter != dag().endNodes(); ++node_iter ) {
    if ( dag().children( *node_iter ).size() > 0 ) {
      for ( gum::DAG::ArcIterator arc_iter = dag().children( *node_iter ).begin();
      arc_iter != dag().children( *node_iter ).end(); ++arc_iter ) {
        output << tab << "\"" << variable( *node_iter ).name() << "\" -> "
        << "\"" << variable( arc_iter->head() ).name() << "\";" << std::endl;
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

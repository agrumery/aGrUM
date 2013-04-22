/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Template implementation of bns/bayesNet.h classes.
 *
 * @author Pierre-Henri Wuillemin and Lionel Torti
 */
// ============================================================================
#include <limits>

#include <agrum/BN/abstractBayesNet.h>
#include <agrum/BN/BayesNet.h>

#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>

// ============================================================================

namespace gum {
// ============================================================================
//                                  BayesNet
// ============================================================================

  template<typename GUM_SCALAR> INLINE
  BayesNet<GUM_SCALAR>::BayesNet() :
    AbstractBayesNet<GUM_SCALAR>(),
    __moralGraph ( 0 ), __topologicalOrder ( 0 ) {
    GUM_CONSTRUCTOR ( BayesNet );
    __topologicalOrder = new Sequence<NodeId>();
    __moralGraph = new UndiGraph();
  }
  
  template<typename GUM_SCALAR> INLINE
  BayesNet<GUM_SCALAR>::BayesNet(std::string name) :
    AbstractBayesNet<GUM_SCALAR>(),
    __moralGraph ( 0 ), __topologicalOrder ( 0 ) {
    GUM_CONSTRUCTOR ( BayesNet );
    __topologicalOrder = new Sequence<NodeId>();
    __moralGraph = new UndiGraph();
    this->setProperty("name", name);
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>::BayesNet ( const BayesNet<GUM_SCALAR>& source ) :
    AbstractBayesNet<GUM_SCALAR> ( source ),
    __dag ( source.__dag ),
    __varMap ( source.__varMap ),
    __moralGraph ( 0 ),
    __topologicalOrder ( 0 ) {
    GUM_CONS_CPY ( BayesNet );

    __moralGraph = new UndiGraph ( * ( source.__moralGraph ) );
    __topologicalOrder = new Sequence<NodeId> ( * ( source.__topologicalOrder ) );

    __copyPotentials ( source );
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>&
  BayesNet<GUM_SCALAR>::operator= ( const BayesNet<GUM_SCALAR>& source ) {
    if ( this != &source ) {
      AbstractBayesNet<GUM_SCALAR>::operator= ( source );
      __dag = source.dag();
      __varMap = source.__varMap;

      ( *__topologicalOrder ) = * ( source.__topologicalOrder );
      ( *__moralGraph ) = * ( source.__moralGraph );

      __clearPotentials();
      __copyPotentials ( source );
    }

    return *this;
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>::~BayesNet() {
    GUM_DESTRUCTOR ( BayesNet );

    for ( HashTableConstIterator<NodeId, Potential<GUM_SCALAR>*> iter = __probaMap.begin();
          iter != __probaMap.end();
          ++iter ) {
      delete *iter;
    }

    delete __moralGraph;
    delete __topologicalOrder;
  }

  template<typename GUM_SCALAR> INLINE
  const DiscreteVariable&  BayesNet<GUM_SCALAR>::variable ( NodeId id ) const {
    return __varMap.get ( id );
  }

  template<typename GUM_SCALAR> INLINE
  void BayesNet<GUM_SCALAR>::changeVariableName ( NodeId id, const std::string& new_name ) {
    __varMap.changeName ( id, new_name );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId BayesNet<GUM_SCALAR>::nodeId ( const DiscreteVariable& var ) const {
    return __varMap.get ( var );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var ) {
    MultiDimArray<GUM_SCALAR>* ptr = new MultiDimArray<GUM_SCALAR>();
    NodeId res = 0;

    try {
      res = add ( var, ptr );

    } catch ( Exception& e ) {
      delete ptr;
      throw;
    }

    return res;
  }

  template<typename GUM_SCALAR>
  NodeId
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var, MultiDimImplementation<GUM_SCALAR> *aContent ) {
    NodeId proposedId = __dag.nextNodeId();

    __varMap.insert ( proposedId, var );
    __dag.insertNode ( proposedId );

    Potential<GUM_SCALAR> *cpt = new Potential<GUM_SCALAR> ( aContent );
    ( *cpt ) << variable ( proposedId );
    __probaMap.insert ( proposedId, cpt );
    return proposedId;
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var , NodeId id ) {
    MultiDimArray<GUM_SCALAR>* ptr = new MultiDimArray<GUM_SCALAR>();
    NodeId res = 0;

    try {
      res = add ( var, ptr, id );

    } catch ( Exception& e ) {
      delete ptr;
      throw;
    }

    return res;
  }

  template<typename GUM_SCALAR>
  NodeId
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var, MultiDimImplementation<GUM_SCALAR> *aContent , NodeId id ) {
    __varMap.insert ( id, var );
    __dag.insertNode ( id );

    Potential<GUM_SCALAR> *cpt = new Potential<GUM_SCALAR> ( aContent );
    ( *cpt ) << variable ( id );
    __probaMap.insert ( id, cpt );
    return id;
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addVariable ( const DiscreteVariable& var ) {
    return add ( var );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addVariable ( const DiscreteVariable& var,
                                      MultiDimImplementation<GUM_SCALAR> *aContent ) {
    return add ( var, aContent );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId BayesNet<GUM_SCALAR>::idFromName ( const std::string& name ) const {
    return __varMap.idFromName ( name );
  }

  template<typename GUM_SCALAR> INLINE
  const DiscreteVariable& BayesNet<GUM_SCALAR>::variableFromName ( const std::string& name ) const {
    return __varMap.variableFromName ( name );
  }

  template<typename GUM_SCALAR> INLINE
  const Potential<GUM_SCALAR>&
  BayesNet<GUM_SCALAR>::cpt ( NodeId varId ) const {
    return * ( __probaMap[varId] );
  }

  template<typename GUM_SCALAR> INLINE
  const DAG&
  BayesNet<GUM_SCALAR>::dag() const {
    return __dag;
  }

  template<typename GUM_SCALAR> INLINE
  const VariableNodeMap&
  BayesNet<GUM_SCALAR>::variableNodeMap() const {
    return __varMap;
  }

  template<typename GUM_SCALAR> INLINE
  Idx
  BayesNet<GUM_SCALAR>::size() const {
    return __dag.size();
  }

  template<typename GUM_SCALAR> INLINE
  Idx
  BayesNet<GUM_SCALAR>::nbrArcs() const {
    return __dag.sizeArcs();
  }

  template<typename GUM_SCALAR>
  Idx
  BayesNet<GUM_SCALAR>::dim() const {
    Idx dim = 0;

    for ( DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node ) {
      Idx q = 1;
      Set<NodeId> s = __dag.parents ( *node );

      for ( Set<NodeId>::iterator parent = s.begin(); parent != s.end(); ++parent ) {
        q *= variable ( *parent ).domainSize();
      }

      dim += ( variable ( *node ).domainSize() - 1 ) * q;
    }

    return dim;
  }

  template<typename GUM_SCALAR> INLINE
  bool
  BayesNet<GUM_SCALAR>::empty() const {
    return size() == 0;
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::eraseVariable ( NodeId varId ) {
    erase ( varId );
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::erase ( const DiscreteVariable& var ) {
    erase ( __varMap.get ( var ) );
  }

  template<typename GUM_SCALAR>
  void
  BayesNet<GUM_SCALAR>::erase ( NodeId varId ) {
    if ( __varMap.exists ( varId ) ) {
      // Reduce the variable child's CPT
      const NodeSet& children = __dag.children ( varId );

      for ( NodeSetIterator iter = children.begin();
            iter != children.end(); ++iter ) {
        __probaMap[ *iter ]->erase ( variable ( varId ) );
      }

      delete __probaMap[varId];

      __probaMap.erase ( varId );
      __varMap.erase ( varId );
      __dag.eraseNode ( varId );
    }
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::insertArc ( NodeId tail, NodeId head ) {
    __dag.insertArc ( tail, head );
    // Add parent in the child's CPT
    ( * ( __probaMap[head] ) ) << variable ( tail );
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::eraseArc ( const Arc& arc ) {
    if ( __varMap.exists ( arc.tail() ) && __varMap.exists ( arc.head() ) ) {
      NodeId head = arc.head(), tail = arc.tail();
      __dag.eraseArc ( arc );
      // Remove parent froms child's CPT
      ( * ( __probaMap[head] ) ) >> variable ( tail );
    }
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::eraseArc ( NodeId tail, NodeId head ) {
    eraseArc ( Arc ( tail, head ) );
  }

  template<typename GUM_SCALAR>
  const UndiGraph&
  BayesNet<GUM_SCALAR>::moralGraph ( bool clear ) const {
    if ( clear or __moralGraph == 0 ) {
      if ( __moralGraph != 0 ) {
        delete __moralGraph;
      }

      __moralGraph = new UndiGraph();

      AbstractBayesNet<GUM_SCALAR>::_moralGraph ( *__moralGraph );
    }

    return *__moralGraph;
  }

  template<typename GUM_SCALAR>
  const Sequence<NodeId>&
  BayesNet<GUM_SCALAR>::topologicalOrder ( bool clear ) const {
    if ( clear or ( __topologicalOrder->empty() ) ) {
      __topologicalOrder->clear();
      AbstractBayesNet<GUM_SCALAR>::_topologicalOrder ( *__topologicalOrder );
    }

    return *__topologicalOrder;
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyOR ( const DiscreteVariable& var , GUM_SCALAR external_weight ) {
    return addNoisyORCompound ( var, external_weight );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyORCompound ( const DiscreteVariable& var , GUM_SCALAR external_weight ) {
    return add ( var, new MultiDimNoisyORCompound<GUM_SCALAR> ( external_weight ) );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addOR ( const DiscreteVariable& var ) {
    if ( var.domainSize() > 2 ) GUM_ERROR ( SizeError, "an OR has to be boolean" );

    return add ( var, new aggregator::Or<GUM_SCALAR> () );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addAND ( const DiscreteVariable& var ) {
    if ( var.domainSize() > 2 ) GUM_ERROR ( SizeError, "an AND has to be boolean" );

    return add ( var, new aggregator::And<GUM_SCALAR> () );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyORNet ( const DiscreteVariable& var , GUM_SCALAR external_weight ) {
    return add ( var, new MultiDimNoisyORNet<GUM_SCALAR> ( external_weight ) );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyOR ( const DiscreteVariable& var , GUM_SCALAR external_weight, NodeId id ) {
    return addNoisyORCompound ( var, external_weight, id );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyORCompound ( const DiscreteVariable& var , GUM_SCALAR external_weight, NodeId id ) {
    return add ( var, new MultiDimNoisyORCompound<GUM_SCALAR> ( external_weight ) , id );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyORNet ( const DiscreteVariable& var , GUM_SCALAR external_weight, NodeId id ) {
    return add ( var, new MultiDimNoisyORNet<GUM_SCALAR> ( external_weight ) , id );
  }

  template<typename GUM_SCALAR>
  void
  BayesNet<GUM_SCALAR>::insertWeightedArc ( NodeId tail, NodeId head, GUM_SCALAR causalWeight ) {
    const MultiDimAdressable& content = cpt ( head ).getMasterRef();

    const MultiDimCIModel<GUM_SCALAR>* CImodel = dynamic_cast<const MultiDimCIModel<GUM_SCALAR>*> ( &content );

    if ( CImodel != 0 ) {
      // or is OK
      insertArc ( tail, head );

      CImodel->causalWeight ( variable ( tail ), causalWeight );
      return;
    }

    GUM_ERROR ( InvalidArc, "Head variable (" << variable ( head ).name() << ") is not a CIModel variable !" );
  }

  template<typename GUM_SCALAR> INLINE
  std::ostream&
  operator<< ( std::ostream& output, const AbstractBayesNet<GUM_SCALAR>& map ) {
    output << map.toString();
    return output;
  }


  template<typename GUM_SCALAR>
  std::string
  BayesNet<GUM_SCALAR>::toDot ( void ) const {
    std::stringstream output;
    output << "digraph \"";

    std::string bn_name;

    try {
      bn_name = this->property ( "name" );
    } catch ( NotFound& ) {
      bn_name = "no_name";
    }


    output << bn_name << "\" {" << std::endl;
    output << "  graph [bgcolor=transparent,label=\"" << bn_name << "\"];" << std::endl;
    output << "  node [style=filled fillcolor=\"#ffffaa\"];" << std::endl << std::endl;

    for ( DAG::NodeIterator node_iter = dag().beginNodes();
          node_iter != dag().endNodes(); ++node_iter ) {
      output << "\"" << variable ( *node_iter ).name() << "\" [comment=\"" << *node_iter << ":" << variable ( *node_iter ) << "\"];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    for ( DAG::NodeIterator node_iter = dag().beginNodes();
          node_iter != dag().endNodes(); ++node_iter ) {
      if ( dag().children ( *node_iter ).size() > 0 ) {
        const NodeSet& children =  dag().children ( *node_iter );

        for ( NodeSetIterator arc_iter = children.begin();
              arc_iter != children.end(); ++arc_iter ) {
          output << tab << "\"" << variable ( *node_iter ).name() << "\" -> "
                 << "\"" << variable ( *arc_iter ).name() << "\";" << std::endl;
        }

      } else if ( dag().parents ( *node_iter ).size() == 0 ) {
        output << tab << "\"" << variable ( *node_iter ).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }


  /// Compute a parameter of the joint probability for the BN (given an instantiation of the vars)
  template<typename GUM_SCALAR>
  GUM_SCALAR BayesNet<GUM_SCALAR>::jointProbability ( const Instantiation& i ) const {
    GUM_SCALAR value = ( GUM_SCALAR ) 1.0;

    GUM_SCALAR tmp;

    for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
      if ( ( tmp = cpt ( *node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
        return ( GUM_SCALAR ) 0;
      }

      value *= tmp;
    }

    return value;
  }

  /// Compute a parameter of the joint probability for the BN (given an instantiation of the vars)
  template<typename GUM_SCALAR>
  GUM_SCALAR BayesNet<GUM_SCALAR>::logJointProbability ( const Instantiation& i ) const {
    GUM_SCALAR value = ( GUM_SCALAR ) 0.0;

    GUM_SCALAR tmp;

    for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
      if ( ( tmp = cpt ( *node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
        return ( GUM_SCALAR ) ( - std::numeric_limits<double>::infinity( ) );
      }

      value += log2 ( cpt ( *node_iter ) [i] );
    }

    return value;
  }

  /// begin Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::beginTopologyTransformation() {
    for ( DAG::NodeIterator node_iter = dag().beginNodes();
          node_iter != dag().endNodes(); ++node_iter ) {
      __probaMap[*node_iter]->beginMultipleChanges();
    }
  }

  /// end Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::endTopologyTransformation() {
    for ( DAG::NodeIterator node_iter = dag().beginNodes();
          node_iter != dag().endNodes(); ++node_iter ) {
      __probaMap[*node_iter]->endMultipleChanges();
    }
  }

  /// clear all potentials
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::__clearPotentials() {
    // Removing previous potentials
    for ( HashTableConstIterator< NodeId, Potential<GUM_SCALAR>* > iter = __probaMap.begin(); iter != __probaMap.end(); ++iter ) {
      delete *iter;
    }

    __probaMap.clear();
  }


  /// copy of potentials from a BN to another, using names of vars as ref.
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::__copyPotentials ( const BayesNet<GUM_SCALAR>& source ) {
    // Copying potentials
    typedef HashTableConstIterator<NodeId, Potential<GUM_SCALAR>*> PotIterator;
    Potential<GUM_SCALAR> *copy_array = 0;

    for ( PotIterator srcIter = source.__probaMap.begin(); srcIter != source.__probaMap.end(); ++srcIter ) {
      // First we build the node's CPT
      copy_array = new Potential<GUM_SCALAR>();

      for ( gum::Idx i = 0; i < ( *srcIter )->nbrDim(); i++ ) {
        ( *copy_array ) << variableFromName ( ( *srcIter )->variable ( i ).name() );
      }


      copy_array->copyFrom ( **srcIter );

      // We add the CPT to the CPT's hashmap
      __probaMap.insert ( srcIter.key(), copy_array );
    }
  }


  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::generateCPTs() {
    DefaultCPTGenerator<GUM_SCALAR> generator;

    for ( DAG::NodeIterator iter = this->beginNodes(); iter != this->endNodes(); ++iter ) {
      generator.generateCPT ( cpt ( *iter ).pos ( variable ( *iter ) ),  cpt ( *iter ) );
    }
  }
} /* namespace gum */

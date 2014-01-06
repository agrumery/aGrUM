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
 * @brief Template implementation of BN/BayesNet.h class.
 *
 * @author Pierre-Henri WUILLEMIN and Lionel TORTI
 */

#include <limits>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/IBayesNet.h>

#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>

#include <agrum/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/ICIModels/multiDimNoisyORCompound.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>


namespace gum {

  template<typename GUM_SCALAR> INLINE
  BayesNet<GUM_SCALAR>::BayesNet() :
    IBayesNet<GUM_SCALAR>() {
    GUM_CONSTRUCTOR ( BayesNet );
  }

  template<typename GUM_SCALAR> INLINE
  BayesNet<GUM_SCALAR>::BayesNet ( std::string name ) :
    IBayesNet<GUM_SCALAR> ( name ) {
    GUM_CONSTRUCTOR ( BayesNet );
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>::BayesNet ( const BayesNet<GUM_SCALAR>& source ) :
    IBayesNet<GUM_SCALAR> ( source ),
    __varMap ( source.__varMap ) {
    GUM_CONS_CPY ( BayesNet );

    __copyPotentials ( source );
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>&
  BayesNet<GUM_SCALAR>::operator= ( const BayesNet<GUM_SCALAR>& source ) {
    if ( this != &source ) {
      IBayesNet<GUM_SCALAR>::operator= ( source );
      __varMap = source.__varMap;

      __clearPotentials();
      __copyPotentials ( source );
    }

    return *this;
  }

  template<typename GUM_SCALAR>
  BayesNet<GUM_SCALAR>::~BayesNet() {
    GUM_DESTRUCTOR ( BayesNet );

    for ( HashTableConstIteratorSafe<NodeId, Potential<GUM_SCALAR>*> iter = __probaMap.beginSafe();
          iter != __probaMap.endSafe();
          ++iter ) {
      delete iter.val ();
    }
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

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var, MultiDimImplementation<GUM_SCALAR>* aContent ) {
    NodeId proposedId = dag().nextNodeId();
    NodeId res = 0;

    res = add ( var, aContent, proposedId );

    return res;
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
  BayesNet<GUM_SCALAR>::add ( const DiscreteVariable& var, MultiDimImplementation<GUM_SCALAR>* aContent , NodeId id ) {
    __varMap.insert ( id, var );
    this->_dag.insertNode ( id );

    Potential<GUM_SCALAR>* cpt = new Potential<GUM_SCALAR> ( aContent );
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
                                      MultiDimImplementation<GUM_SCALAR>* aContent ) {
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
  const VariableNodeMap&
  BayesNet<GUM_SCALAR>::variableNodeMap() const {
    return __varMap;
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
      const NodeSet& children = dag().children ( varId );

      for ( NodeSetIterator iter = children.begin();
            iter != children.end(); ++iter ) {
        __probaMap[ *iter ]->erase ( variable ( varId ) );
      }

      delete __probaMap[varId];

      __probaMap.erase ( varId );
      __varMap.erase ( varId );
      this->_dag.eraseNode ( varId );
    }
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::insertArc ( NodeId tail, NodeId head ) {
    addArc ( tail, head );
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::addArc ( NodeId tail, NodeId head ) {
    this->_dag.insertArc ( tail, head );
    // Add parent in the child's CPT
    ( * ( __probaMap[head] ) ) << variable ( tail );
  }

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::eraseArc ( const Arc& arc ) {
    if ( __varMap.exists ( arc.tail() ) && __varMap.exists ( arc.head() ) ) {
      NodeId head = arc.head(), tail = arc.tail();
      this->_dag.eraseArc ( arc );
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
  void
  BayesNet<GUM_SCALAR>::reverseArc ( const Arc& arc ) {
    // check that the arc exsists
    if ( ! __varMap.exists ( arc.tail() ) || ! __varMap.exists ( arc.head() )
         || ! dag().existsArc ( arc ) ) {
      GUM_ERROR ( InvalidArc, "a nonexisting arc cannot be reversed" );
    }
    NodeId tail = arc.tail (), head = arc.head ();

    // check that the reversal does not induce a cycle
    try {
      DAG d = dag ();
      d.eraseArc ( arc );
      d.insertArc ( head, tail );
    }
    catch ( Exception& e ) {
      GUM_ERROR ( InvalidArc, "this arc reversal would induce a directed cycle" );
    }
    
    // with the same notations as Shachter (1986), "evaluating influence diagrams",
    // p.878, we shall first compute the product of probabilities:
    // pi_j^old (x_j | x_c^old(j) ) * pi_i^old (x_i | x_c^old(i) )
    Potential<GUM_SCALAR> prod { cpt ( tail ) * cpt ( head ) };

    // modify the topology of the graph: add to tail all the parents of head
    // and add to head all the parents of tail
    beginTopologyTransformation ();
    NodeSet new_parents = dag().parents ( tail ) + dag().parents ( head );
    // remove arc (head, tail)
    eraseArc ( arc );
    // add the necessary arcs to the tail
    for ( NodeSet::const_iterator iter = new_parents.begin ();
          iter != new_parents.end (); ++iter ) {
      if ( ( *iter != tail ) && ! dag().existsArc ( *iter, tail ) ) {
        addArc ( *iter, tail );
      }
    }
    addArc ( head, tail );
    // add the necessary arcs to the head
    new_parents.erase ( tail );
     for ( NodeSet::const_iterator iter = new_parents.begin ();
          iter != new_parents.end (); ++iter ) {
      if ( ( *iter != head ) && ! dag().existsArc ( *iter, head ) ) {
        addArc ( *iter, head );
      }
    }
    endTopologyTransformation ();

    // update the conditional distributions of head and tail
    Set<const DiscreteVariable*> del_vars;
    del_vars << &(variable (tail));
    Potential<GUM_SCALAR> new_cpt_head { projectSum ( prod, del_vars ) };
    Potential<GUM_SCALAR>& cpt_head =
      const_cast<Potential<GUM_SCALAR>&> ( cpt ( head ) );
    cpt_head = new_cpt_head;

    Potential<GUM_SCALAR> new_cpt_tail { prod / cpt_head };
    Potential<GUM_SCALAR>& cpt_tail =
      const_cast<Potential<GUM_SCALAR>&> ( cpt ( tail ) );
    cpt_tail = new_cpt_tail;
  }
  

  template<typename GUM_SCALAR> INLINE
  void
  BayesNet<GUM_SCALAR>::reverseArc ( NodeId tail, NodeId head ) {
    reverseArc ( Arc ( tail, head ) );
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
  BayesNet<GUM_SCALAR>::addNoisyORNet ( const DiscreteVariable& var , GUM_SCALAR external_weight ) {
    return add ( var, new MultiDimNoisyORNet<GUM_SCALAR> ( external_weight ) );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyAND ( const DiscreteVariable& var , GUM_SCALAR external_weight ) {
    return add ( var, new MultiDimNoisyAND<GUM_SCALAR> ( external_weight ) );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyOR ( const DiscreteVariable& var , GUM_SCALAR external_weight, NodeId id ) {
    return addNoisyORCompound ( var, external_weight, id );
  }

  template<typename GUM_SCALAR> INLINE
  NodeId
  BayesNet<GUM_SCALAR>::addNoisyAND ( const DiscreteVariable& var , GUM_SCALAR external_weight, NodeId id ) {
    return add ( var, new MultiDimNoisyAND<GUM_SCALAR> ( external_weight ) , id );
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
    addWeightedArc ( tail, head, causalWeight );
  }

  template<typename GUM_SCALAR>
  void
  BayesNet<GUM_SCALAR>::addWeightedArc ( NodeId tail, NodeId head, GUM_SCALAR causalWeight ) {
    const MultiDimAdressable& content = cpt ( head ).getMasterRef();

    const MultiDimICIModel<GUM_SCALAR>* CImodel = dynamic_cast<const MultiDimICIModel<GUM_SCALAR>*> ( &content );

    if ( CImodel != 0 ) {
      // or is OK
      addArc ( tail, head );

      CImodel->causalWeight ( variable ( tail ), causalWeight );
    } else {
      GUM_ERROR ( InvalidArc, "Head variable (" << variable ( head ).name() << ") is not a CIModel variable !" );
    }
  }

  template<typename GUM_SCALAR> INLINE
  std::ostream&
  operator<< ( std::ostream& output, const BayesNet<GUM_SCALAR>& bn ) {
    output << bn.toString();
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

    for ( const auto node_iter : nodes() ) {
      output << "\"" << variable ( node_iter ).name() << "\" [comment=\"" << node_iter << ":" << variable ( node_iter ) << "\"];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    for ( const auto node_iter : nodes() ) {
      if ( dag().children ( node_iter ).size() > 0 ) {
        const NodeSet& children =  dag().children ( node_iter );

        for ( NodeSetIterator arc_iter = children.begin();
              arc_iter != children.end(); ++arc_iter ) {
          output << tab << "\"" << variable ( node_iter ).name() << "\" -> "
                 << "\"" << variable ( *arc_iter ).name() << "\";" << std::endl;
        }

      } else if ( dag().parents ( node_iter ).size() == 0 ) {
        output << tab << "\"" << variable ( node_iter ).name() << "\";" << std::endl;
      }
    }

    output << "}" << std::endl;

    return output.str();
  }

  /// begin Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::beginTopologyTransformation() {
    for ( const auto node_iter : nodes() )
      __probaMap[node_iter]->beginMultipleChanges();
  }

  /// end Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::endTopologyTransformation() {
    for ( const auto node_iter : nodes() )
      __probaMap[node_iter]->endMultipleChanges();
  }

  /// clear all potentials
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::__clearPotentials() {
    // Removing previous potentials
    for ( HashTableConstIteratorSafe< NodeId, Potential<GUM_SCALAR>* > iter = __probaMap.beginSafe(); iter != __probaMap.endSafe(); ++iter ) {
      delete iter.val();
    }

    __probaMap.clear();
  }


  /// copy of potentials from a BN to another, using names of vars as ref.
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::__copyPotentials ( const BayesNet<GUM_SCALAR>& source ) {
    // Copying potentials
    typedef HashTableConstIteratorSafe<NodeId, Potential<GUM_SCALAR>*> PotIterator;
    Potential<GUM_SCALAR>* copy_array = 0;

    for ( PotIterator srcIter = source.__probaMap.beginSafe(); srcIter != source.__probaMap.endSafe(); ++srcIter ) {
      // First we build the node's CPT
      copy_array = new Potential<GUM_SCALAR>();

      for ( gum::Idx i = 0; i < ( srcIter.val() )->nbrDim(); i++ ) {
        ( *copy_array ) << variableFromName ( ( srcIter.val() )->variable ( i ).name() );
      }


      copy_array->copyFrom ( *( srcIter.val() ) );

      // We add the CPT to the CPT's hashmap
      __probaMap.insert ( srcIter.key(), copy_array );
    }
  }


  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::generateCPTs() {
    SimpleCPTGenerator<GUM_SCALAR> generator;

    for ( const auto iter : nodes() ) {
      generator.generateCPT ( cpt ( iter ).pos ( variable ( iter ) ),  cpt ( iter ) );
    }
  }



  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::changePotential ( NodeId id, Potential<GUM_SCALAR>* newPot ) {
    if ( cpt ( id ).nbrDim() != newPot->nbrDim() ) {
      GUM_ERROR ( OperationNotAllowed, "cannot exchange potentials with different dimensions for variable with id " << id );
    }

    for ( Idx i = 0; i < cpt ( id ).nbrDim(); i++ ) {
      if ( &cpt ( id ).variable ( i ) != & ( newPot->variable ( i ) ) ) {
        GUM_ERROR ( OperationNotAllowed, "cannot exchange potentails because, for variable with id " << id << ", dimension " << i << " differs. " );
      }
    }

    _unsafeChangePotential ( id, newPot );
  }


  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::_unsafeChangePotential ( NodeId id, Potential<GUM_SCALAR>* newPot ) {
    delete __probaMap[id];
    __probaMap[id] = newPot;
  }

} /* namespace gum */

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

#include <agrum/BN/IBayesNet.h>

#include <agrum/multidim/aggregators/or.h>
#include <agrum/multidim/aggregators/and.h>

#include <agrum/multidim/CIModels/multiDimNoisyAND.h>
#include <agrum/multidim/CIModels/multiDimNoisyORNet.h>
#include <agrum/multidim/CIModels/multiDimNoisyORCompound.h>

#include <agrum/BN/generator/simpleCPTGenerator.h>


namespace gum {

//                                  BayesNet


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

    for ( HashTableConstIterator<NodeId, Potential<GUM_SCALAR>*> iter = __probaMap.begin();
          iter != __probaMap.end();
          ++iter ) {
      delete *iter;
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

  template<typename GUM_SCALAR>
  Idx
  BayesNet<GUM_SCALAR>::dim() const {
    Idx dim = 0;

    //for ( DAG::NodeIterator node = dag().beginNodes(); node != dag().endNodes(); ++node ) {
    for ( const auto node : nodes() ) {
      Idx q = 1;
      Set<NodeId> s = dag().parents ( node );

      for ( Set<NodeId>::iterator parent = s.begin(); parent != s.end(); ++parent ) {
        q *= variable ( *parent ).domainSize();
      }

      dim += ( variable ( node ).domainSize() - 1 ) * q;
    }

    return dim;
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

    const MultiDimCIModel<GUM_SCALAR>* CImodel = dynamic_cast<const MultiDimCIModel<GUM_SCALAR>*> ( &content );

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


  template<typename GUM_SCALAR> INLINE
  std::string
  BayesNet<GUM_SCALAR>::toString ( void ) const {
    Size param = 0;

    double dSize = log10DomainSize();

    for ( const auto it : nodes() ) {
      param += ( ( const MultiDimImplementation<GUM_SCALAR>& ) cpt ( it ).getMasterRef() ).realSize();
    }

    double compressionRatio = log10 ( 1.0 * param ) - dSize;

    std::stringstream s;
    s << "BN{nodes: " << size() << ", arcs: " << dag().sizeArcs() << ", ";

    if ( dSize > 6 )
      s << "domainSize: 10^" << dSize;
    else
      s << "domainSize: " << round ( pow ( 10.0, dSize ) );

    s << ", parameters: " << param << ", compression ratio: ";

    if ( compressionRatio > -3 )
      s << trunc ( 100.0 - pow ( 10.0, compressionRatio + 2.0 ) );
    else
      s << "100-10^" << compressionRatio + 2.0;

    s << "% }";

    return s.str();
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

    //for ( DAG::NodeIterator node_iter = dag().beginNodes();node_iter != dag().endNodes(); ++node_iter ) {
    for ( const auto node_iter : nodes() ) {
      output << "\"" << variable ( node_iter ).name() << "\" [comment=\"" << node_iter << ":" << variable ( node_iter ) << "\"];" << std::endl;
    }

    output << std::endl;

    std::string tab = "  ";

    //for ( DAG::NodeIterator node_iter = dag().beginNodes();node_iter != dag().endNodes(); ++node_iter ) {
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


  /// Compute a parameter of the joint probability for the BN (given an instantiation of the vars)
  template<typename GUM_SCALAR>
  GUM_SCALAR BayesNet<GUM_SCALAR>::jointProbability ( const Instantiation& i ) const {
    GUM_SCALAR value = ( GUM_SCALAR ) 1.0;

    GUM_SCALAR tmp;

    //for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
    for ( const auto node_iter : nodes() ) {
      if ( ( tmp = cpt ( node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
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

    //for ( DAG::NodeIterator node_iter = dag().beginNodes(); node_iter != dag().endNodes(); ++node_iter ) {
    for ( const auto node_iter : nodes() ) {
      if ( ( tmp = cpt ( node_iter ) [i] ) == ( GUM_SCALAR ) 0 ) {
        return ( GUM_SCALAR ) ( - std::numeric_limits<double>::infinity( ) );
      }

      value += log2 ( cpt ( node_iter ) [i] );
    }

    return value;
  }

  /// begin Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::beginTopologyTransformation() {
    //for ( DAG::NodeIterator node_iter = dag().beginNodes();node_iter != dag().endNodes(); ++node_iter ) {
    for ( const auto node_iter : nodes() )
      __probaMap[node_iter]->beginMultipleChanges();
  }

  /// end Multiple Change for all CPTs
  template<typename GUM_SCALAR>
  void BayesNet<GUM_SCALAR>::endTopologyTransformation() {
    //for ( DAG::NodeIterator node_iter = dag().beginNodes();node_iter != dag().endNodes(); ++node_iter ) {
    for ( const auto node_iter : nodes() )
      __probaMap[node_iter]->endMultipleChanges();
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
    Potential<GUM_SCALAR>* copy_array = 0;

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
    SimpleCPTGenerator<GUM_SCALAR> generator;

    //for ( DAG::NodeIterator iter = this->beginNodes(); iter != this->endNodes(); ++iter ) {
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


  template <typename GUM_SCALAR>
  bool
  BayesNet<GUM_SCALAR>::operator== ( const BayesNet& from ) const {
    if ( dag() == from.dag() ) {
      //for ( DAG::NodeIterator node = beginNodes(); node != endNodes(); ++node ) {
      for ( const auto node : nodes() ) {
        // We don't use Potential::operator== because BN's don't share
        // DiscreteVariable's pointers.
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
        bijection.insert ( & ( variable ( node ) ), & ( from.variable ( node ) ) );
        const NodeSet& parents = dag().parents ( node );

        for ( NodeSetIterator arc = parents.begin(); arc != parents.end(); ++arc ) {
          bijection.insert ( & ( variable ( *arc ) ), & ( from.variable ( *arc ) ) );
        }

        Instantiation i ( cpt ( node ) );

        Instantiation j ( from.cpt ( node ) );

        for ( i.setFirst(); not i.end(); i.inc() ) {
          typedef Bijection<const DiscreteVariable*, const DiscreteVariable*>::iterator BiIter;

          for ( BiIter iter = bijection.begin(); iter != bijection.end(); ++iter ) {
            j.chgVal ( * ( iter.second() ), i.val ( * ( iter.first() ) ) );
          }

          if ( std::pow ( cpt ( node ).get ( i ) - from.cpt ( node ).get ( j ), ( GUM_SCALAR ) 2 ) > ( GUM_SCALAR ) 1e-6 ) {
            return false;
          }
        }
      }

      return true;
    }

    return false;
  }

  template <typename GUM_SCALAR>
  bool
  BayesNet<GUM_SCALAR>::operator!= ( const BayesNet& from ) const {
    return not this->operator== ( from );
  }

} /* namespace gum */

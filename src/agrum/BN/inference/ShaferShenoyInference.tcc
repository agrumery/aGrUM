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
 * @brief Implementations of the classes defined in
 * bns/inference/ShaferShenoyInference.h.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


// to ease parsing by IDE
#include <agrum/BN/inference/ShaferShenoyInference.h>

namespace gum {


// Default constructor
  template<typename GUM_SCALAR>
  ShaferShenoyInference<GUM_SCALAR>::ShaferShenoyInference ( const IBayesNet<GUM_SCALAR>& bayesNet ) :
    BayesNetInference<GUM_SCALAR> ( bayesNet ), __triangulation ( 0 ) {
    GUM_CONSTRUCTOR ( ShaferShenoyInference );

    NodeProperty<Size> __modalitiesMap;

    for ( auto iter_node = this->bn().nodes().beginSafe(); iter_node != this->bn().nodes().endSafe(); ++iter_node )
      __modalitiesMap.insert ( *iter_node,  this->bn().variable ( *iter_node ).domainSize() );

    __triangulation = new DefaultTriangulation ( & ( this->bn().moralGraph() ), &__modalitiesMap );

    __triangulation->junctionTree();
    __buildCliquesTables();
  }

// Destructor
  template<typename GUM_SCALAR>
  ShaferShenoyInference<GUM_SCALAR>::~ShaferShenoyInference() {
    GUM_DESTRUCTOR ( ShaferShenoyInference );

    delete __triangulation;

    for ( typename Property< MultiDimBucket<GUM_SCALAR>* >::onArcs::iterator_safe
          iter = __messagesMap.beginSafe(); iter != __messagesMap.endSafe(); ++iter ) {
      delete iter.val();
    }

    for ( typename Property< CliqueProp<GUM_SCALAR>* >::onNodes::iterator_safe
          iter = __clique_prop.beginSafe(); iter != __clique_prop.endSafe(); ++iter ) {
      delete iter.val();
    }

    for ( SetIteratorSafe< Potential<GUM_SCALAR>* > iter = __dummies.beginSafe();
          iter != __dummies.endSafe(); ++iter ) {
      delete *iter;
    }

  }

// Returns the Triangulation used by this class.
  template <typename GUM_SCALAR> INLINE
  Triangulation&
  ShaferShenoyInference<GUM_SCALAR>::triangulation() {
    return *__triangulation;
  }

// Makes the inference
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::makeInference() {
    this->_invalidateMarginals();

    // Setting all collect flags at false
    for ( typename Property< CliqueProp<GUM_SCALAR>* >::onNodes::iterator_safe
          iter = __clique_prop.beginSafe(); iter != __clique_prop.endSafe(); ++iter ) {
      ( iter.val() )->isCollected = false;
    }

    for ( typename Property< CliqueProp<GUM_SCALAR>* >::onNodes::iterator_safe
          iter = __clique_prop.beginSafe(); iter != __clique_prop.endSafe(); ++iter ) {
      if ( not ( iter.val() )->isCollected ) {
        __collectFromClique ( iter.key() );
        __diffuseFromClique ( iter.key() );
      }
    }
  }

// Returns the probability of the variable.
// @param id The variable's id.
// @throw NotFound Raised if no variable matches id.
// @throw OperationNotAllowed Raised if the inference haven't be done.
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::_fillMarginal ( NodeId id,
      Potential<GUM_SCALAR>& marginal ) {
    NodeId cliqueId = __triangulation->createdJunctionTreeClique ( id );
    // First we find the smallest clique containing id

    for ( auto iter_node = __triangulation->junctionTree().nodes().beginSafe(); iter_node != __triangulation->junctionTree().nodes().endSafe(); ++iter_node ) {
      if ( ( __triangulation->junctionTree().clique ( *iter_node ).contains ( id ) ) and
           ( __clique_prop[*iter_node]->bucket().domainSize() < __clique_prop[cliqueId]->bucket().domainSize() )
         ) {
        cliqueId = *iter_node;
      }
    }

    // Second we launch a collect starting from cliqueId
    __collectFromClique ( cliqueId );

    // Third we fill the marginal with the good values using a bucket
    MultiDimBucket<GUM_SCALAR> bucket;

    bucket.add ( this->bn().variable ( id ) );

    bucket.add ( __clique_prop[cliqueId]->bucket() );

    const NodeSet& neighbours = __getNeighbours ( cliqueId );

    for ( NodeSetIterator iter = neighbours.beginSafe ();
          iter != neighbours.endSafe (); ++iter ) {
      bucket.add ( __messagesMap[Arc ( *iter, cliqueId )] );
    }

    marginal.add ( this->bn().variable ( id ) ); // marginal is empty, this is stupid... (I know I'm the guy who did it...)

    Instantiation inst ( marginal );

    for ( inst.setFirst(); not inst.end(); inst.inc() ) {
      marginal.set ( inst, bucket.get ( inst ) );
    }

    marginal.normalize();
  }

// insert new evidence in the graph
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::insertEvidence (
    const List<const Potential<GUM_SCALAR>*>& pot_list ) {
    for ( ListConstIteratorSafe<const Potential<GUM_SCALAR>*> iter = pot_list.cbeginSafe();
          iter != pot_list.cendSafe(); ++iter ) {
      __clique_prop[__node2CliqueMap[
                      this->bn().nodeId ( ( *iter )->variable ( 0 ) )]
                   ]->addEvidence ( **iter );
      // don't forget that the next line won't be executed if the previous one
      // raised an exception because the evidence isn't valid.
      __removeDiffusedMessages (
        __node2CliqueMap[ this->bn().nodeId ( ( *iter )->variable ( 0 ) )]
      );
    }
  }

// remove a given evidence from the graph
  template <typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::eraseEvidence ( const Potential<GUM_SCALAR>* e ) {
    if ( not ( e->variablesSequence().size() != 1 ) ) {
      __clique_prop[
        __node2CliqueMap[this->bn().nodeId ( e->variable ( 0 ) )]
      ]->removeEvidence ( e->variable ( 0 ) );
      __removeDiffusedMessages ( __node2CliqueMap[
                                   this->bn().nodeId ( e->variable ( 0 ) )
                                 ] );
    }
  }

// remove all evidence from the graph
  template <typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::eraseAllEvidence() {
    for ( typename Property< CliqueProp<GUM_SCALAR>* >::onNodes::iterator_safe
          iter = __clique_prop.beginSafe(); iter != __clique_prop.endSafe(); ++iter ) {
      __removeDiffusedMessages ( iter.key() );
      ( iter.val() )->removeAllEvidence();
    }
  }

// @return Returns the list of neighbours of a given clique
  template<typename GUM_SCALAR> INLINE
  const NodeSet&
  ShaferShenoyInference<GUM_SCALAR>::__getNeighbours ( NodeId cliqueId ) {
    return __triangulation->junctionTree().neighbours ( cliqueId );
  }

// @return Returns a separator given two adjacent cliques
  template<typename GUM_SCALAR> INLINE
  const NodeSet&
  ShaferShenoyInference<GUM_SCALAR>::__getSeparator ( NodeId clique_1, NodeId clique_2 ) {
    return __triangulation->junctionTree().separator ( clique_1, clique_2 );
  }

// @return Returns the clique in which the node's cpt must be stored
  template<typename GUM_SCALAR>
  NodeId
  ShaferShenoyInference<GUM_SCALAR>::__getClique
  ( const std::vector<NodeId>& eliminationOrder, NodeId id ) {
    Set<NodeId> idSet;
    idSet.insert ( id );

    const NodeSet& parents = this->bn().dag().parents ( id );

    for ( NodeSetIterator iter = parents.beginSafe (); iter != parents.endSafe (); ++iter ) {
      idSet.insert ( *iter );
    }

    for ( size_t i = 0; i < eliminationOrder.size(); ++i ) {
      if ( idSet.contains ( eliminationOrder[i] ) ) {
        return __triangulation->createdJunctionTreeClique ( eliminationOrder[i] );
      }
    }

    std::stringstream msg;

    msg << "No clique found for node " << id;
    GUM_ERROR ( FatalError, msg.str() );
  }

// Builds the cliques tables
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__buildCliquesTables() {
    const std::vector<NodeId>& elim = __triangulation->eliminationOrder();

    NodeSet cliquesSet;
    // First pass to create the clique's table

    for ( auto iter_node = __triangulation->junctionTree().nodes().beginSafe(); iter_node != __triangulation->junctionTree().nodes().endSafe(); ++iter_node ) {
      __clique_prop.insert ( *iter_node, new CliqueProp<GUM_SCALAR> ( *iter_node ) );
      cliquesSet.insert ( *iter_node );

      for ( NodeSetIterator jter = __triangulation->junctionTree().clique ( *iter_node ).beginSafe();
            jter != __triangulation->junctionTree().clique ( *iter_node ).endSafe(); ++jter ) {
        __clique_prop[*iter_node]->addVariable ( this->bn().variable ( *jter ) );
      }
    }

    // Second pass to add the potentials in the good cliques
    for ( size_t i = 0; i < elim.size(); i++ ) {
      NodeId cliqueId = __getClique ( elim, elim[i] );
      __node2CliqueMap.insert ( elim[i], cliqueId );
      __clique_prop[cliqueId]->addPotential ( this->bn().cpt ( elim[i] ) );
      cliquesSet.erase ( cliqueId );
    }

    // Second pass to fill empty cliques with "one" matrices.
    for ( NodeSetIterator iter = cliquesSet.beginSafe(); iter != cliquesSet.endSafe(); ++iter ) {
      __clique_prop[*iter]->addPotential ( *__makeDummyPotential ( *iter ) );
    }
  }

// Calls a collect with a node as source
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__collectFromClique ( NodeId source ) {
    __clique_prop[source]->isCollected = true;

    try {
      const NodeSet& neighbours =  __getNeighbours ( source );

      for ( NodeSetIterator iter = neighbours.beginSafe();
            iter != neighbours.endSafe(); ++iter ) {
        __collect ( source, *iter );
      }
    } catch ( NotFound& ) {
      // Raised if source has no neighbours
    }
  }

// Collecting phase of the inference
  template<typename GUM_SCALAR>
  bool
  ShaferShenoyInference<GUM_SCALAR>::__collect ( NodeId source, NodeId current ) {
    __clique_prop[current]->isCollected = true;
    bool newMsg = false; // Flag used to know if we must recompute the message current -> source

    const NodeSet& neighbours = __getNeighbours ( current );

    for ( NodeSetIterator iter = neighbours.beginSafe();
          iter != neighbours.endSafe(); ++iter ) {
      if ( *iter != source ) {
        bool retVal = __collect ( current, *iter );
        newMsg = newMsg or retVal;
      }
    }

    if ( newMsg ) {
      // I need to recompute current's message, so no need to check for new
      // evidence
      __removeDiffusedMessages ( current );
      __sendMessage ( current, source );
      return true;
    } else if ( not __messageExists ( current, source ) ) {
      // There is new evidence (or first call)
      __sendMessage ( current, source );
      return true;
    } else {
      // The message was already computed
      return false;
    }
  }

// Diffusing phase of the inference
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__diffuseFromClique ( NodeId source ) {
    try {
      const NodeSet& neighbours = __getNeighbours ( source );

      for ( NodeSetIterator iter = neighbours.beginSafe();
            iter != neighbours.endSafe(); ++iter ) {
        if ( __messageExists ( source, *iter ) ) {
          // No new evidence and msg already computed
          __diffuse ( source, *iter, false );
        } else {
          // New evidence or first call
          __sendMessage ( source, *iter );
          __diffuse ( source, *iter, true );
        }
      }
    } catch ( NotFound& ) {
      // Raised if source has no neighbours
    }
  }

// Diffusing phase of the inference
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__diffuse ( NodeId source, NodeId current,
      bool recompute ) {
    const NodeSet& neighbours =  __getNeighbours ( current );

    for ( NodeSetIterator iter = neighbours.beginSafe();
          iter != __getNeighbours ( current ).endSafe(); ++iter ) {
      if ( *iter != source ) {
        if ( recompute or ( not __messageExists ( current, *iter ) ) ) {
          // New evidence or first call
          __sendMessage ( current, *iter );
          __diffuse ( current, *iter, true );
        } else {
          // No new evidence and msg already computed
          __diffuse ( current, *iter, false );
        }
      }
    }
  }

// Create and saves the message from key.first to key.second in the
// __messagesMap.
  template<typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__sendMessage ( NodeId tail, NodeId head ) {
    // Building the message's table held by the separator
    MultiDimBucket<GUM_SCALAR>* message = new MultiDimBucket<GUM_SCALAR>();

    for ( NodeSet::iterator_safe iter = __getSeparator ( tail, head ).beginSafe();
          iter != __getSeparator ( tail, head ).endSafe(); ++iter ) {
      message->add ( this->bn().variable ( *iter ) );
    }

    // Check if the clique was initialized
    try {
      message->add ( __clique_prop[tail]->bucket() );
    } catch ( NotFound& ) {
      std::stringstream msg; msg << ": missing CliqueProp on clique " << tail;
      GUM_ERROR ( FatalError, msg.str() );
    }

    // Second, add message from tail's neighbours
    const NodeSet& neighbours = __getNeighbours ( tail );

    for ( NodeSetIterator iter = neighbours.beginSafe();
          iter != __getNeighbours ( tail ).endSafe(); ++iter ) {
      if ( *iter != head ) {
        try {
          message->add ( __messagesMap[Arc ( *iter, tail )] );
        } catch ( NotFound& ) {
          std::stringstream msg;
          msg << ": missing message (" << *iter << ", " << tail << ")";
          msg << " to compute message (" << tail << ", " << head << ")";
          GUM_ERROR ( FatalError, msg.str() );
        }
      }
    }

    try {
      delete __messagesMap[Arc ( tail, head )];
    } catch ( NotFound& ) {
      /* Nothing to delete */
    }

    __messagesMap.insert ( Arc ( tail, head ), message );
  }

  template <typename GUM_SCALAR> INLINE
  bool
  ShaferShenoyInference<GUM_SCALAR>::__messageExists ( NodeId source, NodeId dest ) {
    return __messagesMap.exists ( Arc ( source , dest ) );
  }

  template <typename GUM_SCALAR>
  void
  ShaferShenoyInference<GUM_SCALAR>::__removeDiffusedMessages ( NodeId cliqueId ) {
    const NodeSet& neighbours = __getNeighbours ( cliqueId );

    for ( NodeSetIterator iter = neighbours.beginSafe();
          iter != neighbours.endSafe(); ++iter ) {
      if ( __messagesMap.exists ( Arc ( cliqueId, *iter ) ) ) {
        delete __messagesMap[Arc ( cliqueId, *iter )];
        __messagesMap.erase ( Arc ( cliqueId, *iter ) );
      }
    }
  }

// @param cliqueId The clique for which the dummy bucket is made.
// @return A pointer over the dummy bucket.
  template <typename GUM_SCALAR> INLINE
  Potential<GUM_SCALAR>*
  ShaferShenoyInference<GUM_SCALAR>::__makeDummyPotential ( NodeId cliqueId ) {
    Potential<GUM_SCALAR>* pot = new Potential<GUM_SCALAR> ( new MultiDimSparse<GUM_SCALAR> ( ( GUM_SCALAR ) 1 ) );
    __dummies.insert ( pot );

    for ( Set<NodeId>::const_iterator_safe iter = __triangulation->junctionTree().clique ( cliqueId ).beginSafe();
          iter != __triangulation->junctionTree().clique ( cliqueId ).endSafe(); ++iter ) {
      pot->add ( this->bn().variable ( *iter ) );
    }

    return pot;
  }


//                                CLIQUEPROP


// Default constructor.
// @param id the id of this clique on which this properties holds.
  template <typename GUM_SCALAR>
  CliqueProp<GUM_SCALAR>::CliqueProp ( NodeId id ) :
    isCollected ( false ), __potential ( new MultiDimBucket<GUM_SCALAR>() ),
    __varsPotential ( 0 ), __name ( "" ) {
    GUM_CONSTRUCTOR ( CliqueProp );
    std::stringstream name;
    name << id;
    __name = name.str();
  }

// Destructor.
  template <typename GUM_SCALAR>
  CliqueProp<GUM_SCALAR>::~CliqueProp() {
    GUM_DESTRUCTOR ( CliqueProp );
    delete __potential;

    if ( __varsPotential != 0 ) {
      delete __varsPotential;
    }
  }

// Returns the name of this clique.
  template <typename GUM_SCALAR> INLINE
  const std::string&
  CliqueProp<GUM_SCALAR>::name() const {
    return __name;
  }

// Add a variable to this clique
// @param v The added variable.
  template <typename GUM_SCALAR> INLINE
  void
  CliqueProp<GUM_SCALAR>::addVariable ( const DiscreteVariable& v ) {
    __potential->add ( v );

    if ( __varsPotential != 0 ) {
      __varsPotential->add ( v );
    }
  }

// Add a Potential to this clique
// @param cpt The added CPT.
  template <typename GUM_SCALAR> INLINE
  void
  CliqueProp<GUM_SCALAR>::addPotential ( const Potential<GUM_SCALAR>& cpt ) {
    if ( __varsPotential != 0 ) {
      __varsPotential->add ( cpt );
    } else {
      __potential->add ( cpt );
    }
  }

// Add an evidence on a variable in this clique.
// @throw NotFound Raised if the evidence is on a variable not present
//                 in this clique.
  template <typename GUM_SCALAR>
  void
  CliqueProp<GUM_SCALAR>::addEvidence ( const Potential<GUM_SCALAR>& evidence ) {
    if ( evidence.variablesSequence().size() != 1 ) {
      std::stringstream msg;
      msg << ": expected evidence on 1 variable, found on ";
      msg  << evidence.variablesSequence().size();
      GUM_ERROR ( OperationNotAllowed, msg.str() );
    }

    if ( not __potential->variablesSequence().exists (
           evidence.variablesSequence().atPos ( 0 )
         ) ) {
      std::stringstream msg;
      msg << ": " << evidence.variablesSequence().atPos ( 0 )->name();
      msg << " not found in this clique " << __name;
      GUM_ERROR ( NotFound, msg.str() );
    }

    if ( __varsPotential == 0 ) {
      __varsPotential = __potential;
      __potential = new MultiDimBucket<GUM_SCALAR>();

      for ( gum::Sequence<const gum::DiscreteVariable*>::const_iterator_safe iter = __varsPotential->variablesSequence().beginSafe(); iter != __varsPotential->variablesSequence().endSafe(); ++iter ) {
        __potential->add ( **iter );
      }

      __potential->add ( __varsPotential );
    }

    __evidences.set ( evidence.variablesSequence().atPos ( 0 ), &evidence );

    __potential->add ( evidence );
  }

// Removes all the evidences containing v
  template <typename GUM_SCALAR> INLINE
  void
  CliqueProp<GUM_SCALAR>::removeEvidence ( const DiscreteVariable& v ) {
    __evidences.erase ( &v );

    if ( __evidences.size() == 0 ) {
      delete __potential;
      __potential = __varsPotential;
      __varsPotential = 0;
    }
  }

// Remove all the evidences
  template <typename GUM_SCALAR> INLINE
  void
  CliqueProp<GUM_SCALAR>::removeAllEvidence() {
    if ( __evidences.size() != 0 ) {
      delete __potential;
      __potential = __varsPotential;
      __varsPotential = 0;
      __evidences.clear();
    }
  }

// @return Returns the mapping of evidences on the variables in this clique.
  template <typename GUM_SCALAR> INLINE
  const HashTable<const DiscreteVariable*, const Potential<GUM_SCALAR>* >&
  CliqueProp<GUM_SCALAR>::evidences() const {
    return __evidences;
  }

/// @return Returns the bucket of this Clique
  template <typename GUM_SCALAR> INLINE
  MultiDimBucket<GUM_SCALAR>&
  CliqueProp<GUM_SCALAR>::bucket() {
    return *__potential;
  }

/// @return Returns the bucket of this Clique
  template <typename GUM_SCALAR> INLINE
  const MultiDimBucket<GUM_SCALAR>&
  CliqueProp<GUM_SCALAR>::bucket() const {
    return *__potential;
  }


} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; 

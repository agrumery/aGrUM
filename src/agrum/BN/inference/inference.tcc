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
 * @brief Implementation of the non pure virtual methods of class Inference.
 */

// to ease parsers in IDE
#ifdef GUM_INFERENCE_H
#include <agrum/BN/inference/inference.h>


namespace gum {

  
  // Default Constructor
  template <typename GUM_SCALAR>
  Inference<GUM_SCALAR>::Inference( const IBayesNet<GUM_SCALAR>& bn ) :
    _bn ( bn ),
    __state( StateOfInference::Unprepared ) {
    GUM_CONSTRUCTOR( Inference );
  }

  
  // Destructor
  template <typename GUM_SCALAR> Inference<GUM_SCALAR>::~Inference() {
    __invalidatePosteriors ();
    clearEvidence ();

    GUM_DESTRUCTOR( Inference );
  }
  

  // remove all the posteriors computed
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::__invalidatePosteriors() noexcept {
    // remove the posteriors for single targets
    for ( const auto& pair : __target_posteriors ) {
      if ( pair.second != nullptr ) {
        delete ( pair.second );
      }
    }
    __target_posteriors.clear ();

    // remove the posteriors for the target sets
        for ( const auto& pair : __settarget_posteriors ) {
      if ( pair.second != nullptr ) {
        delete ( pair.second );
      }
    }
    __settarget_posteriors.clear ();
  }
  

  // returns whether the inference object is in a ready state
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isReady () const noexcept {
    return ( __state == StateOfInference::Ready );
  }

  
  // returns whether the inference object is in a done state
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isDone () const noexcept {
    return ( __state == StateOfInference::Done );
  }


  // Returns a constant reference over the IBayesNet referenced by this class
  template <typename GUM_SCALAR>
  INLINE const IBayesNet<GUM_SCALAR>& Inference<GUM_SCALAR>::bn () const noexcept {
    return _bn;
  }


  // prepare the internal inference structures for the next inference
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::prepareInference () {
    if ( isReady() || isDone() ) {
      return;
    }

    _prepareInference();
    __state = StateOfInference::Ready;
  }


  // perform the heavy computations needed to compute the targets' posteriors
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::makeInference () {
    if ( isDone() ) {
      return;
    }

    if ( ! isReady() ) {
      prepareInference ();
    }

    _makeInference();

    __invalidatePosteriors ();
    __state = StateOfInference::Done;
  }
  

  // return true if variable is a target
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isTarget( NodeId var ) const {
    if ( ! _bn.variableNodeMap().exists( var ) ) {
      GUM_ERROR( UndefinedElement, var << " is not a NodeId in the bn" );
    }

    if ( __targets.empty() ) {
      return true;
    }

    return __targets.contains( var );
  }
  

  // return true if target is a nodeset target.
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isSetTarget( const NodeSet& vars ) const {
    for ( const auto var : vars ) {
      if ( ! _bn.variableNodeMap().exists( var ) ) {
        GUM_ERROR( UndefinedElement, var << " is not a NodeId in the bn" );
      }
    }

    return __settargets.contains( vars );
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::clearTargets () {
    __targets.clear();
    __settargets.clear();
    __state = StateOfInference::Unprepared;
  }
  

  // Add a single target to the list of targets
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::addTarget( NodeId target ) {
    // check if the node belongs to the Bayesian network
    if ( ! _bn.variableNodeMap().exists( target ) ) {
      GUM_ERROR( UndefinedElement, target << " is not a NodeId in the bn" );
    }

    // if the target set is empty, add the new target
    if ( __targets.empty () ) {
      __targets.insert( target );
      __state = StateOfInference::Unprepared;
    }
    else if ( ! __targets.contains ( target ) ) {
      if ( __targets.size() == _bn.size() - 1 ) {
        clearTargets();  // all node are targets <=> no target anymore
      }
      else {
        __targets.insert( target );
      }

      __state = StateOfInference::Unprepared;
    }
  }


  // Add a set of nodes as a new target
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::addSetTarget( const NodeSet& target ) {
    // check if the nodes in the target belong to the Bayesian network
    for ( const auto node : target ) {
      if ( ! _bn.variableNodeMap().exists( node ) ) {
        GUM_ERROR( UndefinedElement, target << " does not belong to the bn" );
      }
    }

    // check that the settarget set does not contain the new target
    if ( __settargets.contains ( target ) ) return;

    __settargets.insert ( target );
    __state = StateOfInference::Unprepared;
  }
  
  
  // returns the list of single targets
  template <typename GUM_SCALAR>
  INLINE const NodeSet& Inference<GUM_SCALAR>::targets () const noexcept {
    return __targets;
  }
  
  
  /// returns the list of target sets
  template <typename GUM_SCALAR>
  INLINE const Set<NodeSet>& Inference<GUM_SCALAR>::setTargets () const noexcept {
    return __settargets;
  }
  

  // create the internal structure for a hard evidence
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  Inference<GUM_SCALAR>::__createHardEvidence ( NodeId id,
                                                Idx val ) const {
    // check that it is possible to create the evidence
    if ( ! _bn.variableNodeMap().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( _bn.variable( id ).domainSize() <= val ) {
      GUM_ERROR( InvalidArgument, _bn.variable( id ) << " and " << val
                                                     << " do not correspond." );
    }
    
    // create the deterministic potential
    Potential<GUM_SCALAR> pot;
    pot.beginMultipleChanges();
    pot << _bn.variable( id );
    pot.endMultipleChanges( 0.0 );

    Instantiation I ( pot );
    I.chgValByIdx( 0, val );
    pot.set( I, 1.0 );

    return pot;
  }


  // checks wether a potential corresponds to a hard evidence
  template <typename GUM_SCALAR>
  bool
  Inference<GUM_SCALAR>::__isHardEvidence( const Potential<GUM_SCALAR>& pot,
                                           Idx& val ) const {
    // checking if pot is determininstic
    bool notZero = false;
    Instantiation I( pot );

    for ( I.setFirst(); !I.end(); I.inc() ) {
      if ( pot[I] != 0.0 ) {
        if ( notZero ) {  // we already met a non-zero value
          return false;
        }
        else {
          val = I.val( 0 );
          notZero = true;  // this is the first met non-zero value
        }
      }
    }

    if ( !notZero ) {  // we met no non-zero value
      GUM_ERROR( FatalError, "Evidence of impossibility (vector of 0s)" );
    }

    return true;  // pot is deterministic
  }


  // adds a new hard evidence on node id
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::addEvidence( NodeId id,
                                                  Idx val ) {
    addEvidence( __createHardEvidence( id, val ) );
  }

  
  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::addEvidence( NodeId id,
                                      const std::vector<GUM_SCALAR>& vals ) {
    // checks that the evidence is meaningful
    if ( ! _bn.variableNodeMap().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( _bn.variable( id ).domainSize() != vals.size() ) {
      GUM_ERROR( InvalidArgument, "node " << _bn.variable( id )
                 << " and its evidence have different sizes." );
    }

    Potential<GUM_SCALAR> pot;
    pot.add( _bn.variable( id ) );
    pot.fillWith( vals );
    addEvidence( std::move ( pot ) );
  }


  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::addEvidence( const Potential<GUM_SCALAR>& pot ) {
    // check if the potential corresponds to an evidence
    if ( pot.nbrDim() != 1 ) {
      GUM_ERROR( InvalidArgument, pot << " is not mono-dimensional." );
    }
    const NodeId id = _bn.variableNodeMap().get( pot.variable( 0 ) );

    if ( hasEvidence( id ) ) {
      GUM_ERROR( InvalidArgument,
                 id << " already has an evidence. Please use chgEvidence()." );
    }
    
    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx val;
    bool hard_evidence = __isHardEvidence ( pot, val );

    // insert the evidence
    __evidence.insert( id, new Potential<GUM_SCALAR>( pot ) );
    if ( hard_evidence ) {  // pot is deterministic
      __hardEvidence.insert( id, val );
    }
    
    __state = StateOfInference::Unprepared;
  }
  

  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::addEvidence( Potential<GUM_SCALAR>&& pot ) {
    // check if the potential corresponds to an evidence
    if ( pot.nbrDim() != 1 ) {
      GUM_ERROR( InvalidArgument, pot << " is not mono-dimensional." );
    }
    const NodeId id = _bn.variableNodeMap().get( pot.variable( 0 ) );

    if ( hasEvidence( id ) ) {
      GUM_ERROR( InvalidArgument,
                 id << " already has an evidence. Please use chgEvidence()." );
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx val;
    bool hard_evidence = __isHardEvidence ( pot, val );

    // insert the evidence
    __evidence.insert( id, new Potential<GUM_SCALAR>
                       ( std::forward<Potential<GUM_SCALAR>> ( pot ) ) );
    if ( hard_evidence ) {  // pot is deterministic
      __hardEvidence.insert( id, val );
    }

    __state = StateOfInference::Unprepared;
  }


  // change the value of an already existing hard evidence
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::chgEvidence( NodeId id,
                                                  Idx val ) {
    Potential<GUM_SCALAR> pot = __createHardEvidence( id, val );
    chgEvidence( pot );
  }


  // change the value of an already existing evidence (might be soft or hard)
  template <typename GUM_SCALAR>
  INLINE void
  Inference<GUM_SCALAR>::chgEvidence( NodeId id,
                                      const std::vector<GUM_SCALAR>& vals ) {
    // check whether this corresponds to an evidence
    if ( ! _bn.variableNodeMap().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( _bn.variable( id ).domainSize() != vals.size() ) {
      GUM_ERROR( InvalidArgument, "node " << _bn.variable( id )
                 << " and its evidence have different sizes." );
    }

    // create the potential corresponding to vals
    Potential<GUM_SCALAR> pot;
    pot.add( _bn.variable( id ) );
    pot.fillWith( vals );
    chgEvidence( pot );
  }


  // change the value of an already existing evidence (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::chgEvidence( const Potential<GUM_SCALAR>& pot ) {
    // check if the potential corresponds to an evidence
    if ( pot.nbrDim() != 1 ) {
      GUM_ERROR( InvalidArgument, pot << " is not mono-dimensional." );
    }
    const NodeId id = _bn.variableNodeMap().get( pot.variable( 0 ) );

    if ( ! hasEvidence( id ) ) {
      GUM_ERROR( InvalidArgument,
                 id << " has no evidence. Please use addEvidence()." );
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx val;
    bool hard_evidence = __isHardEvidence ( pot, val );

    // modify the evidence already stored
    const Potential<GUM_SCALAR>* localPot = __evidence[id];
    Instantiation I( pot );
    for ( I.setFirst(); !I.end(); I.inc() ) {
      localPot->set( I, pot[I] );
    }

    // the inference state will be different
    // whether evidence change from Hard to Soft or not.
    bool hasChangedSoftHard = false;

    if ( hard_evidence ) {
      if ( ! hasHardEvidence( id ) ) {
        hasChangedSoftHard = true;
        __hardEvidence.insert( id, val );
      }
    }
    else {
      if ( hasHardEvidence( id ) ) {  // evidence was hard
        __hardEvidence.erase( id );
        hasChangedSoftHard = true;
      }
    }

    if ( _onEvidenceChanged( id, hasChangedSoftHard ) ) {
      __state = StateOfInference::Unprepared;
    }
    else {
      // if unprepared, it remains unprepared
      // it it is done, it becomes Ready again
      // it if is ready, it remains Ready
      if ( isDone() ) {
        __state = StateOfInference::Ready;
      }
    }
  }


  // indicates whether some node(s) have received evidence
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::hasEvidence () const {
    return ! __evidence.empty ();
  }


  // indicates whether node id has received an evidence
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::hasEvidence ( NodeId id ) const {
    return __evidence.exists( id );
  }
  

  // indicates whether node id has received a hard evidence
  template <typename GUM_SCALAR>
  INLINE bool
  Inference<GUM_SCALAR>::hasHardEvidence ( NodeId id ) const {
    return __hardEvidence.exists( id );
  }


  // indicates whether node id has received a soft evidence
  template <typename GUM_SCALAR>
  INLINE bool
  Inference<GUM_SCALAR>::hasSoftEvidence ( NodeId id ) const {
    return __evidence.exists( id ) && ! __hardEvidence.exists( id );
  }

  
  // returns the number of evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrEvidence () const {
    return __evidence.size();
  }


  // returns the number of hard evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrHardEvidence () const {
    return __hardEvidence.size();
  }
  

  // returns the number of soft evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrSoftEvidence () const {
    return __evidence.size() - __hardEvidence.size();
  }

  
  // removed the evidence, if any, corresponding to node id
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::clearEvidence ( NodeId id ) {
    if ( hasEvidence( id ) ) {
      if ( hasHardEvidence( id ) ) {
        __hardEvidence.erase ( id );
      }

      delete ( __evidence[id] );
      __evidence.erase( id );
      __state = StateOfInference::Unprepared;
    }
  }


  // removes all the evidence entered into the network
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::clearEvidence() {

    for ( const auto& pair : __evidence )
      if ( pair.second != nullptr ) {
        delete ( pair.second );
      }

    __evidence.clear();

    __state = StateOfInference::Unprepared;
  }

  
  // const access to evidence
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<const Potential<GUM_SCALAR>*>&
  Inference<GUM_SCALAR>::_evidence () const {
    return __evidence;
  }
  

  // const access to hard evidence
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<Idx>&
  Inference<GUM_SCALAR>::_hardEvidence () const {
    return __hardEvidence;
  }

  
  // Compute the posterior of a node.
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  Inference<GUM_SCALAR>::posterior( NodeId var ) {
    if ( ! isTarget ( var ) ) {  // throws UndefinedElement if var is not a target
      GUM_ERROR( UndefinedElement, var << " is not a target node" );
    }

    if ( !isDone() ) {
      makeInference();
    }

    if ( !__target_posteriors.exists( var ) ) {
      Potential<GUM_SCALAR>* pot = new Potential<GUM_SCALAR>();
      pot->add( _bn.variable( var ) );
      _fillPosterior( var, *pot );
      __target_posteriors.insert( var, pot );
    }

    return *__target_posteriors[var];
  }


  // Compute the posterior of a nodeset.
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  Inference<GUM_SCALAR>::posterior( const NodeSet& vars ) {
    for ( const auto var : vars ) {
      if ( ! isTarget ( var ) ) { // throws UndefinedElement if var is not a target
        GUM_ERROR( UndefinedElement, var << " is not a target node" );
      }
    }

    if ( ! isDone () ) {
      makeInference();
    }

    if ( !__settarget_posteriors.exists( vars ) ) {
      Potential<GUM_SCALAR>* pot = new Potential<GUM_SCALAR>();
      pot.beginMultipleChanges();
      pot->add( _bn.variable( var ) );
      pot.endMultipleChanges();

      _fillSetPosterior( vars, *pot );
      __settarget_posteriors.insert( vars, pot );
    }

    return *__settarget_posteriors[vars];
  }


} /* namespace gum */


#endif  // GUM_INFERENCE_H
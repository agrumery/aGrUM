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


namespace gum {

  
  // Default Constructor
  template <typename GUM_SCALAR>
  Inference<GUM_SCALAR>::Inference ( const IBayesNet<GUM_SCALAR>* bn ) :
    __bn ( bn ) {
    __computeDomainSizes ();
    
    GUM_CONSTRUCTOR( Inference );
  }

  
  // Default Constructor
  template <typename GUM_SCALAR>
  Inference<GUM_SCALAR>::Inference () {
    GUM_CONSTRUCTOR( Inference );
  }

  
  // Destructor
  template <typename GUM_SCALAR> Inference<GUM_SCALAR>::~Inference () {
    // clear all evidence.
    // Warning: Do not use method eraseAllEvidence () because it contains a call
    // to pure virtual method _onAllEvidenceErased which belongs to an inherited
    // instance and, therefore, does not exist anymore when ~Inference () is
    // called
    for ( const auto& pair : __evidence ) {
      if ( pair.second != nullptr ) {
        delete ( pair.second );
      }
    }

    GUM_DESTRUCTOR( Inference );
  }
  

  // returns whether the inference object is in a ready state
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isReady4Inference () const noexcept {
    return ( __state == StateOfInference::Ready4Inference );
  }

  
  // returns whether the inference object is in a done state
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::isDone () const noexcept {
    return ( __state == StateOfInference::Done );
  }

  
  // returns the state of the inference engine
  template <typename GUM_SCALAR>
  INLINE typename Inference<GUM_SCALAR>::StateOfInference
  Inference<GUM_SCALAR>::state () const noexcept {
    return __state;
  }


  // Returns a constant reference over the IBayesNet referenced by this class
  template <typename GUM_SCALAR>
  INLINE const IBayesNet<GUM_SCALAR>&
  Inference<GUM_SCALAR>::BayesNet () const {
    if ( __bn == nullptr )
      GUM_ERROR ( UndefinedElement, "No Bayes net has been assigned to "
                  "the inference algorithm." );
    return *__bn;
  }

  
  // assigns a new BN to the inference engine
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::setBayesNet ( const IBayesNet<GUM_SCALAR>* bn ) {
    clear ();
    __bn = bn;
    __computeDomainSizes ();
    _onBayesNetChanged ( bn );
    __state = StateOfInference::UnpreparedStructure;
  }


  // assigns a BN to a newly constructed inference engine
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::__setBayesNet ( const IBayesNet<GUM_SCALAR>* bn ) {
    __bn = bn;
    __computeDomainSizes ();
    __state = StateOfInference::UnpreparedStructure;
  }

  
  // clears all the data structures allocated for the last inference
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::clear () {
    eraseAllEvidence ();
    __state = StateOfInference::UnpreparedStructure;
  }

  
  /// computes the domain sizes of the random variables
  template <typename GUM_SCALAR>
  void Inference<GUM_SCALAR>::__computeDomainSizes () {
    __domain_sizes.clear ();
    if ( __bn != nullptr ) {
      for ( const auto node : __bn->dag() ) {
        __domain_sizes.insert ( node, __bn->variable( node ).domainSize () );
      }
    }
  }

  
  // get the domain sizes of the random variables of the BN
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<Size>&
  Inference<GUM_SCALAR>::domainSizes () const {
    return __domain_sizes;
  }
 


  // ##############################################################################
  // Evidence
  // ##############################################################################

  // create the internal structure for a hard evidence
  template <typename GUM_SCALAR>
  Potential<GUM_SCALAR>
  Inference<GUM_SCALAR>::__createHardEvidence ( const NodeId id,
                                                const Idx val ) const {
    // check that it is possible to create the evidence
    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    if ( ! __bn->dag().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( __bn->variable( id ).domainSize() <= val ) {
      GUM_ERROR( InvalidArgument, "node " << __bn->variable( id )
                 << " has fewer possible values than " << val );
    }
    
    // create the deterministic potential
    Potential<GUM_SCALAR> pot;
    pot.beginMultipleChanges();
    pot << __bn->variable( id );
    pot.endMultipleChanges( 0.0 );

    Instantiation I ( pot );
    I.chgVal ( __bn->variable( id ), val );
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

    for ( I.setFirst (); !I.end (); I.inc () ) {
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

    if ( ! notZero ) {  // we met no non-zero value
      GUM_ERROR( FatalError, "Evidence of impossibility (vector of 0s)" );
    }

    return true;  // pot is deterministic
  }


  // adds a new hard evidence on node id
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::addEvidence( const NodeId id,
                                                  const Idx val ) {
    addEvidence( __createHardEvidence( id, val ) );
  }

  
  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::addEvidence( const NodeId id,
                                      const std::vector<GUM_SCALAR>& vals ) {
    // checks that the evidence is meaningful
    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    if ( ! __bn->dag().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( __bn->variable( id ).domainSize() != vals.size() ) {
      GUM_ERROR( InvalidArgument, "node " << __bn->variable( id )
                 << " and its evidence vector have different sizes." );
    }

    Potential<GUM_SCALAR> pot;
    pot.add( __bn->variable( id ) );
    pot.fillWith( vals );
    addEvidence( std::move ( pot ) );
  }


  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::addEvidence( Potential<GUM_SCALAR>&& pot ) {
    // check if the potential corresponds to an evidence
    if ( pot.nbrDim() != 1 ) {
      GUM_ERROR( InvalidArgument, pot << " is not mono-dimensional." );
    }
    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );
    
    const NodeId id = __bn->nodeId ( pot.variable( 0 ) );

    if ( hasEvidence( id ) ) {
      GUM_ERROR( InvalidArgument," node " << id
                 << " already has an evidence. Please use chgEvidence()." );
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx val;
    bool is_hard_evidence = __isHardEvidence ( pot, val );

    // insert the evidence
    __evidence.insert( id, new Potential<GUM_SCALAR>
                       ( std::forward<Potential<GUM_SCALAR>> ( pot ) ) );
    if ( is_hard_evidence ) {  // pot is deterministic
      __hard_evidence.insert( id, val );
      __hard_evidence_nodes.insert ( id );
    }
    else {
      __soft_evidence_nodes.insert ( id );
    }

    __state = StateOfInference::UnpreparedStructure;
    _onEvidenceAdded ( id, is_hard_evidence );
  }

  
  // adds a new evidence on node id (might be soft or hard)
  template <typename GUM_SCALAR>
  INLINE void
  Inference<GUM_SCALAR>::addEvidence( const Potential<GUM_SCALAR>& pot ) {
    Potential<GUM_SCALAR> new_pot ( pot );
    addEvidence( std::move ( new_pot ) );
  }


  /// adds a new list of evidence
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::addListOfEvidence
  ( const List<Potential<GUM_SCALAR>*>& potlist ) {
    for ( const auto pot : potlist )
      addEvidence ( *pot );
  }

  
  /// adds a new set of evidence
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::addSetOfEvidence
  ( const Set<Potential<GUM_SCALAR>*>& potset ) {
    for ( const auto pot : potset )
      addEvidence ( *pot );
  }

  
  // indicates whether some node(s) have received evidence
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::hasEvidence () const {
    return ! __evidence.empty ();
  }


  // indicates whether node id has received an evidence
  template <typename GUM_SCALAR>
  INLINE bool Inference<GUM_SCALAR>::hasEvidence ( const NodeId id ) const {
    return __evidence.exists( id );
  }
  

  // indicates whether node id has received a hard evidence
  template <typename GUM_SCALAR>
  INLINE bool
  Inference<GUM_SCALAR>::hasHardEvidence ( const NodeId id ) const {
    return __hard_evidence_nodes.exists( id );
  }


  // indicates whether node id has received a soft evidence
  template <typename GUM_SCALAR>
  INLINE bool
  Inference<GUM_SCALAR>::hasSoftEvidence ( const NodeId id ) const {
    return __soft_evidence_nodes.exists( id );
  }


  // change the value of an already existing hard evidence
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::chgEvidence( const NodeId id,
                                                  const Idx val ) {
    chgEvidence( __createHardEvidence( id, val ) );
  }


  // change the value of an already existing evidence (might be soft or hard)
  template <typename GUM_SCALAR>
  INLINE void
  Inference<GUM_SCALAR>::chgEvidence( const NodeId id,
                                      const std::vector<GUM_SCALAR>& vals ) {
    // check whether this corresponds to an evidence
    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    if ( ! __bn->dag().exists( id ) ) {
      GUM_ERROR( UndefinedElement, id << " is not a NodeId in the bn" );
    }

    if ( __bn->variable( id ).domainSize() != vals.size() ) {
      GUM_ERROR( InvalidArgument, "node " << __bn->variable( id )
                 << " and its evidence have different sizes." );
    }

    // create the potential corresponding to vals
    Potential<GUM_SCALAR> pot;
    pot.add( __bn->variable( id ) );
    pot.fillWith( vals );
    chgEvidence( pot );
  }


  // change the value of an already existing evidence (might be soft or hard)
  template <typename GUM_SCALAR>
  void
  Inference<GUM_SCALAR>::chgEvidence( const Potential<GUM_SCALAR>& pot ) {
    // check if the potential corresponds to an evidence
    if ( pot.nbrDim() != 1 ) {
      GUM_ERROR( InvalidArgument, pot << " is not a mono-dimensional potential." );
    }
    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    const NodeId id = __bn->nodeId ( pot.variable( 0 ) );

    if ( ! hasEvidence( id ) ) {
      GUM_ERROR( InvalidArgument,
                 id << " has no evidence. Please use addEvidence()." );
    }

    // check whether we have a hard evidence (and also check whether the
    // potential only contains 0 (in this case, this will automatically raise
    // an exception) )
    Idx val;
    bool is_hard_evidence = __isHardEvidence ( pot, val );

    // modify the evidence already stored
    const Potential<GUM_SCALAR>* localPot = __evidence[id];
    Instantiation I( pot );
    for ( I.setFirst(); !I.end(); I.inc() ) {
      localPot->set( I, pot[I] );
    }

    // the inference state will be different
    // whether evidence change from Hard to Soft or not.
    bool hasChangedSoftHard = false;

    if ( is_hard_evidence ) {
      if ( ! hasHardEvidence( id ) ) {
        hasChangedSoftHard = true;
        __hard_evidence.insert( id, val );
        __hard_evidence_nodes.insert ( id );
        __soft_evidence_nodes.erase ( id );
      }
      else {
        __hard_evidence[id] = val;
      }
    }
    else {
      if ( hasHardEvidence( id ) ) {  // evidence was hard
        __hard_evidence.erase( id );
        __hard_evidence_nodes.erase( id );
        __soft_evidence_nodes.insert ( id );
        hasChangedSoftHard = true;
      }
    }
    
    if ( hasChangedSoftHard ) {
      __state = StateOfInference::UnpreparedStructure;
    }
    else {
      // if status = UnpreparedStructure, remain in that state, otherwise,
      // go into an outdatedPotential state
      if ( __state != StateOfInference::UnpreparedStructure )
        __state = StateOfInference::OutdatedPotentials;
    }

    _onEvidenceChanged ( id, hasChangedSoftHard );
  }

  
  // removed the evidence, if any, corresponding to node id
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::eraseEvidence ( NodeId id ) {
    if ( hasEvidence( id ) ) {
      if ( hasHardEvidence( id ) ) {
        _onEvidenceErased ( id, true );
        __hard_evidence.erase ( id );
        __hard_evidence_nodes.erase ( id );
        __state = StateOfInference::UnpreparedStructure;
      }
      else {
        _onEvidenceErased ( id, false );
        __soft_evidence_nodes.erase ( id );
        if ( __state != StateOfInference::UnpreparedStructure )
          __state = StateOfInference::OutdatedPotentials;
      }

      delete ( __evidence[id] );
      __evidence.erase( id );
    }
  }


  // removes all the evidence entered into the network
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::eraseAllEvidence() {
    bool has_hard_evidence = ! __hard_evidence.empty ();
    this->_onAllEvidenceErased ( has_hard_evidence );

    for ( const auto& pair : __evidence ) {
      if ( pair.second != nullptr ) {
        delete ( pair.second );
      }
    }

    __evidence.clear ();
    __hard_evidence.clear ();
    __hard_evidence_nodes.clear ();
    __soft_evidence_nodes.clear ();

    if ( has_hard_evidence ||
         ( __state == StateOfInference::UnpreparedStructure ) )
      __state = StateOfInference::UnpreparedStructure;
    else
      __state = StateOfInference::OutdatedPotentials;
  }
  

  // returns the number of evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrEvidence () const {
    return __evidence.size();
  }


  // returns the number of hard evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrHardEvidence () const {
    return __hard_evidence_nodes.size();
  }
  

  // returns the number of soft evidence entered into the Bayesian network
  template <typename GUM_SCALAR>
  INLINE Size Inference<GUM_SCALAR>::nbrSoftEvidence () const {
    return __soft_evidence_nodes.size();
  }


  // indicate for each node with hard evidence which value it took
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<Idx>&
  Inference<GUM_SCALAR>::hardEvidence () const {
    return __hard_evidence;
  }

  
  // the set of evidence entered into the network
  template <typename GUM_SCALAR>
  INLINE const NodeProperty<const Potential<GUM_SCALAR>*>&
  Inference<GUM_SCALAR>::evidence () const {
    return __evidence;
  }
  

  /// the set of nodes that received soft evidence
  template <typename GUM_SCALAR>
  INLINE const NodeSet&
  Inference<GUM_SCALAR>::softEvidenceNodes () const {
    return __soft_evidence_nodes;
  }
  
  
  /// the set of nodes that received hard evidence
  template <typename GUM_SCALAR>
  INLINE const NodeSet&
  Inference<GUM_SCALAR>::hardEvidenceNodes () const {
    return __hard_evidence_nodes;
  }
  


  
  // ##############################################################################
  // Inference
  // ##############################################################################

  // put the inference into an unprepared state
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::_setUnpreparedStructureState () {
    __state = StateOfInference::UnpreparedStructure;
  }

  
  /** puts the inference into an OutdatedPotentials state if it is not
   *  already in an UnpreparedStructure state */
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::_setOutdatedPotentialsState () {
    if ( __state != StateOfInference::UnpreparedStructure )
      __state = StateOfInference::OutdatedPotentials;
  }


  // prepare the internal inference structures for the next inference
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::prepareInference () {
    if ( isReady4Inference () || isDone () ) {
      return;
    }

    if ( __bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    if ( __state == StateOfInference::UnpreparedStructure )
      _prepareInferenceStructure ();
    else
      _updateInferencePotentials ();
    
    __state = StateOfInference::Ready4Inference;
  }


  // perform the heavy computations needed to compute the targets' posteriors
  template <typename GUM_SCALAR>
  INLINE void Inference<GUM_SCALAR>::makeInference () {
    if ( isDone () ) {
      return;
    }

    if ( ! isReady4Inference () ) {
      prepareInference ();
    }

    _makeInference();

    __state = StateOfInference::Done;
  }


} /* namespace gum */

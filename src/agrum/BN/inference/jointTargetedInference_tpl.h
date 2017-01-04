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
 * @brief Implementation of the non pure virtual methods of class
 * JointTargetedInference.
 */


namespace gum {


  // Default Constructor
  template <typename GUM_SCALAR>
  JointTargetedInference<GUM_SCALAR>::JointTargetedInference(
      const IBayesNet<GUM_SCALAR>* bn )
      : MarginalTargetedInference<GUM_SCALAR>( bn ) {
    // assign a BN if this has not been done before (due to virtual inheritance)
    if ( this->__bn == nullptr ) {
      BayesNetInference<GUM_SCALAR>::__setBayesNetDuringConstruction( bn );
    }
    GUM_CONSTRUCTOR( JointTargetedInference );
  }


  // Destructor
  template <typename GUM_SCALAR>
  JointTargetedInference<GUM_SCALAR>::~JointTargetedInference() {
    GUM_DESTRUCTOR( JointTargetedInference );
  }


  // assigns a new BN to the inference engine
  template <typename GUM_SCALAR>
  void JointTargetedInference<GUM_SCALAR>::_onBayesNetChanged(
      const IBayesNet<GUM_SCALAR>* bn ) {
    MarginalTargetedInference<GUM_SCALAR>::_onBayesNetChanged( bn );
    _onAllJointTargetsErased();
    __joint_targets.clear();
  }


  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if target is a nodeset target.
  template <typename GUM_SCALAR>
  INLINE bool
  JointTargetedInference<GUM_SCALAR>::isJointTarget( const NodeSet& vars ) const {
    if ( this->__bn == nullptr )
      GUM_ERROR( NullElement,
                 "No Bayes net has been assigned to the "
                 "inference algorithm" );

    const auto& dag = this->__bn->dag();
    for ( const auto var : vars ) {
      if ( !dag.exists( var ) ) {
        GUM_ERROR( UndefinedElement, var << " is not a NodeId in the bn" );
      }
    }

    return __joint_targets.contains( vars );
  }


  // Clear all previously defined single targets
  template <typename GUM_SCALAR>
  INLINE void JointTargetedInference<GUM_SCALAR>::eraseAllMarginalTargets() {
    MarginalTargetedInference<GUM_SCALAR>::eraseAllTargets();
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template <typename GUM_SCALAR>
  INLINE void JointTargetedInference<GUM_SCALAR>::eraseAllJointTargets() {
    _onAllJointTargetsErased();
    __joint_targets.clear();
    this->__state =
        BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
  }


  // Clear all previously defined targets (single and joint targets)
  template <typename GUM_SCALAR>
  INLINE void JointTargetedInference<GUM_SCALAR>::eraseAllTargets() {
    eraseAllMarginalTargets();
    eraseAllJointTargets();
  }


  // Add a set of nodes as a new target
  template <typename GUM_SCALAR>
  void JointTargetedInference<GUM_SCALAR>::addJointTarget(
      const NodeSet& joint_target ) {
    // check if the nodes in the target belong to the Bayesian network
    if ( this->__bn == nullptr )
      GUM_ERROR( NullElement,
                 "No Bayes net has been assigned to the "
                 "inference algorithm" );

    const auto& dag = this->__bn->dag();
    for ( const auto node : joint_target ) {
      if ( !dag.exists( node ) ) {
        GUM_ERROR( UndefinedElement,
                   "at least one one in " << joint_target
                                          << " does not belong to the bn" );
      }
    }

    // check that the joint_target set does not contain the new target
    if ( __joint_targets.contains( joint_target ) ) return;

    // check if joint_target is a subset of an already existing target
    for ( const auto& target : __joint_targets ) {
      if ( target.isSupersetOf( joint_target ) ) return;
    }

    // check if joint_target is not a superset of an already existing target
    // in this case, we need to remove old existing target
    for ( auto iter = __joint_targets.beginSafe();
          iter != __joint_targets.endSafe();
          ++iter ) {
      if ( iter->isSubsetOf( joint_target ) ) eraseJointTarget( *iter );
    }

    __joint_targets.insert( joint_target );
    _onJointTargetAdded( joint_target );
    this->__state =
        BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
  }


  // removes an existing set target
  template <typename GUM_SCALAR>
  void JointTargetedInference<GUM_SCALAR>::eraseJointTarget(
      const NodeSet& joint_target ) {
    // check if the nodes in the target belong to the Bayesian network
    if ( this->__bn == nullptr )
      GUM_ERROR( NullElement,
                 "No Bayes net has been assigned to the "
                 "inference algorithm" );

    const auto& dag = this->__bn->dag();
    for ( const auto node : joint_target ) {
      if ( !dag.exists( node ) ) {
        GUM_ERROR( UndefinedElement,
                   "at least one one in " << joint_target
                                          << " does not belong to the bn" );
      }
    }

    // check that the joint_target set does not contain the new target
    if ( __joint_targets.contains( joint_target ) ) {
      _onJointTargetErased( joint_target );
      __joint_targets.erase( joint_target );
      this->__state =
          BayesNetInference<GUM_SCALAR>::StateOfInference::OutdatedBNStructure;
    }
  }


  /// returns the list of target sets
  template <typename GUM_SCALAR>
  INLINE const Set<NodeSet>&
               JointTargetedInference<GUM_SCALAR>::jointTargets() const noexcept {
    return __joint_targets;
  }

  /// returns the number of target sets
  template <typename GUM_SCALAR>
  INLINE Size JointTargetedInference<GUM_SCALAR>::nbrJointTargets() const
      noexcept {
    return __joint_targets.size();
  }


  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a nodeset.
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  JointTargetedInference<GUM_SCALAR>::jointPosterior( const NodeSet& vars ) {
    // try to get the smallest set of targets that contains "vars"
    NodeSet set;
    bool    found_exact_target;

    if ( __joint_targets.contains( vars ) ) {
      set = vars;
      found_exact_target = true;
    } else {
      for ( const auto& target : __joint_targets ) {
        if ( vars.isSubsetOf( target ) ) {
          set = target;
          found_exact_target = false;
          break;
        }
      }
    }

    if ( set.empty() ) {
      GUM_ERROR( UndefinedElement,
                 " no joint target containing " << vars << "could be found" );
    }

    if ( !this->isDone() ) {
      this->makeInference();
    }

    if ( found_exact_target )
      return _jointPosterior( vars );
    else
      return _jointPosterior( vars, set );
  }


  // Compute the posterior of a node
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  JointTargetedInference<GUM_SCALAR>::posterior( const NodeId node ) {
    if ( this->targets().contains( node ) )
      return MarginalTargetedInference<GUM_SCALAR>::posterior( node );
    else
      return jointPosterior( NodeSet{node} );
  }

  // Compute the posterior of a node
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  JointTargetedInference<GUM_SCALAR>::posterior( const std::string& nodeName ) {
    return posterior(this->BN().idFromName( nodeName ) );
  }

  // ##############################################################################
  // Entropy
  // ##############################################################################


  /* Mutual information between X and Y
   * @see http://en.wikipedia.org/wiki/Mutual_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  GUM_SCALAR JointTargetedInference<GUM_SCALAR>::I( const NodeId X,
                                                    const NodeId Y ) {
    Potential<GUM_SCALAR> pX, pY, *pXY = nullptr;

    try {
      // here use unnormalized joint posterior rather than just posterior
      // to avoid saving the posterior in the cache of the inference engines
      // like LazyPropagation or SahferShenoy.
      pXY = this->_unnormalizedJointPosterior( {X, Y} );
      pXY->normalize();
      if ( X != Y ) {
        pX = pXY->margSumOut( {&(this->BN().variable( Y ) )} );
        pY = pXY->margSumOut( {&(this->BN().variable( X ) )} );
      } else {
        pX = *pXY;
        pY = *pXY;
      }
    } catch ( ... ) {
      if ( pXY != nullptr ) {
        delete pXY;
        pXY = nullptr;
      }
      throw;
    }

    Instantiation i( *pXY );
    GUM_SCALAR    res = (GUM_SCALAR)0;

    for ( i.setFirst(); !i.end(); ++i ) {
      GUM_SCALAR vXY = ( *pXY )[i];
      GUM_SCALAR vX = pX[i];
      GUM_SCALAR vY = pY[i];

      if ( vXY > (GUM_SCALAR)0 ) {
        if ( vX == (GUM_SCALAR)0 || vY == (GUM_SCALAR)0 ) {
          GUM_ERROR( OperationNotAllowed,
                     "Mutual Information (X,Y) with P(X)=0 or P(Y)=0 "
                     "and P(X,Y)>0" );
        }

        res += vXY * ( log2( vXY ) - log2( vX ) - log2( vY ) );
      }
    }

    delete pXY;

    return res;
  }


  /** Variation of information between X and Y
   * @see http://en.wikipedia.org/wiki/Variation_of_information
   *
   * @warning Due to limitation of @joint, may not be able to compute this value
   * @throw OperationNotAllowed in these cases
   */
  template <typename GUM_SCALAR>
  INLINE GUM_SCALAR JointTargetedInference<GUM_SCALAR>::VI( const NodeId X,
                                                            const NodeId Y ) {
    return this->H( X ) + this->H( Y ) - 2 * I( X, Y );
  }


} /* namespace gum */

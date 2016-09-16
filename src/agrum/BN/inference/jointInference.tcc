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
 * @brief Implementation of the non pure virtual methods of class JointInference.
 */


namespace gum {

  
  // Default Constructor
  template <typename GUM_SCALAR>
  JointInference<GUM_SCALAR>::JointInference ( const IBayesNet<GUM_SCALAR>* bn ) :
    Inference<GUM_SCALAR> ( bn ) {
    GUM_CONSTRUCTOR( JointInference );
  }

  
  // Destructor
  template <typename GUM_SCALAR> JointInference<GUM_SCALAR>::~JointInference () {
    GUM_DESTRUCTOR( JointInference );
  }
  

  // assigns a new BN to the inference engine
  template <typename GUM_SCALAR>
  void
  JointInference<GUM_SCALAR>::setBayesNet ( const IBayesNet<GUM_SCALAR>* bn ) {
    _onAllJointTargetsErased ();
    __joint_targets.clear ();
    Inference<GUM_SCALAR>::setBayesNet ( bn );
  }

   
  
  // ##############################################################################
  // Targets
  // ##############################################################################

  // return true if target is a nodeset target.
  template <typename GUM_SCALAR>
  INLINE bool
  JointInference<GUM_SCALAR>::isJointTarget( const NodeSet& vars ) const {
    if ( this->__bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    const auto& dag = this->__bn->dag ();
    for ( const auto var : vars ) {
      if ( ! dag.exists( var ) ) {
        GUM_ERROR( UndefinedElement, var << " is not a NodeId in the bn" );
      }
    }

    return __joint_targets.contains( vars );
  }


  // Clear all previously defined targets (single and joint targets)
  template <typename GUM_SCALAR>
  INLINE void JointInference<GUM_SCALAR>::eraseAllTargets () {
    this->_onAllTargetsErased ();
    this->__targets.clear ();
    this->__joint_targets.clear();
    this->__state = Inference<GUM_SCALAR>::StateOfInference::UnpreparedStructure;
  }


  // Clear all previously defined single targets
  template <typename GUM_SCALAR>
  INLINE void JointInference<GUM_SCALAR>::eraseAllSingleTargets () {
    Inference<GUM_SCALAR>::eraseAllTargets ();
  }


  // Clear all previously defined targets (single targets and sets of targets)
  template <typename GUM_SCALAR>
  INLINE void JointInference<GUM_SCALAR>::eraseAllJointTargets () {
    this->_onAllJointTargetsErased ();
    __joint_targets.clear();
    this->__state = Inference<GUM_SCALAR>::StateOfInference::UnpreparedStructure;
  }


  // Add a set of nodes as a new target
  template <typename GUM_SCALAR>
  void JointInference<GUM_SCALAR>::addJointTarget ( const NodeSet& joint_target ) {
    // check if the nodes in the target belong to the Bayesian network
    if ( this->__bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    const auto& dag = this->__bn->dag ();
    for ( const auto node : joint_target ) {
      if ( ! dag.exists( node ) ) {
        GUM_ERROR( UndefinedElement, "at least one one in " << joint_target
                   << " does not belong to the bn" );
      }
    }

    // check that the joint_target set does not contain the new target
    if ( ! __joint_targets.contains ( joint_target ) ) {
      __joint_targets.insert ( joint_target );
      _onJointTargetAdded ( joint_target );
      this->__state = Inference<GUM_SCALAR>::StateOfInference::UnpreparedStructure;
    }
  }

  
  // removes an existing set target
  template <typename GUM_SCALAR>
  void
  JointInference<GUM_SCALAR>::eraseJointTarget ( const NodeSet& joint_target ) {
    // check if the nodes in the target belong to the Bayesian network
    if ( this->__bn == nullptr )
      GUM_ERROR ( NullElement, "No Bayes net has been assigned to the "
                  "inference algorithm" );

    const auto& dag = this->__bn->dag ();
    for ( const auto node : joint_target ) {
      if ( ! dag.exists( node ) ) {
        GUM_ERROR( UndefinedElement, "at least one one in " << joint_target
                   << " does not belong to the bn" );
      }
    }

    // check that the joint_target set does not contain the new target
    if ( __joint_targets.contains ( joint_target ) ) {
      _onJointTargetErased ( joint_target );
      __joint_targets.erase ( joint_target );
      this->__state = Inference<GUM_SCALAR>::StateOfInference::UnpreparedStructure;
    }
  }
  
  
  /// returns the list of target sets
  template <typename GUM_SCALAR>
  INLINE const Set<NodeSet>&
  JointInference<GUM_SCALAR>::jointTargets () const noexcept {
    return __joint_targets;
  }


  
  // ##############################################################################
  // Inference
  // ##############################################################################

  // Compute the posterior of a nodeset.
  template <typename GUM_SCALAR>
  const Potential<GUM_SCALAR>&
  JointInference<GUM_SCALAR>::jointPosterior( const NodeSet& vars ) {
    if ( ! __joint_targets.contains ( vars ) ) {
        GUM_ERROR( UndefinedElement, vars << " is not a joint target" );
    }

    if ( ! this->isDone () ) {
      this->makeInference();
    }

    return _jointPosterior ( vars );
  }


} /* namespace gum */


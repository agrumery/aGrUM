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
 * @brief Template implementation of BN/BayesNetFragment.h classes.
 *
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 */
#include <agrum/BN/BayesNetFragment.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/multidim/potential.h>

namespace gum {
  template<typename GUM_SCALAR>
  BayesNetFragment<GUM_SCALAR>::BayesNetFragment ( const BayesNet<GUM_SCALAR>& bn ) :
    DiGraphListener ( &bn.dag() ),
    __bn ( bn ) {
    GUM_CONSTRUCTOR ( BayesNetFragment );
  }

  template<typename GUM_SCALAR>
  BayesNetFragment<GUM_SCALAR>::~BayesNetFragment() {
    GUM_DESTRUCTOR ( BayesNetFragment );

    for ( const auto id : nodes() )
      _uninstallCPT ( id );
  }

  //============================================================
  // signals to keep consistency with the referred BayesNet
  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::whenNodeAdded ( const void* src, NodeId id ) noexcept {
    // nothing to do
  }
  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::whenNodeDeleted ( const void* src, NodeId id ) noexcept {
    uninstallNode ( id );
  }
  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::whenArcAdded ( const void* src, NodeId from, NodeId to ) noexcept {
    //nothing to do
  }
  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::whenArcDeleted ( const void* src, NodeId from, NodeId to ) noexcept {
    _uninstallArc ( from, to );
  }

  //============================================================
  // IBayesNet interface : BayesNetFragment here is a decorator for the bn

  template<typename GUM_SCALAR> INLINE const Potential<GUM_SCALAR>&
  BayesNetFragment<GUM_SCALAR>::cpt ( NodeId id ) const {
    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound, id << " is not installed" );

    if ( __localCPTs.exists ( id ) )
      return *__localCPTs[id];
    else
      return __bn.cpt ( id );
  }

  template<typename GUM_SCALAR>  INLINE const VariableNodeMap&
  BayesNetFragment<GUM_SCALAR>::variableNodeMap() const {
    GUM_ERROR ( FatalError, "Not implemented yet. please use referent bayesnet method" );
  }

  template<typename GUM_SCALAR>  INLINE const DiscreteVariable&
  BayesNetFragment<GUM_SCALAR>::variable ( NodeId id ) const {
    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound, id << " is not installed" );

    return __bn.variable ( id );
  }

  template<typename GUM_SCALAR> INLINE  NodeId
  BayesNetFragment<GUM_SCALAR>::nodeId ( const DiscreteVariable& var ) const {
    NodeId id = __bn.nodeId ( var );

    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound, "variable " << var.name() << " is not installed" );

    return id;
  }

  template<typename GUM_SCALAR> INLINE NodeId
  BayesNetFragment<GUM_SCALAR>::idFromName ( const std::string& name ) const {
    NodeId id = __bn.idFromName ( name );

    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound, "variable " << name << " is not installed" );

    return id;
  }

  template<typename GUM_SCALAR> INLINE const DiscreteVariable&
  BayesNetFragment<GUM_SCALAR>::variableFromName ( const std::string& name ) const {
    NodeId id = __bn.idFromName ( name );

    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound, "variable " << name << " is not installed" );

    return __bn.variable ( id );
  }


  //============================================================
  // specific API for BayesNetFragment
  template<typename GUM_SCALAR> INLINE bool
  BayesNetFragment<GUM_SCALAR>::isInstalledNode ( NodeId id ) const noexcept {
    return this->_dag.existsNode ( id );
  }

  template<typename GUM_SCALAR> void
  BayesNetFragment<GUM_SCALAR>::installNode ( NodeId id ) {
    if ( ! __bn.dag().existsNode ( id ) )
      GUM_ERROR ( NotFound, "Node " << id << " does not exist in referred BayesNet" );

    if ( !isInstalledNode ( id ) ) {
      this->_dag.insertNode ( id );

      // adding arcs with id as a tail
      for ( const auto pa : this->__bn.dag().parents ( id ) ) {
        if ( isInstalledNode ( pa ) )
          this->_dag.insertArc ( pa, id );
      }

      //addin arcs with id as a head
      for ( const auto son : this->__bn.dag().children ( id ) )
        if ( isInstalledNode ( son ) )
          this->_dag.insertArc ( id, son );
    }
  }

  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::installAscendants ( NodeId id ) {
    installNode ( id );

    // bn is a dag => this will have an end ...
    for ( const auto pa : this->__bn.dag().parents ( id ) )
      installAscendants ( pa );
  }

  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::uninstallNode ( NodeId id ) noexcept {
    if ( isInstalledNode ( id ) ) {
      this->_dag.eraseNode ( id );
      _uninstallCPT ( id );
    }
  }

  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::_uninstallArc ( NodeId from, NodeId to ) noexcept {
    if ( this->_dag.existsArc ( from, to ) ) {
      this->_dag.eraseArc ( Arc ( from, to ) );
    }

    _uninstallCPT ( to );
  }

  template<typename GUM_SCALAR> void
  BayesNetFragment<GUM_SCALAR>::_installCPT ( NodeId id, const Potential<GUM_SCALAR>* pot ) noexcept {
    // topology
    for ( const auto node : this->_dag.parents ( id ) )
      this->_dag.eraseArc ( Arc ( node, id ) );

    for ( Idx i = 1; i < pot->nbrDim(); i++ ) {
      this->_dag.insertArc ( idFromName ( pot->variable ( i ).name() ), id );
    }

    //local cpt
    _uninstallCPT ( id );
    __localCPTs.insert ( id, pot );
  }

  template<typename GUM_SCALAR> INLINE void
  BayesNetFragment<GUM_SCALAR>::_uninstallCPT ( NodeId id ) noexcept {
    if ( __localCPTs.exists ( id ) ) {
      delete __localCPTs[id];
    }
  }

  template<typename GUM_SCALAR> void
  BayesNetFragment<GUM_SCALAR>::installMarginal ( NodeId id, const Potential<GUM_SCALAR>* pot ) {
    if ( ! isInstalledNode ( id ) ) {
      GUM_ERROR ( NotFound,
                  "The node " << id << " is not part of this fragment" );
    }

    if ( pot->nbrDim() > 1 ) {
      GUM_ERROR ( OperationNotAllowed,
                  "The potential is not a marginal :" << pot );
    }

    if ( pot->variable ( 0 ).name() != variable ( id ).name() ) {
      GUM_ERROR ( OperationNotAllowed,
                  "The potential is not a marginal for variable <" << variable ( id ).name() << ">" );
    }

    _installCPT ( id, pot );
  }

  template<typename GUM_SCALAR> bool
  BayesNetFragment<GUM_SCALAR>::checkConsistency ( NodeId id ) {
    if ( ! isInstalledNode ( id ) )
      GUM_ERROR ( NotFound,
                  "The node " << id << " is not part of this fragment" );

    const auto& parents = this->_dag.parents ( id );
    const auto& cpt = this->cpt ( id );
    NodeSet cpt_parents;

    for ( Idx i = 1; i < cpt.nbrDim(); i++ )
      cpt_parents.insert ( __bn.idFromName ( cpt.variable ( i ).name() ) );

    return parents == cpt_parents;
  }

  template<typename GUM_SCALAR> INLINE bool
  BayesNetFragment<GUM_SCALAR>::checkConsistency () noexcept{
    for ( const auto node : nodes() )
      if ( ! checkConsistency ( node ) ) return false;

    return true;
  }
}// gum


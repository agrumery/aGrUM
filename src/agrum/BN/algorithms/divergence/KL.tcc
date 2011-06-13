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
 * @brief KL divergence between BNs implementation
 *
 * @author Pierre-Henri Wuillemin
 */
// ============================================================================
#include <math.h>
#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/BayesNet.h>

namespace gum {
  template<typename T_DATA>
  KL<T_DATA>::KL() {
    GUM_ERROR ( OperationNotAllowed, "No default constructor for algorithms" );
    GUM_CONSTRUCTOR ( KL );
  }

  template<typename T_DATA>
  KL<T_DATA>::KL ( const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q ) :
      _p ( P ),_q ( Q ),
      _klPQ ( 0.0 ),_klQP ( 0.0 ),_errorPQ ( false ),_errorQP ( false ),
      __difficulty ( complexity::HEAVY ),__done ( false ) {
    __checkCompatibility(); //may throw OperationNotAlloxed
    GUM_CONSTRUCTOR ( KL );
  }

  template<typename T_DATA>
  KL<T_DATA>::KL ( const KL<T_DATA>& kl ) :
      _p ( kl._p ),_q ( kl._q ),
      _klPQ ( kl._klPQ ),_klQP ( kl._klQP ),_errorPQ ( kl._errorPQ ),_errorQP ( kl._errorPQ ),
      __difficulty ( kl.__difficulty ),__done ( kl.__done ) {
    GUM_CONSTRUCTOR ( KL );
  }

  template<typename T_DATA>
  KL<T_DATA>::~KL() {
    GUM_DESTRUCTOR ( KL );
  }

  template<typename T_DATA>
  complexity::difficulty
  KL<T_DATA>::difficulty() const {
      double diff=_p.log10DomainSize();

      if ( diff<GAP_difficult_correct ) return complexity::CORRECT;

      if ( diff<GAP_heavy_difficult ) return complexity::DIFFICULT;

      return complexity::HEAVY;
    }

  template<typename T_DATA> INLINE
  double
  KL<T_DATA>::klPQ() {
    _process();
    return _klPQ;
  }

  template<typename T_DATA> INLINE
  double
  KL<T_DATA>::klQP() {
    _process();
    return _klQP;
  }

  template<typename T_DATA> INLINE
  double
  KL<T_DATA>::hellinger() {
    _process();
    return _hellinger;
  }

  template<typename T_DATA> INLINE
  bool
  KL<T_DATA>::errorPQ() {
    _process();
    return _errorPQ;
  }

  template<typename T_DATA> INLINE
  bool
  KL<T_DATA>::errorQP() {
    _process();
    return _errorQP;
  }

  template<typename T_DATA> INLINE
  const BayesNet<T_DATA>&
  KL<T_DATA>::p ( void ) const {
    return _p;
  }

  template<typename T_DATA> INLINE
  const BayesNet<T_DATA>&
  KL<T_DATA>::q ( void ) const {
    return _q;
  }

// check if the 2 BNs are compatible
  template<typename T_DATA>
  bool
  KL<T_DATA>::__checkCompatibility() const {

      if ( _p.size() !=_q.size() ) GUM_ERROR ( OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same size)" );

      if ( _p.log10DomainSize() !=_q.log10DomainSize() ) GUM_ERROR ( OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same domainSize)" );

      for ( DAG::NodeIterator it=_p.beginNodes();it!=_p.endNodes();++it ) {
          const DiscreteVariable& vp=_p.variable ( *it );

          try {
              const DiscreteVariable& vq=_q.variableFromName ( vp.name() );

              if ( vp.domainSize() !=vq.domainSize() ) GUM_ERROR ( OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same domainSize for"+vp.name() +")" );

              for ( Id i=0;i<vp.domainSize();i++ ) {
                  try {
                      vq[vp.label ( i ) ];

                    } catch ( OutOfBounds& e ) {

                      GUM_ERROR ( OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same labels for "+vp.name() +")" );
                    }
                }
            } catch ( NotFound& e ) {

              GUM_ERROR ( OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same vars : "+vp.name() +")" );
            }
        }

      return true;
    }

// do the job if not already __done
  template<typename T_DATA>
  void
  KL<T_DATA>::_process() {
    if ( ! __done ) {
        _computeKL();
        __done=true;
      }
  }

  // we are certain that Iq and Ip consist of variables with the same names and with the same labels.
  // But the order may be different ... :(
  template<typename T_DATA>
  void
  KL<T_DATA>::__synchroInstantiations ( Instantiation& Iq,const Instantiation& Ip ) const {
      for ( Idx i=0;i<Ip.nbrDim();i++ ) {
          const std::string& v_name=Ip.variable ( i ).name();
          const std::string& v_label=Ip.variable ( i ).label ( Ip.val ( i ) );
          const DiscreteVariable& vq=_q.variableFromName ( v_name );
          Iq.chgVal ( vq,vq[v_label] );
        }
    }

  // in order to keep KL instantiable
  template<typename T_DATA>
  void KL<T_DATA>::_computeKL() {
    GUM_ERROR ( OperationNotAllowed, "No default computations" );
  }
} //namespace gum
// kate: indent-mode cstyle; space-indent on; indent-width 2; 

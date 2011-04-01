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
#include <agrum/BN/algorithms/divergence/defaultKL.h>
#include <agrum/BN/BayesNet.h>

namespace gum {
  template<typename T_DATA>
  DefaultKL<T_DATA>::DefaultKL() {
    GUM_ERROR (OperationNotAllowed, "No default constructor for algorithms");
    GUM_CONSTRUCTOR (DefaultKL);
  }

  template<typename T_DATA>
  DefaultKL<T_DATA>::DefaultKL (const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q) :
      _p (P),_q (Q),
      _klPQ (0.0),_klQP (0.0),_errorPQ (false),_errorQP (false),
      __difficulty (KL::HEAVY),__done (false) {
    __checkCompatibility(); //may throw OperationNotAlloxed
    GUM_CONSTRUCTOR (DefaultKL);
  }

  template<typename T_DATA>
  DefaultKL<T_DATA>::DefaultKL (const DefaultKL<T_DATA>& kl) :
      _p (kl._p),_q (kl._q),
      _klPQ (kl._klPQ),_klQP (kl._klQP),_errorPQ (kl._errorPQ),_errorQP (kl._errorPQ),
      __difficulty (kl.__difficulty),__done(kl.__done) {
    GUM_CONSTRUCTOR (DefaultKL);
  }

  template<typename T_DATA>
  DefaultKL<T_DATA>::~DefaultKL() {
    GUM_DESTRUCTOR (DefaultKL);
  }

  template<typename T_DATA>
  KL::difficulty
  DefaultKL<T_DATA>::difficulty() const {
    double diff=_p.log10DomainSize();

    if (diff<GAP_difficult_correct) return KL::CORRECT;

    if (diff<GAP_heavy_difficult) return KL::DIFFICULT;

    return KL::HEAVY;
  }

  template<typename T_DATA> INLINE
  double
  DefaultKL<T_DATA>::klPQ() {
    _process();
    return _klPQ;
  }

  template<typename T_DATA> INLINE
  double
  DefaultKL<T_DATA>::klQP() {
    _process();
    return _klQP;
  }

  template<typename T_DATA> INLINE
  bool
  DefaultKL<T_DATA>::errorPQ() {
    _process();
    return _errorPQ;
  }

  template<typename T_DATA> INLINE
  bool
  DefaultKL<T_DATA>::errorQP() {
    _process();
    return _errorQP;
  }

// check if the 2 BNs are compatible
  template<typename T_DATA>
  bool
  DefaultKL<T_DATA>::__checkCompatibility() const {

    if (_p.size() !=_q.size()) GUM_ERROR (OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same size)");

    if (_p.log10DomainSize() !=_q.log10DomainSize()) GUM_ERROR (OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same domainSize)");

    for (DAG::NodeIterator it=_p.beginNodes();it!=_p.endNodes();++it) {
      const DiscreteVariable& vp=_p.variable (*it);

      try {
        const DiscreteVariable& vq=_q.variableFromName (vp.name());

        if (vp.domainSize() !=vq.domainSize()) GUM_ERROR (OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same domainSize for"+vp.name() +")");

        for (Id i=0;i<vp.domainSize();i++) {
          try {
            vq[vp.label (i) ];

          } catch (OutOfBounds& e) {

            GUM_ERROR (OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same labels for "+vp.name() +")");
          }
        }
      } catch (NotFound& e) {

        GUM_ERROR (OperationNotAllowed, "KL : the 2 BNs are not compatible (not the same vars : "+vp.name() +")");
      }
    }

    return true;
  }

// do the job if not already __done
  template<typename T_DATA>
  void
  DefaultKL<T_DATA>::_process() {
    if (! __done) {
      _computeKL();
      __done=true;
    }
  }

  template<typename T_DATA>
  void
  DefaultKL<T_DATA>::_computeKL (void) {
    GUM_ERROR (OperationNotAllowed,"No default KL computation in abstractKL");
  }
} //namespace gum

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
 * @brief KL divergence between BNs brute force implementation
 *
 * @author Pierre-Henri Wuillemin
 */
// ============================================================================
#include <math.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/bruteForceKL.h>

namespace gum {
  // we are certain that Iq and Ip consist of variables with the same names and with the same labels.
  // But the order may be different ... :(
  template<typename T_DATA>
  void
  BruteForceKL<T_DATA>::__synchroInstantiations (Instantiation& Iq,const Instantiation& Ip) const {
    for (Idx i=0;i<Ip.nbrDim();i++) {
      const std::string& v_name=Ip.variable (i).name();
      const std::string& v_label=Ip.variable (i).label (Ip.val (i));
      const DiscreteVariable& vq=_q.variableFromName (v_name);
      Iq.chgVal (vq,vq[v_label]);
    }
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::BruteForceKL() : DefaultKL<T_DATA>() {
    GUM_CONSTRUCTOR (BruteForceKL);
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::BruteForceKL (const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q) :DefaultKL<T_DATA> (P,Q) {
    GUM_CONSTRUCTOR (BruteForceKL);
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::BruteForceKL (const DefaultKL< T_DATA >& kl) :DefaultKL<T_DATA> (kl) {
    GUM_CONSTRUCTOR (BruteForceKL);
  }

  template<typename T_DATA>
  BruteForceKL<T_DATA>::~BruteForceKL() {
    GUM_DESTRUCTOR (BruteForceKL);
  }

  template<typename T_DATA>
  void BruteForceKL<T_DATA>::_computeKL() {
    _klPQ=_klQP=0.0;
    _errorPQ=_errorQP=0;

    gum::Instantiation Ip=_p.completeInstantiation();
    gum::Instantiation Iq=_q.completeInstantiation();

    for (Ip.setFirst();! Ip.end();++Ip) {
      __synchroInstantiations (Iq,Ip);
      T_DATA pp=_p.jointProbability (Ip);
      T_DATA pq=_q.jointProbability (Iq);

      if (pp!=0.0) {
        if (pq!=0.0) {
          _klPQ+=pp*log (pq/pp);
        } else {
          _errorPQ++;
        }
      }

      if (pq!=0.0) {
        if (pp!=0.0) {
          _klQP+=pq*log (pp/pq);
        } else {
          _errorQP++;
        }
      }
    }

    _klPQ=-_klPQ/log (2);

    _klQP=-_klQP/log (2);
  }

} // namespace gum

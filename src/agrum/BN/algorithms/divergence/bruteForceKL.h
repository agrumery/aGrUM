/***************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
* @brief algorithm for exact computation KL divergence between BNs
*
* @author Pierre-Henri WUILLEMIN
*
*/
#ifndef GUM_BRUTE_FORCE_KL_H
#define GUM_BRUTE_FORCE_KL_H

#include <agrum/BN/algorithms/divergence/KL.h>

namespace gum {

  /**
  * BruteForceKL computes exactly the KL divergence betweens 2 BNs.
  *
  * BruteForceKL should be used only if difficulty() gives an estimation ( KL_CORRECT
  *) of the needed time.
  * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The
  *computations are made once. The second is for free :)
  * BruteForce allows as well to compute in the same time the Hellinger distance (\f$
  *\sqrt{\sum_i (\sqrt{p_i}-\sqrt{q_i})^2}\f$) (Kokolakis and Nanopoulos, 2001).
  *
  * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a case, KL
  *keeps working but trace this error (errorPQ() and errorQP())?  *
  *
  * @warning This BruteForceKL should be use only if
  *difficulty()==complexity::CORRECT or at most complexity::DIFFICULT ...
  * snippets :
  * @code
  * gum::KL base_kl(net1,net2);
  * if (base_kl.difficulty()!=KL::HEAVY) {
  *  gum::BruteForceKL kl(base_kl);
  *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
  * } else {
  *  gum::GibbsKL kl(base_kl);
  *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
  * }
  * @endcode
  */

  template <typename GUM_SCALAR> class BruteForceKL : public KL<GUM_SCALAR> {
    public:
    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize or
     * compatible node sets.
     */
    BruteForceKL(const IBayesNet<GUM_SCALAR> &P, const IBayesNet<GUM_SCALAR> &Q);

    /** copy constructor
     */
    BruteForceKL(const KL<GUM_SCALAR> &kl);

    /** destructor */
    virtual ~BruteForceKL();

    protected:
    void _computeKL(void);

    using KL<GUM_SCALAR>::_p;
    using KL<GUM_SCALAR>::_q;
    using KL<GUM_SCALAR>::_hellinger;
    using KL<GUM_SCALAR>::_bhattacharya;

    using KL<GUM_SCALAR>::_klPQ;
    using KL<GUM_SCALAR>::_klQP;

    using KL<GUM_SCALAR>::_errorPQ;
    using KL<GUM_SCALAR>::_errorQP;
  };

  extern template class BruteForceKL<float>;
  extern template class BruteForceKL<double>;
} // namespace gum

#include <agrum/BN/algorithms/divergence/bruteForceKL.tcc>

#endif // GUM_BRUTE_FORCE_KL_H

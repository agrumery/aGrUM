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
* @brief algorithm for approximated computation KL divergence between BNs using GIBBS sampling
*
* @author Pierre-Henri Wuillemin
*
*/
#ifndef GUM_GIBBS_KL_H
#define GUM_GIBBS_KL_H

#include <agrum/BN/algorithms/divergence/KL.h>
#include <agrum/BN/particles/Gibbs.h>
#include <agrum/BN/algorithms/GibbsSettings.h>

namespace gum {

/**
* GibbsKL computes the KL divergence betweens 2 BNs using a pattern approximation : GIBBS sampling.
*
* KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The computations are made once. The second is for free :)
* BruteForce allows as well to compute in the same time the Hellinger distance (\f$ \sqrt{\sum_i (\sqrt{p_i}-\sqrt{q_i})^2}\f$) (Kokolakis and Nanopoulos, 2001).
*
* It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a case, KL keeps working but trace this error (errorPQ() and errorQP())
*
* @warning : convergence and stop criteria are designed w.r.t the main computation : KL(P||Q). The 2 others have no guarantee.
*
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

template<typename T_DATA> class GibbsKL:
            public GibbsSettings,
            public KL<T_DATA>,
            public particle::Gibbs<T_DATA> {
public:
    /* no default constructor */

    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize or compatible node sets.
     */
    GibbsKL ( const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q );

    /** copy constructor
     */
    GibbsKL ( const KL<T_DATA>& kl );


    /** destructor */
    ~GibbsKL ();

    using particle::Gibbs<T_DATA>::particle;
    using particle::Gibbs<T_DATA>::initParticle;
    using particle::Gibbs<T_DATA>::nextParticle;
    using particle::Gibbs<T_DATA>::bn;

protected:
    void _computeKL ( void );

    using KL<T_DATA>::_p;
    using KL<T_DATA>::_q;
    using KL<T_DATA>::_hellinger;

    using KL<T_DATA>::_klPQ;
    using KL<T_DATA>::_klQP;

    using KL<T_DATA>::_errorPQ;
    using KL<T_DATA>::_errorQP;

};

} // namespace gum

#include <agrum/BN/algorithms/divergence/GibbsKL.tcc>

#endif// GUM_GIBBS_KL_H

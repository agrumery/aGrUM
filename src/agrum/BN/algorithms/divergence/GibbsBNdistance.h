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
 * @brief algorithm for approximated computation KL divergence between BNs using
 *GIBBS
 *sampling
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN
 *
 */


#ifndef GUM_GIBBS_KL2_H
#define GUM_GIBBS_KL2_H

#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>
#include <agrum/core/approximations/approximationScheme.h>

#include <agrum/core/signal/signaler.h>

namespace gum {

  /**
   * GibbsKL computes the KL divergence betweens 2 BNs using an approximation
   *pattern:  GIBBS sampling.
   *
   * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The
   *computations are made once. The second is for free :)
   * GibbsKL allows as well to compute in the same time the Hellinger distance
   *(\f$
   *\sqrt{\sum_i (\sqrt{p_i}-\sqrt{q_i})^2}\f$) (Kokolakis and Nanopoulos, 2001)
   * and Bhattacharya distance (Kaylath,T. 1967)
   *
   * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a
   *case, KL
   *keeps working but trace this error (errorPQ() and errorQP()). In those cases,
   *Hellinger distance approximation is under-evaluated.
   *
   * @warning : convergence and stop criteria are designed w.r.t the main
   *computation
   *: KL(P||Q). The 3 others have no guarantee.
   *
   * snippets :
   * @code
   * gum::KL base_kl(net1,net2);
   * if (base_kl.difficulty()!=KL::HEAVY) {
   *  gum::ExactBNdistance kl(base_kl);
   *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
   * } else {
   *  gum::GibbsBNdistance kl(base_kl);
   *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
   * }
   * @endcode
   */

  template < typename GUM_SCALAR >
  class GibbsBNdistance
      : public BNdistance< GUM_SCALAR >
      , public ApproximationScheme
      , public GibbsOperator< GUM_SCALAR > {
    public:
    /* no default constructor */

    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize
     * or
     * compatible node sets.
     */


    GibbsBNdistance(const IBayesNet< GUM_SCALAR >& P,
                    const IBayesNet< GUM_SCALAR >& Q);

    /** copy constructor
     */
    explicit GibbsBNdistance(const BNdistance< GUM_SCALAR >& kl);

    /** destructor */
    ~GibbsBNdistance();

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfLowerBound Raised if b < 1.
     */
    void setBurnIn(Size b);

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    Size burnIn() const;

    protected:
    void _computeKL() final;

    using BNdistance< GUM_SCALAR >::_p;
    using BNdistance< GUM_SCALAR >::_q;
    using BNdistance< GUM_SCALAR >::_hellinger;
    using BNdistance< GUM_SCALAR >::_bhattacharya;
    using BNdistance< GUM_SCALAR >::_jsd;

    using BNdistance< GUM_SCALAR >::_klPQ;
    using BNdistance< GUM_SCALAR >::_klQP;

    using BNdistance< GUM_SCALAR >::_errorPQ;
    using BNdistance< GUM_SCALAR >::_errorQP;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GibbsBNdistance< double >;
#endif

}   // namespace gum

#include <agrum/BN/algorithms/divergence/GibbsKL_tpl.h>

#endif
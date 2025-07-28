/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief algorithm for approximated computation KL divergence between BNs using
 *GIBBS
 *sampling
 *
 * @author Paul ALAM & Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */


#ifndef GUM_GIBBS_KL2_H
#define GUM_GIBBS_KL2_H

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/BN/algorithms/divergence/BNdistance.h>
#include <agrum/BN/inference/tools/gibbsOperator.h>

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
  class GibbsBNdistance:
      public BNdistance< GUM_SCALAR >,
      public ApproximationScheme,
      public GibbsOperator< GUM_SCALAR > {
    public:
    /* no default constructor */

    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize
     * or
     * compatible node sets.
     */


    GibbsBNdistance(const IBayesNet< GUM_SCALAR >& P, const IBayesNet< GUM_SCALAR >& Q);

    /** copy constructor
     */
    explicit GibbsBNdistance(const BNdistance< GUM_SCALAR >& kl);

    /** destructor */
    ~GibbsBNdistance();

    /**
     * @brief Number of burn in for one iteration.
     * @param b The number of burn in.
     * @throw OutOfBounds Raised if b < 1.
     */
    void setBurnIn(Size b);

    /**
     * @brief Returns the number of burn in.
     * @return Returns the number of burn in.
     */
    Size burnIn() const;

    protected:
    void computeKL_() final;

    using BNdistance< GUM_SCALAR >::p_;
    using BNdistance< GUM_SCALAR >::q_;
    using BNdistance< GUM_SCALAR >::hellinger_;
    using BNdistance< GUM_SCALAR >::bhattacharya_;
    using BNdistance< GUM_SCALAR >::jsd_;

    using BNdistance< GUM_SCALAR >::klPQ_;
    using BNdistance< GUM_SCALAR >::klQP_;

    using BNdistance< GUM_SCALAR >::errorPQ_;
    using BNdistance< GUM_SCALAR >::errorQP_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class GibbsBNdistance< double >;
#endif

}   // namespace gum

#include <agrum/BN/algorithms/divergence/GibbsKL_tpl.h>

#endif

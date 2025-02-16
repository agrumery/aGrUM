/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


/**
 * @file
 * @brief algorithm for exact computation KL divergence between BNs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#ifndef GUM_BRUTE_FORCE_KL_H
#define GUM_BRUTE_FORCE_KL_H

#include <agrum/BN/algorithms/divergence/BNdistance.h>

namespace gum {

  /**
   * ExactBNdistance computes exactly the KL divergence betweens 2 BNs.
   *
   * ExactBNdistance should be used only if difficulty() gives an estimation (
   *KL_CORRECT
   *) of the needed time.
   * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The
   *computations are made once. The second is for free :)
   * ExactBNdistance allows as well to compute in the same time the Hellinger
   *distance
   *(\f$
   *\sqrt{\sum_i (\sqrt{p_i}-\sqrt{q_i})^2}\f$) (Kokolakis and Nanopoulos, 2001).
   *
   * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a
   *case, KL
   *keeps working but trace this error (errorPQ() and errorQP())?  *
   *
   * @warning This ExactBNdistance should be use only if
   *difficulty()==complexity::CORRECT or at most complexity::DIFFICULT ...
   * snippets :
   * @code
   * gum::KL base_kl(net1,net2);
   * if (base_kl.difficulty()!=KL::HEAVY) {
   *  gum::ExactBNdistance kl(base_kl);
   *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
   * } else {
   *  gum::GibbsKL kl(base_kl);
   *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
   * }
   * @endcode
   */

  template < typename GUM_SCALAR >
  class ExactBNdistance: public BNdistance< GUM_SCALAR > {
    public:
    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize
     * or
     * compatible node sets.
     */
    ExactBNdistance(const IBayesNet< GUM_SCALAR >& P, const IBayesNet< GUM_SCALAR >& Q);

    /** copy constructor
     */
    explicit ExactBNdistance(const BNdistance< GUM_SCALAR >& kl);

    /** destructor */
    virtual ~ExactBNdistance();

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
  extern template class ExactBNdistance< double >;
#endif

}   // namespace gum

#include <agrum/BN/algorithms/divergence/exactBNdistance_tpl.h>

#endif   // GUM_BRUTE_FORCE_KL_H

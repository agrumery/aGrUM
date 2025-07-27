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
 * @brief algorithm for KL divergence between BNs
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 */
#ifndef GUM_KL_H
#define GUM_KL_H

#include <agrum/BN/IBayesNet.h>

namespace gum {

  /**
   * @brief Complexity allows to characterize the awaited difficulty for an
   * algorithm
   * given a specific instance
   * Therefore this is not a theoretical characterization but rather a pragmatic
   * rate
   * of that very instance.
   */
  enum class Complexity : char { Heavy, Difficult, Correct };

  /**
   * @class KL
   *
   * @brief KL is the base class for KL computation betweens 2 BNs.
   *
   * KL is not virtual because it may be instantiated but protected methods throw
   *gum::OperationNotAllow : we do not know here how the computation is done.
   * Since this computation may be very difficult, KL.Complexity() give an
   *estimation
   *( KL_Complexity::Heavy,KL_Complexity::Difficult,KL_Complexity::Correct ) of
   *the
   *needed time.
   * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The
   *computations are made once. The second is for free :)
   *
   * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a
   *case, KL
   *keeps working but trace this error (errorPQ() and errorQP())?
   */
  template < typename GUM_SCALAR >
  class BNdistance {
// difficulty is chosen w.r.t the log10DomainSize of the BN
#define GAP_COMPLEXITY_KL_HEAVY_DIFFICULT   double(12.0)
#define GAP_COMPLEXITY_KL_DIFFICULT_CORRECT double(7.0)

    public:
    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize
     * or
     * compatible node sets.
     */
    BNdistance(const IBayesNet< GUM_SCALAR >& P, const IBayesNet< GUM_SCALAR >& Q);

    /** copy constructor
     */
    BNdistance(const BNdistance< GUM_SCALAR >& kl);

    /** destructor */
    virtual ~BNdistance();

    /**
     * return
     * KL::Complexity::Heavy,KL::Complexity::Difficult,KL::Complexity::Correct
     * depending on the BNs  _p_ and  _q_
     */
    Complexity difficulty() const;

    /// @name Accessors to results. The first call do the computations. The
    /// others do
    /// not.
    /// @{

    /// @return divergence KL(P||Q)
    double klPQ();

    /// @return the number of errors while processing divergence KL(P||Q)
    Size errorPQ();

    /// @return divergence KL(Q||P)
    double klQP();

    /// @return the number of errors while processing divergence KL(Q||P)
    Size errorQP();

    /// @return hellinger distance (@see
    /// http://en.wikipedia.org/wiki/Hellinger_distance)
    double hellinger();

    /// @return Bhattacharya distance (@see
    /// http://en.wikipedia.org/wiki/Bhattacharya_distance)
    double bhattacharya();

    /// @return Jensen-Shannon divergence(@see
    /// https://en.wikipedia.org/wiki/Jensen%E2%80%93Shannon_divergence)
    double jsd();

    /// @return p
    const IBayesNet< GUM_SCALAR >& p() const;

    /// @return q
    const IBayesNet< GUM_SCALAR >& q() const;
    /// @}

    protected:
    // should be pure virtual but using BNdistance directly is a way to delay the
    // choice between different computation scheme (@see ExactBNdistance)
    virtual void computeKL_();

    void process_();

    const IBayesNet< GUM_SCALAR >& p_;
    const IBayesNet< GUM_SCALAR >& q_;

    GUM_SCALAR klPQ_;
    GUM_SCALAR klQP_;

    Size errorPQ_;
    Size errorQP_;

    GUM_SCALAR hellinger_;
    GUM_SCALAR bhattacharya_;
    GUM_SCALAR jsd_;

    private:
    bool       _checkCompatibility_() const;
    Complexity _difficulty_;
    bool       _done_;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS

  extern template class BNdistance< double >;

#endif

}   // namespace gum

#include <agrum/BN/algorithms/divergence/BNdistance_tpl.h>

#endif   // GUM_KL_H

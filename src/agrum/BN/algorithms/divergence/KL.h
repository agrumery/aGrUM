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
* @brief algorithm for KL divergence between BNs
*
* @author Pierre-Henri WUILLEMIN
*
*/
#ifndef GUM_KL_H
#define GUM_KL_H

#include <agrum/core/sequence.h>
#include <agrum/BN/IBayesNet.h>

namespace gum {

  /**
   * @brief Complexity allows to characterize the awaited difficulty for an algorithm
   * given a specific instance
   * Therefore this is not a theoretical characterization but rather a pragmatic rate
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
  * Since this computation may be very difficult, KL.Complexity() give an estimation
  *( KL_Complexity::Heavy,KL_Complexity::Difficult,KL_Complexity::Correct ) of the
  *needed time.
  * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The
  *computations are made once. The second is for free :)
  *
  * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a case, KL
  *keeps working but trace this error (errorPQ() and errorQP())?
  */
  template <typename GUM_SCALAR> class KL {
// difficulty is chosen w.r.t the log10DomainSize of the BN
#define GAP_COMPLEXITY_KL_HEAVY_DIFFICULT double(12.0)
#define GAP_COMPLEXITY_KL_DIFFICULT_CORRECT double(7.0)
    public:
    /** constructor must give 2 BNs
     * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize or
     * compatible node sets.
     */
    KL(const IBayesNet<GUM_SCALAR> &P, const IBayesNet<GUM_SCALAR> &Q);

    /** copy constructor
     */
    KL(const KL<GUM_SCALAR> &kl);

    /** destructor */
    ~KL();

    /**
     * return KL::Complexity::Heavy,KL::Complexity::Difficult,KL::Complexity::Correct
     * depending on the BNs __p and __q
     */
    Complexity difficulty() const;

    /// @name Accessors to results. The first call do the computations. The others do
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

    /// @return p
    const IBayesNet<GUM_SCALAR> &p(void) const;

    /// @return q
    const IBayesNet<GUM_SCALAR> &q(void) const;
    /// @}

    protected:
    // should be pure virtual but using KL directly is a way to delay the choice
    // between different computation scheme (@see BruteForceKL)
    virtual void _computeKL(void);
    void _process();

    const IBayesNet<GUM_SCALAR> &_p;
    const IBayesNet<GUM_SCALAR> &_q;

    GUM_SCALAR _klPQ;
    GUM_SCALAR _klQP;
    GUM_SCALAR _hellinger;
    GUM_SCALAR _bhattacharya;

    Size _errorPQ;
    Size _errorQP;

    private:
    bool __checkCompatibility() const;
    Complexity __difficulty;
    bool __done;
  };

  extern template class KL<float>;
  extern template class KL<double>;
} // namespace gum

#include <agrum/BN/algorithms/divergence/KL.tcc>

#endif // GUM_KL_H

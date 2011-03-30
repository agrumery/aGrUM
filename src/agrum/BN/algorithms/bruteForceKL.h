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
* @author Pierre-Henri Wuillemin
*
*/
#ifndef GUM_BRUTE_FORCE_KL_H
#define GUM_BRUTE_FORCE_KL_H

#include <agrum/BN/algorithms/defaultKL.h>

namespace gum {

  /**
  * BruteForceKL computes exactly the KL divergence betweens 2 BNs.
  *
  * BruteForceKL should be used only if difficulty() gives an estimation ( KL_CORRECT ) of the needed time.
  * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The computations are made once. The second is for free :)
  *
  * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a case, KL keeps working but trace this error (errorPQ() and errorQP())?
  * 
  * BruteForce allows as well to compute in the same time the Hellinger distance (\f$ \sqrt{\sum_i (\sqrt{p_i}-\sqrt{q_i})^2}\f$) (Kokolakis and Nanopoulos, 2001).
  * 
  * @warning This BruteForceKL should be use only if difficulty()==KL::CORRECT or at most KL::DIFFICULT ...
  * snippets :
  * @code
  * gum::DefaultKL dkl(net1,net2);
  * if (dkl.difficulty()!=KL::HEAVY) {
  *  gum::BruteForceKL kl(dkl);
  *  std::cout<<"KL net1||net2 :"<<kl.klPQ()<<std::endl;
  * } else {
  *  // other KL algorithm (IS for instance)
  * }
  * @endcode
  */

  template<typename T_DATA> class BruteForceKL:public DefaultKL<T_DATA> {
    public:
      /** no default constructor
       * @throw gum::OperationNotAllowed since this default constructor is not authorized
       */
      BruteForceKL ();

      /** constructor must give 2 BNs
       * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize or compatible node sets.
       */
      BruteForceKL (const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q);

      /** copy constructor
       */
      BruteForceKL (const DefaultKL<T_DATA>& kl);


      /** destructor */
      ~BruteForceKL ();
      
      double hellinger();

    protected:
      void _computeKL (void);
      
      double _hellinger;

      using DefaultKL<T_DATA>::_p;
      using DefaultKL<T_DATA>::_q;

      using DefaultKL<T_DATA>::_klPQ;
      using DefaultKL<T_DATA>::_klQP;

      using DefaultKL<T_DATA>::_errorPQ;
      using DefaultKL<T_DATA>::_errorQP;

    private:
      //// synchronize Iq with Ip
      void __synchroInstantiations (Instantiation& Iq,const Instantiation& Ip) const;
  };

} // namespace gum

#include <agrum/BN/algorithms/bruteForceKL.tcc>

#endif// GUM_BRUTE_FORCE_KL_H

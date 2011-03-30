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
* @author Pierre-Henri Wuillemin
*
*/
#ifndef GUM_KL_H
#define GUM_KL_H

#include <agrum/core/sequence.h>
#include <agrum/BN/BayesNet.h>

extern std::vector< float > v;

namespace gum {

  /**
  * DefaultKL is the base class for KL computation betweens 2 BNs.
  *
  * DefaultKL is not virtual because it may be instantiated but protected methods throw gum::OperationNotAllow : we do not know here how the computation is done.
  * Since this computation may be very difficult, KL.difficulty() give an estimation ( KL_HEAVY,KL_DIFFICULT,KL_CORRECT ) of the needed time.
  * KL.process() computes KL(P||Q) using klPQ() and KL(Q||P) using klQP(). The computations are made once. The second is for free :)
  *
  * It may happen that P*ln(P/Q) is not computable (Q=0 and P!=0). In such a case, KL keeps working but trace this error (errorPQ() and errorQP())?
  */

  namespace KL {
    enum difficulty {HEAVY,DIFFICULT,CORRECT};
  }

  template<typename T_DATA> class DefaultKL {
      static const int GAP_heavy_difficult = 12;
      static const int GAP_difficult_correct = 7;
    public:

      /** no default constructor
       * @throw gum::OperationNotAllowed since this default constructor is not authorized
       */
      DefaultKL ();

      /** constructor must give 2 BNs
       * @throw gum::OperationNotAllowed if the 2 BNs have not the same domainSize or compatible node sets.
       */
      DefaultKL (const BayesNet<T_DATA>& P,const BayesNet<T_DATA>& Q);

      /** copy constructor
       */
      DefaultKL (const DefaultKL< T_DATA >& kl);

      /** destructor */
      ~DefaultKL ();

      /**
       * return KL::HEAVY,KL::DIFFICULT,KL::CORRECT depending on the BNs __p and __q
       */
      KL::difficulty difficulty() const;


      /// @name Accessors to results. The first call do the computations. The others do not.
      /// @{

      /// @return divergence KL(P||Q)
      double klPQ();

      /// @return divergence KL(Q||P)
      double klQP();

      /// @return true if erros while processing divergence KL(P||Q)
      bool errorPQ();

      /// @return true if erros while processing divergence KL(Q||P)
      bool errorQP();
      /// @}

    protected:
      virtual void _computeKL (void);

      const BayesNet<T_DATA> _p;
      const BayesNet<T_DATA> _q;

      double _klPQ;
      double _klQP;

      bool _errorPQ;
      bool _errorQP;

    private:
      void __process();

      bool __checkCompatibility() const;
      KL::difficulty __difficulty;
      bool __done;

  };
} //namespace gum

#include <agrum/BN/algorithms/defaultKL.tcc>

#endif //GUM_KL_H

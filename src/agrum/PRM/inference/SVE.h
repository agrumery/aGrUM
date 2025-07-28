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
 * @brief Headers of SVE (Structured Variable Elimination).
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef GUM_SVE_H
#define GUM_SVE_H

#include <vector>

#include <agrum/base/graphs/algorithms/triangulations/partialOrderedTriangulation.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/PRM/classBayesNet.h>
#include <agrum/PRM/inference/PRMInference.h>
#include <agrum/PRM/instanceBayesNet.h>

namespace gum {
  namespace prm {

    /**
     * @class SVE
     * @headerfile SVE.h <agrum/PRM/SVE.h>
     * @brief This class is an implementation of the Structured Variable
     *Elimination
     *        algorithm on PRM<GUM_SCALAR>.
     *
     */
    template < typename GUM_SCALAR >
    class SVE: public PRMInference< GUM_SCALAR > {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default Constructor.
      SVE(const PRM< GUM_SCALAR >& prm, const PRMSystem< GUM_SCALAR >& system);

      /// Destructor.
      ~SVE();

      /// @}
      // ========================================================================
      /// @name Getters & setters.
      // ========================================================================
      /// @{

      /// Returns the name of the current inference algorithm
      virtual std::string name() const;

      /// @}

      protected:
      // ========================================================================
      /// @name Query methods.
      // ========================================================================
      /// @{

      /// Code alias.
      using Chain = typename PRMInference< GUM_SCALAR >::Chain;

      /// See PRMInference<GUM_SCALAR>::evidenceAdded_().
      virtual void evidenceAdded_(const Chain& chain);

      /// See PRMInference<GUM_SCALAR>::evidenceRemoved_().
      virtual void evidenceRemoved_(const Chain& chain);

      /// See PRMInference<GUM_SCALAR>::posterior_().
      virtual void posterior_(const Chain& chain, Tensor< GUM_SCALAR >& m);

      /// See PRMInference<GUM_SCALAR>::joint_().
      virtual void joint_(const std::vector< Chain >& queries, Tensor< GUM_SCALAR >& j);

      /// @}

      private:
      /// Code alias
      using BucketSet         = Set< Tensor< GUM_SCALAR >* >;
      using BucketSetIterator = typename Set< Tensor< GUM_SCALAR >* >::iterator_safe;
      using ArraySetIterator  = typename Set< MultiDimArray< GUM_SCALAR >* >::iterator_safe;

      HashTable< const PRMClass< GUM_SCALAR >*, std::vector< NodeId >* > _elim_orders_;

      HashTable< const PRMClass< GUM_SCALAR >*, BucketSet* > _lifted_pools_;

      Sequence< std::string >* _class_elim_order_;

      HashTable< const PRMInstance< GUM_SCALAR >*, gum::VariableSet* > _delayedVariables_;

      /// Some variable must be delayed for more than one
      /// PRMInstance<GUM_SCALAR>,
      /// when
      /// the delayed
      /// variable counter reach 0 it can be eliminated.
      HashTable< std::string, Size > _delayedVariablesCounters_;

      BucketSet _lifted_trash_;

      /// @name Inference sub methods.

      /// @{

      void _eliminateNodes_(const PRMInstance< GUM_SCALAR >* query,
                            NodeId                           id,
                            BucketSet&                       pool,
                            BucketSet&                       trash);

      void _eliminateNodesDownward_(const PRMInstance< GUM_SCALAR >*          from,
                                    const PRMInstance< GUM_SCALAR >*          i,
                                    BucketSet&                                pool,
                                    BucketSet&                                trash,
                                    List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                                    Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
                                    Set< const PRMInstance< GUM_SCALAR >* >&  eliminated);

      void _eliminateNodesUpward_(const PRMInstance< GUM_SCALAR >*          i,
                                  BucketSet&                                pool,
                                  BucketSet&                                trash,
                                  List< const PRMInstance< GUM_SCALAR >* >& elim_list,
                                  Set< const PRMInstance< GUM_SCALAR >* >&  ignore,
                                  Set< const PRMInstance< GUM_SCALAR >* >&  eliminated);

      void _eliminateNodesWithEvidence_(const PRMInstance< GUM_SCALAR >* i,
                                        BucketSet&                       pool,
                                        BucketSet&                       trash,
                                        Set< NodeId >*                   delayedVars = 0);

      void _eliminateDelayedVariables_(const PRMInstance< GUM_SCALAR >* i,
                                       BucketSet&                       pool,
                                       BucketSet&                       trash);

      void _insertLiftedNodes_(const PRMInstance< GUM_SCALAR >* i,
                               BucketSet&                       pool,
                               BucketSet&                       trash);

      void _variableElimination_(const PRMInstance< GUM_SCALAR >* i,
                                 BucketSet&                       pool,
                                 BucketSet&                       trash,
                                 Set< NodeId >*                   delayedVars = 0);

      /// Returns true if second can be eliminated before first.
      bool _checkElimOrder_(const PRMInstance< GUM_SCALAR >* first,
                            const PRMInstance< GUM_SCALAR >* second);

      void _initElimOrder_();

      void _insertEvidence_(const PRMInstance< GUM_SCALAR >* i, BucketSet& pool);

      /// When there is a loop in the references some variable elimination
      /// must be delayed, this methods add such variable to  _delayedVariables_
      /// to keep track of them.
      /// @param i An PRMInstance<GUM_SCALAR> with a child of j->get(id).
      /// @param j The PRMInstance<GUM_SCALAR> with the delayed variable.
      /// @param id The NodeId of the delayed variable.
      void _addDelayedVariable_(const PRMInstance< GUM_SCALAR >* i,
                                const PRMInstance< GUM_SCALAR >* j,
                                NodeId                           id);

      std::vector< NodeId >& _getElimOrder_(const PRMClass< GUM_SCALAR >& c);

      Tensor< GUM_SCALAR >* _getAggTensor_(const PRMInstance< GUM_SCALAR >*  i,
                                           const PRMAggregate< GUM_SCALAR >* agg);

      void        _initLiftedNodes_(const PRMClass< GUM_SCALAR >& c);
      std::string _trim_(const std::string& s);

      /// @}
    };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
    extern template class SVE< double >;
#endif


  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/inference/SVE_tpl.h>

#endif /* GUM_SVE_H */

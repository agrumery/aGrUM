/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
/** @file
 * @brief the pattern used by all binary MultiDimDecisionDiagramBases operators
 *
 * @author Jean-Christophe MAGNAN
 */

// check if we allowed these patterns to be used
#ifndef GUM_OPERATOR_PATTERN_ALLOWED

#warning To use operatorPattern4MultiDimDecisionDiagram.h, you must define GUM_OPERATOR_PATTERN_ALLOWED

#else

#include <agrum/multidim/multiDimImplementation.h>
#include <agrum/multidim/multiDimDecisionDiagramBase.h>
#include <agrum/multidim/multiDimDecisionDiagramFactoryBase.h>
#include <agrum/multidim/patterns/DDUtility/o4DDMiscellaneous.h>

namespace gum {

/// a specialized function for combining two MultiDimDecisionDiagrams

#ifdef GUM_MULTI_DIM_OPERATOR_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T

  template <typename T>
  MultiDimDecisionDiagramBase<T> *
  GUM_MULTI_DIM_OPERATOR_NAME(const MultiDimDecisionDiagramBase<T> *t1,
                              const MultiDimDecisionDiagramBase<T> *t2) {
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_NAME_F
#define GUM_MULTI_DIM_OPERATOR_TYPE T

    template <typename T>
    MultiDimDecisionDiagramBase<T> *GUM_MULTI_DIM_OPERATOR_NAME_F(
        const MultiDimDecisionDiagramBase<T> *t1,
        const MultiDimDecisionDiagramBase<T> *t2,
        const T (*f)(const T &, const T &)) {
#endif

#ifdef GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME
#define GUM_MULTI_DIM_OPERATOR_TYPE T

      template <typename T>
      MultiDimImplementation<T> *GUM_MULTI_DIM_OPERATOR_IMPL2DECISION_DIAGRAM_NAME(
          const MultiDimImplementation<T> *tt1,
          const MultiDimImplementation<T> *tt2) {
        const MultiDimDecisionDiagramBase<T> *t1 =
            reinterpret_cast<const MultiDimDecisionDiagramBase<T> *>(tt1);
        const MultiDimDecisionDiagramBase<T> *t2 =
            reinterpret_cast<const MultiDimDecisionDiagramBase<T> *>(tt2);
#endif

        MultiDimDecisionDiagramBase<GUM_MULTI_DIM_OPERATOR_TYPE> *ret = nullptr;

        if ((t1 == nullptr || t1->empty()) && (t2 == nullptr || t2->empty()))
          return ret;

        if (t1 == nullptr || t1->empty()) {
          ret = reinterpret_cast<MultiDimDecisionDiagramBase<T> *>(t2->newFactory());
          ret->copy(*t2);
          return ret;
        }

        if (t2 == nullptr || t2->empty()) {
          ret = reinterpret_cast<MultiDimDecisionDiagramBase<T> *>(t1->newFactory());
          ret->copy(*t1);
          return ret;
        }

        //     std::cout << "Début opération" << std::endl;
        NonOrderedOperatorData<T> opData;
        opData.initialize(t1, t2);

#ifdef O4DDWITHORDER
        GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(opData, "");
#else
  GUM_MULTI_DIM_DECISION_DIAGRAM_RECUR_FUNCTION(opData, nullptr, "");
#endif
        //     std::cout << "Fin opération" << std::endl;
        ret = opData.factory->getMultiDimDecisionDiagram(false);

        return ret;
      }
#undef GUM_MULTI_DIM_OPERATOR_TYPE
    }
#endif /* GUM_OPERATOR_PATTERN_ALLOWED */

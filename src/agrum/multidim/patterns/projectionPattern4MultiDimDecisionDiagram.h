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
 * @brief the pattern used by all the projections of multidimensional tables
 *
 * @author Jean-Christophe MAGNAN and Pierre-Henri WUILLEMIN
 */

#include <agrum/multidim/instantiation.h>
// check if we allowed these patterns to be used
#ifndef GUM_PROJECTION_PATTERN_ALLOWED

#warning To use projectionPattern, you must define GUM_PROJECTION_PATTERN_ALLOWED

#else
#include <agrum/multidim/patterns/DDUtility/DDModeMacro.h>
namespace gum {

/// a specialized function for projecting a multiDimDecisionDiagram over a subset of
/// its vars

#ifdef GUM_DECISION_DIAGRAM_PROJECTION_NAME

#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR

  template <typename GUM_SCALAR>
  MultiDimDecisionDiagramBase<GUM_SCALAR> *GUM_DECISION_DIAGRAM_PROJECTION_NAME(
      const MultiDimDecisionDiagramBase<GUM_SCALAR> *table,
      const Set<const DiscreteVariable *> &delVars) {

#endif

#ifdef GUM_DECISION_DIAGRAM_PROJECTION_IMPL2DECISION_DIAGRAM_NAME

#define GUM_MULTI_DIM_PROJECTION_TYPE GUM_SCALAR

    template <typename GUM_SCALAR>
    MultiDimImplementation<GUM_SCALAR> *
    GUM_DECISION_DIAGRAM_PROJECTION_IMPL2DECISION_DIAGRAM_NAME(
        const MultiDimImplementation<GUM_SCALAR> *ttable,
        const Set<const DiscreteVariable *> &delVars) {

      const MultiDimDecisionDiagramBase<GUM_SCALAR> *table =
          reinterpret_cast<const MultiDimDecisionDiagramBase<GUM_SCALAR> *>(ttable);

#endif
      MultiDimDecisionDiagramFactoryBase<GUM_SCALAR> *factory = table->getFactory();
      factory->setMultiDimDecisionDiagram(table);
      Sequence<const DiscreteVariable *> varSeq = table->variablesSequence();

      auto endSeqIter = varSeq.rbegin();
      bool modified = true;

      while (modified && endSeqIter != varSeq.rend()) {
        auto varIter = varSeq.begin();
        auto nextVarIter = varIter;
        modified = false;

        while (varIter != endSeqIter) {
          ++nextVarIter;

          if (nextVarIter != varSeq.end()) {
            if (delVars.exists(*varIter) && !delVars.exists(*nextVarIter)) {
              factory->swap(*varIter, *nextVarIter);
              varSeq.swap(varSeq.pos(*varIter), varSeq.pos(*nextVarIter));
              modified = true;
            }
          }

          ++varIter;
        }

        --endSeqIter;
      }

      MultiDimDecisionDiagramBase<GUM_MULTI_DIM_PROJECTION_TYPE> *ret =
          factory->getMultiDimDecisionDiagram();
      factory->clear();

      for (const auto var : delVars)
        varSeq.erase(var);

      factory->setVariablesSequence(varSeq);
      HashTable<NodeId, NodeId> explorationTable;
      Idx nbOperation = 1;

      for (const auto var : delVars)
        nbOperation *= var->domainSize();

#ifdef P4DDDEBUG
      factory->setRootNode(GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
          ret, factory, ret->root(), false, explorationTable, delVars, nbOperation,
          ""));
#else
  factory->setRootNode(GUM_DECISION_DIAGRAM_PROJECTION_EXPLORATION_FUNCTION(
      ret, factory, ret->root(), false, explorationTable, delVars, nbOperation));
#endif
      delete ret;
      ret = factory->getMultiDimDecisionDiagram();
      delete factory;
      return ret;
    }

#undef GUM_MULTI_DIM_PROJECTION_TYPE
  } /* end of namespace GUM */
#endif /* GUM_PROJECTION_PATTERN_ALLOWED */

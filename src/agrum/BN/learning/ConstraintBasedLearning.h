/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
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
 * @brief Abstract base class for constraint-based structure learning algorithms.
 *
 * ConstraintBasedLearning factors the code common to all constraint-based
 * algorithms (MIIC, PC, FCI, …): constraint management, template-method
 * implementations of learnPDAG / learnDAG / learnBN, and shared graph
 * utilities. Subclasses must implement learnSkeleton() and learnMixedStructure().
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_CONSTRAINT_BASED_LEARNING_H
#define GUM_CONSTRAINT_BASED_LEARNING_H

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include <agrum/config.h>

#include <agrum/base/core/approximations/approximationScheme.h>
#include <agrum/base/graphs/algorithms/MeekRules.h>

#define GUM_SL_EMIT(x, y, action, explain)                                                \
  {                                                                                       \
    std::ostringstream action_stream;                                                     \
    action_stream << action;                                                              \
    std::ostringstream explain_stream;                                                    \
    explain_stream << explain;                                                            \
    GUM_EMIT4(onStructuralModification, x, y, action_stream.str(), explain_stream.str()); \
  }

namespace gum {

  namespace learning {

    using ThreePoints = std::tuple< NodeId, NodeId, NodeId >;

    /**
     * @class ConstraintBasedLearning
     * @brief Abstract base class for constraint-based structure learning algorithms.
     *
     * Provides constraint management (forbidden/mandatory arcs, max indegree),
     * template-method implementations of learnPDAG, learnDAG and learnBN,
     * and shared graph utilities.
     *
     * Subclasses must implement learnSkeleton() and learnMixedStructure().
     * learnSkeleton() is exposed as a separate step because some algorithms
     * (e.g., PC) expose skeleton construction as an independent user-facing
     * operation, while others (e.g., MIIC) call it internally from
     * learnMixedStructure().
     *
     * @ingroup learning_group
     */
    class ConstraintBasedLearning: public ApproximationScheme {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      ConstraintBasedLearning();
      explicit ConstraintBasedLearning(int maxLog);
      ConstraintBasedLearning(const ConstraintBasedLearning&);
      ConstraintBasedLearning(ConstraintBasedLearning&&);
      ~ConstraintBasedLearning() override;

      /// @}

      ConstraintBasedLearning& operator=(const ConstraintBasedLearning&);
      ConstraintBasedLearning& operator=(ConstraintBasedLearning&&);

      // ##########################################################################
      /// @name Constraint setters
      // ##########################################################################
      /// @{

      void setForbiddenGraph(const gum::DiGraph& forbidGraph);
      void setMandatoryGraph(const gum::DAG& mandaGraph);
      void setMaxIndegree(gum::Size n);
      void addConstraints(const HashTable< std::pair< NodeId, NodeId >, char >& constraints);
      const std::vector< Arc > latentVariables() const;

      /// @}

      // ##########################################################################
      /// @name Learning — template methods (call the pure virtual interface)
      // ##########################################################################
      /// @{

      /// learns the essential graph (CPDAG)
      PDAG learnPDAG(MixedGraph graph);

      /// learns a DAG
      DAG learnDAG(MixedGraph graph);

      /// learns structure then estimates parameters
      template < GUM_Numeric GUM_SCALAR = double, typename PARAM_ESTIMATOR >
      BayesNet< GUM_SCALAR > learnBN(PARAM_ESTIMATOR& estimator, MixedGraph graph);

      /// @}

      Signaler< gum::NodeId, gum::NodeId, std::string, std::string > onStructuralModification;

      protected:
      // ##########################################################################
      /// @name Pure virtual interface — subclasses must implement
      // ##########################################################################
      /// @{

      virtual MixedGraph learnSkeleton(MixedGraph graph) = 0;

      virtual MixedGraph learnMixedStructure(MixedGraph graph) = 0;

      /// @}

      // ##########################################################################
      /// @name Graph utilities
      // ##########################################################################
      /// @{

      static bool _existsDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);
      static bool _existsNonTrivialDirectedPath_(const MixedGraph& graph, NodeId n1, NodeId n2);
      void        orientDoubleHeadedArcs_(MixedGraph& mg);
      std::vector< ThreePoints > unshieldedTriples_(const MixedGraph& graph);

      void applyStructuralConstraints_(MixedGraph& graph);

      /// Builds a complete MixedGraph on the nodes of template_graph, minus edges
      /// forbidden by structural constraints. Every constraint-based skeleton phase
      /// must start from this graph so forbidden pairs are never tested by CI.
      MixedGraph initGraph_(const MixedGraph& template_graph);

      gum::MeekRules meekRules_;

      /// @}

      // ##########################################################################
      /// @name Constraint checks
      // ##########################################################################
      /// @{

      bool isForbiddenArc_(NodeId x, NodeId y) const;
      bool isForbiddenEdge_(NodeId x, NodeId y) const;
      bool isMaxIndegree_(const MixedGraph& graph, NodeId x);
      bool isArcValid_(const MixedGraph& graph, NodeId x, NodeId y);

      /// @}

      // ##########################################################################
      /// @name Common state (accessible to subclasses)
      // ##########################################################################

      gum::DiGraph                                   _forbiddenGraph_;
      gum::DAG                                       _mandatoryGraph_;
      HashTable< std::pair< NodeId, NodeId >, char > _initialMarks_;
      std::vector< Arc >                             _latentCouples_;
      int                                            _maxLog_{100};
      const std::vector< NodeId >                    _emptySet_;

      private:
      gum::Size _maxIndegree_{std::numeric_limits< gum::Size >::max()};
    };

  } /* namespace learning */

} /* namespace gum */

#include <agrum/BN/learning/ConstraintBasedLearning_tpl.h>

#endif /* GUM_CONSTRAINT_BASED_LEARNING_H */
